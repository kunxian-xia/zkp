

all: libqsp.a

libqsp.a: Field.o SparsePolynomial.o BoolGate.o BoolCircuit.o  \
		 GateBoolAND.o GateBoolXOR.o GateBoolNAND.o GateBoolOR.o \
		 CircuitBinaryAdder.o QSP.o 
	ar rcs libqsp.a Field.o SparsePolynomial.o BoolGate.o BoolCircuit.o \
		 GateBoolAND.o GateBoolXOR.o GateBoolNAND.o GateBoolOR.o \
		 CircuitBinaryAdder.o QSP.o

%.o: %.cpp 
	g++ -std=c++11 -g -c $<