
#define WIN32_LEAN_AND_MEAN  // Prevent conflicts in Window.h and WinSock.h
#include <assert.h>
#include "QSP.h"

// We build up different sets of roots, but in the end, we combine them into one indexed array as:
// [vTargetRoots || wTargetRoots || wireCheckerTargetRoots]
QSP::QSP(BoolCircuit* circuit, Field* field) {
	//this->encoding = encoding;
	//this->field = encoding->getSrcField();
	this->field = field;
	this->circuit = circuit;
	this->polyIDctr = 0;

	assert(circuit->gates.size() > 1);	// Lagrange assumes more than 1 root, so we need more than one gate	
	
	V.reserve(circuit->gates.size());		// Give V a heads-up.  We'll want at least one (probably more) polys/gate
	newSparsePoly();	// Allocate a slot for v_0
	
	// Process each gate
	int gateID = 0;
	processGates();
	
	//print();

	// We just built V.  Use it as a model for W.	
	duplicateVtoW();

#ifdef VERBOSE
	printf("Prior to strengthening, the degree is %d\n", vTargetRoots.size());
#endif

	// Finally add the wire checker to strengthen the QSP
	this->strengthen();

	// Update the QSP's stats
	size = (int) (V.size() - 1); // V_0 doesn't count against the QSP's size
	degree = (int) (vTargetRoots.size() + wTargetRoots.size() + wireCheckerTargetRoots.size());

#ifdef VERBOSE
	printf("Constructed a QSP with size %d and degree %d\n", size, degree);
#endif

	// Finally, put the roots in a more accessible container	
	targetRoots = new FieldElt[degree];
	int rootIndex = 0;
	for (int i = 0; i < vTargetRoots.size(); i++) {	
		field->copy(vTargetRoots[i].elt, targetRoots[rootIndex++]);
	}
	for (int i = 0; i < wTargetRoots.size(); i++) {	
		field->copy(wTargetRoots[i].elt, targetRoots[rootIndex++]);
	}
	for (int i = 0; i < wireCheckerTargetRoots.size(); i++) {	
		field->copy(wireCheckerTargetRoots[i].elt, targetRoots[rootIndex++]);
	}
}

// W has the same sparse polynomials as V, but they're defined in terms of a separate set of roots.
void QSP::duplicateVtoW() {
	// Generate w's roots
	genFreshRoots((int)vTargetRoots.size(), wTargetRoots); //[kunxian]: equals to 9*s

	// Duplicate the polynomials in terms of the new roots
	W.resize(V.size()); //[kunxian]: equals to 12*s
	for (int i = 0; i < V.size(); i++) {
		W[i] = new SparsePolynomial;
		W[i]->id = V[i]->id;

		for (V[i]->iterStart(); !V[i]->iterEnd(); V[i]->iterNext()) {	
			//	W draws from it's own roots, so we need to increment the ID far enough to get beyond V's roots
			W[i]->addNonZeroRoot(V[i]->curRootID() + (unsigned long) vTargetRoots.size(), *V[i]->curRootVal(), field);
		}
	}	
}


QSP::~QSP() {
	for (int i = 0; i < this->size+1; i++) {
		delete V[i];
		delete W[i];		
	}

	delete [] targetRoots;
}

int QSP::genFreshRoots(int num, EltVector& roots) {
	int oldSize = (int) roots.size();
	roots.resize(roots.size()+num);

	for (int i = 0; i < num; i++) {
		field->assignFreshElt(&roots[oldSize + i].elt);
	}

	return oldSize;
}

SparsePolynomial* QSP::newSparsePoly() {
	SparsePolynomial* ret = new SparsePolynomial();
	ret->id = polyIDctr++;
	V.resize(polyIDctr);
	V[polyIDctr-1] = ret;
	return ret;
}

// Returns a pointer to V_0
SparsePolynomial* QSP::getSPtarget() {
	return V.front();
}

void QSP::processGates() {
	for (BoolGateList::iterator iter = circuit->gates.begin();
	     iter != circuit->gates.end();
			 iter++) {
		(*iter)->generateSP(this);
	}
}

void QSP::printPolynomials(SparsePolyPtrVector& polys, int numPolys) {
	//for (int i = 0; i < numPolys; i++) {
	//	if (polys[i]->nonZeroRoots.size() > 0) {
	//		printf("%d\t", i);
	//		for (NonZeroRoots::iterator iter = polys[i]->nonZeroRoots.begin();
	//		     iter != polys[i]->nonZeroRoots.end();
	//		     iter++) {
	//			printf("(%lld, ", (*iter).first);
	//			field->print((*iter).second->value);
	//			printf("), ");	
	//		}
	//		printf("\n");
	//	} else {
	//		printf("%d\t All zero\n", i);			
	//	}
	//}
}

void QSP::print() {
	printf("Size: %d, degree: %d\n", size, degree);
	printf("Roots: {");

	for (int i = 0; i < vTargetRoots.size(); i++) {
		field->print(vTargetRoots[i].elt);
		if (i < vTargetRoots.size() - 1) {
			printf(", ");
		}
	}
	printf("}\n");

	printf("V index : V non-zero roots\n");
	printPolynomials(V, (int)V.size());
}

// Convert this QSP into a strong QSP (by adding a wire checker)
void QSP::strengthen() {
	FieldElt one;
	field->one(one);

	/* 
	 * I_ij = zero and one polynomials of wire i  
	 * v_k(x) = vprime_k(x) (mod tprime(x)) for k = 1, ..., m
	 * w_k(x) = wprime_k(x) (mod tprime(x))
	 *
	 * v_k(x) = 0 (mod tw(x))
	 * w_k(x) = 0 (mod tv(x))
	 *
	 * v_0(x) = 1 (mod tw(x))
	 * w_0(x) = 1 (mod tv(x))
	 *
	 * v_0(x) = 0 (mod tprime(x))
	 * w_0(x) = 0 (mod tprime(x))
	 */

	/* 
	 * 对每个wire都计算一个wire consistency checker
	 * 然后将所有的checker经过CRT过程构成最后的checker
	 * 
	 */
#ifndef DEBUG_STRENGTHEN
	// For each wire in the circuit
	for (BoolWireVector::iterator witer = circuit->wires.begin();
	     witer != circuit->wires.end();
		 witer++) {
		int num0indices = (int) (*witer)->zero.size();
		int num1indices = (int) (*witer)->one.size();
		int maxIndices = max(num0indices, num1indices);	// = L_max
		int numNewRoots = 3 * maxIndices - 2;			// = L'

		// Create two new sets of roots of size numNewRoots
		int firstNewRootIndex = genFreshRoots(numNewRoots * 2, wireCheckerTargetRoots);

		int rootIndex = firstNewRootIndex;
		int rootIndexOffset = (int) (vTargetRoots.size() + wTargetRoots.size());	// These roots are ID'ed starting after the V and W roots

		// For k \in I_0
		for (SparsePolyPtrVector::iterator iter = (*witer)->zero.begin();
			 iter != (*witer)->zero.end();
			 iter++) {
			V[(*iter)->id]->addNonZeroRoot(numNewRoots + rootIndex + rootIndexOffset, one, field); // v_k(r^(1)_k) = 1
			W[(*iter)->id]->addNonZeroRoot(rootIndex + rootIndexOffset, one, field);	// w_k(r^(0)_k) = 1

			rootIndex++;
		}

		rootIndex = firstNewRootIndex;
		// For k \in I_1
		for (SparsePolyPtrVector::iterator iter = (*witer)->one.begin();
			 iter != (*witer)->one.end();
			 iter++) {
			V[(*iter)->id]->addNonZeroRoot(rootIndex + rootIndexOffset, one, field);  // v_k(r^(0)_k) = 1
			W[(*iter)->id]->addNonZeroRoot(numNewRoots + rootIndex + rootIndexOffset, one, field); // w_k(r^(1)_k) = 1

			rootIndex++;
		}		
	}

#endif

	// We also need to update v_0 and w_0 when we combine everything together

	// v_0(x) = 1 mod t^{W}(x)
	for (int i = 0; i < wTargetRoots.size(); i++) {
		V[0]->addNonZeroRoot((int) (i + vTargetRoots.size()), one, field);
	}

	// w_0(x) = 1 mod t^{V}(x)
	for (int i = 0; i < vTargetRoots.size(); i++) {
		W[0]->addNonZeroRoot(i, one, field);
	}
}

FieldElt* QSP::createLagrangeDenominators() {
	FieldElt* factorial = new FieldElt[this->degree];
	FieldElt* denominators = new FieldElt[this->degree];

	// We exploit the fact that root[k] = root[k-1] + 1
	// which means that denom_i = \prod_{k=1}^{i-1} k \prod_{k=1}^{d-i} (-k),
	// where the second term = (-1)^{d-1} \prod_{k=1}^{d-i} k
	// Thus, we compute 1!, 2!, 3!, ..., degree!, and store it s.t. factorial[i] = (i+1)!
	FieldElt ctr, zero, one;
	field->one(factorial[0]);
	field->one(ctr);
	field->zero(zero);
	field->one(one);
	for (int k = 1; k < this->degree; k++) { // For each r_k
		field->add(ctr, one, ctr);
		field->mul(factorial[k-1], ctr, factorial[k]);
	}

	for (int i = 0; i < this->degree; i++) {	// For each L_i
		// Calculate the denominator = (k-1)! * (d-k)! * (-1)^{d-1}
		int k = i + 1;	// 1-based index
		int dMinusK = this->degree - k - 1 >= 0 ? this->degree - k - 1 : 0;
		int kFact = k - 1 - 1 > 0 ? k - 2 : 0;
		field->mul(factorial[kFact], factorial[dMinusK], denominators[i]);	// -1 for 0-based arrawy
		if ((this->degree - k) % 2 == 1) { // Need a -1 term
			field->sub(zero, denominators[i], denominators[i]);
		}
	}

	delete [] factorial;

	return denominators;
}

