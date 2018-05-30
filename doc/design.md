## 

The compiler parses a C file and then translates it to a equivalent arithmetic or bool circuit. 
That is, the target machine is not a RAM but a circuit. 

The front end will output a IL representation such as 
> (== (* (input->a) 2) (+ (output->b) 5)) 

The back end then translates this IL into equivalent arithmetic curcuit using add, mul, split gates. 

### Stage 1. Parsing

Given a single C file, parse it and get AST(abstract syntax tree). From the ast, we can get 

- type declaration, such as struct decl;

- global variable declaration, which we will keep track in the global symbol table(symtab);

- determine the memory model of each symbol.

#### a. Build AST
We can use the `pycparser` library which is written in Python. To parse a C file, we just need to new a CParser object and invoke its `parse` method. That is, 

```python
import pycparser

cfile = open(filename)
lines = cfile.read()

parser = pycparser.CParser()
root_ast = parser.parse(lines)

```