#include "Types.h"
#include <assert.h>
#include "BoolGate.h"
//#include "QSP.h"

BoolGate::~BoolGate() {
	;
}

void BoolGate::assignOutput(BoolWire* wire) {
	assert(wire);
	assert(!output);	// Otherwise we overwrite an existing output wire!
	this->output = wire;
	wire->input = this;
}

void BoolGate::assignInput(BoolWire* wire) {
	assert(wire);
	assert(assignedInputs < maxInputs);
	inputs[assignedInputs++] = wire;
	wire->output = this;
}

// If this gate's inputs have been visited, set it's output to visited, and return true
// Else, false
bool BoolGate::testAndSetReady() {
	for (int i = 0; i < assignedInputs; i++) {
		if (!inputs[i]->visited) {
			return false;
		}
	}
	output->visited = true;
	return true;
}

SparsePolyPtrVector* BoolGate::getOutput(bool a, bool b) {
	return this->op(a, b) ? &output[0].one : &output[0].zero;
}

/*
 *	gate checker function: 构造其对应的Quad Span Program S
 *  S = (t(x), I_labeled = {I_l0, I_l1, I_r0, I_r1, I_o0, I_o1},
 * 			 I_free = {})
 * 
 */ 
void BoolGate::genericSP(QSP* qsp) {
	/* 
	 * [GGPR13]中的Lemma 2，构造12个向量，每个向量都在9维空间
	 * 
	 * 前9个向量是线性无关的，pi(ri) = 1 and pi(rj) = 0 (j \neq i)
	 * 
	 * 利用Lagrange插值法构造多项式，根据多项式在不同点的取值
	 * 
	 * pi至少是9次多项式
	 */
	int rootIndex = qsp->genFreshRoots(9, qsp->vTargetRoots); 
	polys.reserve(9);
	SparsePolynomial* sparsePoly;

	// Create 8 independent vectors/polys, 4 for each input wire
	for (int i = 0; i < 8; i++) {
		sparsePoly = qsp->newSparsePoly();				
		polys.push_back(sparsePoly);
		FieldElt one;
		qsp->field->one(one);
		sparsePoly->addNonZeroRoot(rootIndex + i, one, qsp->field);

		if (i < 2) {
			inputs[0]->zero.push_back(sparsePoly); //I_l0 = {p0, p1}
		} else if (i < 4) {
			inputs[0]->one.push_back(sparsePoly); //I_l1 = {p2, p3}
		} else if (i < 6) {
			inputs[1]->zero.push_back(sparsePoly); //I_r0 = {p4, p5}
		} else {
			inputs[1]->one.push_back(sparsePoly); //I_r1 = {p6, p7}
		}
	}

	// Create the four output wires

	// Rest will be -1
	FieldElt zero, one, negOne;
	qsp->field->zero(zero);
	qsp->field->one(one);
	qsp->field->sub(zero, one, negOne);

	// Output wire 1
	sparsePoly = qsp->newSparsePoly();	
	polys.push_back(sparsePoly);
	getOutput(true, true)->push_back(sparsePoly);
	sparsePoly->addNonZeroRoot(rootIndex + 2, negOne, qsp->field);	
	sparsePoly->addNonZeroRoot(rootIndex + 6, negOne, qsp->field);

	// All output wires include the target's "special" 1 (target is (0, 0, ..., 0, 1))
	sparsePoly->addNonZeroRoot(rootIndex + 8, one, qsp->field); //t - p2 - p6

	// Output wire 2
	sparsePoly = qsp->newSparsePoly();
	polys.push_back(sparsePoly);
	getOutput(true, false)->push_back(sparsePoly);
	sparsePoly->addNonZeroRoot(rootIndex + 3, negOne, qsp->field);
	sparsePoly->addNonZeroRoot(rootIndex + 4, negOne, qsp->field);
	sparsePoly->addNonZeroRoot(rootIndex + 8, one, qsp->field);// t - p3 - p4
	
	// Output wire 3
	sparsePoly = qsp->newSparsePoly();
	polys.push_back(sparsePoly);
	getOutput(false, true)->push_back(sparsePoly);
	sparsePoly->addNonZeroRoot(rootIndex + 0, negOne, qsp->field);
	sparsePoly->addNonZeroRoot(rootIndex + 7, negOne, qsp->field);
	sparsePoly->addNonZeroRoot(rootIndex + 8, one, qsp->field); //t - p0 - p7
	
	// Output wire 4
	sparsePoly = qsp->newSparsePoly();
	polys.push_back(sparsePoly);
	getOutput(false, false)->push_back(sparsePoly);
	sparsePoly->addNonZeroRoot(rootIndex + 1, negOne, qsp->field);
	sparsePoly->addNonZeroRoot(rootIndex + 5, negOne, qsp->field);
	sparsePoly->addNonZeroRoot(rootIndex + 8, one, qsp->field); //[kunxian]: t - p1 - p5

	// Finally, extend the current v_0 with the _negative_ of the target vector for this SP (target is (0, 0, ..., 0, 1))
	sparsePoly = qsp->getSPtarget(); //[kunxian]: get v_0
	sparsePoly->addNonZeroRoot(rootIndex + 8, negOne, qsp->field);//t
	//[kunxian]: up to this step, the construction of SP for a bool gate is done
	//           which add two polys to each input wire's zero and one, and 
	//			 four polys to output wire.

	//[kunxian]: begin to construct witness that f(u) = v
	// Assign the appropriate polynomials to each wire
	assert(assignedInputs == 2);
	if (inputs[0]->value && inputs[1]->value) {
		inputs[0]->eval.push_back(polys[2]); //l1, r1
		inputs[1]->eval.push_back(polys[6]);
		output->eval.push_back(polys[8]);  //t - l1 - r1
	} else if (inputs[0]->value && !inputs[1]->value) {
		inputs[0]->eval.push_back(polys[3]); //l1', r0
		inputs[1]->eval.push_back(polys[4]);
		output->eval.push_back(polys[9]);  //t - l1' - r0
	} else if (!inputs[0]->value && inputs[1]->value) {
		inputs[0]->eval.push_back(polys[0]); //l0, r1'
		inputs[1]->eval.push_back(polys[7]);
		output->eval.push_back(polys[10]); //t - l0 - r1'
	} else if (!inputs[0]->value && !inputs[1]->value) {
		inputs[0]->eval.push_back(polys[1]); //l1, r0'
		inputs[1]->eval.push_back(polys[5]);
		output->eval.push_back(polys[11]); //t - l1 - r0'
	} else {
		assert(0);	// Shouldn't reach here
	}
}


