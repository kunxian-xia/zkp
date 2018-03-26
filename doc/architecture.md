## How are the QSP/QAP codes organized

- `BoolCircuit.cpp`: base class for a boolean circuit. Any specific bool circuit will extend this class. To build a boolean circuit, you need to assign input wires, output wires, intermediate wires to gates. Then **sort the gates topologically** so that evaluate the bool circuit is quite straightforwrd.
 

- `BoolWire.h`: a wire contains the following info
    - the value it carries
    - the gates it connects as an input
    - the gate it connects as an output
    - the unique id which it's assigned in the circuit
    - 

- `BoolGate.h`: This is a base class for all boolean gates. The  AND, OR gates are subclasses of this class. A gate contains the following info
    - the op it defines
    - the input wires
    - the output wire
    - 

- BoolGate examples: 
    - GateBoolOR.h
    - GateBoolAND.h
    - GateBoolXOR.h
    - GateBoolNAND.h
    
- BoolCircuit examples are 
    - CircuitBinaryAdder.cpp: 

- QSP for BoolCircuit: 