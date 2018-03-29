#pragma once

#include <list>
#include <vector>

//#include "Types.h"
//#include "BoolGate.h"
//#include "BoolPoly.h"
#include "SparsePolynomial.h"

class BoolWire;
class BoolGate;

typedef std::list<BoolWire *> BoolWireList;
typedef std::vector<SparsePolynomial*> SparsePolyPtrVector;

// a wire can be input to one gate, and output to another gate
class BoolWire {
public:
	BoolWire(BoolGate* in = NULL, BoolGate* out = NULL) : input(in), output(out) { value = false; visited = false; }
	
	BoolGate* input;	// NULL if this is a circuit input
	BoolGate* output;	// NULL if this is a circuit output
	bool value;
	bool visited;			// Used for DFS of circuit graph
	long long id; 
	//[kunxian]: every wire must have a unique id, and a wire can be inputs to many gates.

	SparsePolyPtrVector zero;	// All of the polys that represent a zero on this wire
	SparsePolyPtrVector one;	// All of the polys that represent a one on this wire
	SparsePolyPtrVector eval;	// Polys representing the value this wire takes on in a given evaluation
};

