## How are the QSP codes organized

- `BoolCircuit.cpp`: base class for a boolean circuit. Any specific bool circuit will extend this class. To build a boolean circuit, you need to assign input wires, output wires, intermediate wires to gates. Then **sort the gates topologically** so that evaluate the bool circuit is quite straightforwrd.
 

- `BoolWire.h`: a wire contains the following info
    - the value it carries
    - the gates it connects as an input
    - the gate it connects as an output
    - the unique id which it's assigned in the circuit
    - the polynomials that belong to zero set and one set of wire

- `BoolGate.h`: This is a base class for all boolean gates. The  AND, OR gates are subclasses of this class. A gate contains the following info
    - the op it defines
    - the input wires
    - the output wire
    - the SP program which can compute the gate

- BoolGate examples: 
    - GateBoolOR.h
    - GateBoolAND.h
    - GateBoolXOR.h
    - GateBoolNAND.h
    
- BoolCircuit examples are 
    - CircuitBinaryAdder.cpp: 

- QSP for BoolCircuit: 

## How are the QAP codes organized?

## pairing
pairing e: G1 * G2 -> Gt is a bilinear map.

In a typical instantiation of pairing e,
- cyclic group G1 is E(Fp)[n] where n = p + 1-t;

- cyclic group G2 is inverse image of E'(Fp2)[n] under a twisting isomorphism phi from E' to E;

- the target group Gt is a multiplicative subgroup of field Fp12;

- Gt = { x in Fp12 : x^r = 1 for some integer r };

- the field Fp12 is constructed via the following tower
    - Fp2 = Fp[u] / (u^2 + 1);
    - Fp6 = Fp2[v] / (v^3 - u') where u' = u+1;
    - Fp12 = Fp6[w] / (w^2 - v).

Therefore, in this typical setting, the operation in these groups are `scalar multiplication`, `scalar multiplication`, and `exponentation`. And the finite fields we need to handle are prime field and its finite extension.

## GGPR proof system

## PHGR proof system