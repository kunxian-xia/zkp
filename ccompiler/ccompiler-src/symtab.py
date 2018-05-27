import os
import sys
import subprocess
import argparse

# mypath = os.path.dirname(__file__)
# sys.path.append("../external-code/ply-3.4")
# sys.path.append("../external-code/pycparser-2.08")

import pycparser
from pycparser import c_ast

class symtab:
    def __init__(self, filename, args):
        self.cpp_arg = args.cpp_arg
        tmpname = self.gcc_preprocess(filename, self.cpp_arg)
        file = open(tmpname)
        lines = file.read().split("\n")
        lines = self.preprocess(lines)
        cfile = "\n".join(lines)
        print(cfile)
        #		ofp = open("/tmp/foo", "w")
        #		ofp.write(cfile)
        #		ofp.close()
        self.verbose = True
        parser = pycparser.CParser()
        ast = parser.parse(cfile)
        self.root_ast = ast

        self.output = self.create_global_symtab()

    def gcc_preprocess(self, filename, cpp_arg):
        # returns filename of preprocessed file
        dir = os.path.dirname(filename)
        print(dir)
        if (dir == ""):
            dir = "."
        tmpname = dir + "\\build_" + os.path.basename(filename) + ".p"

        def under_to_dash(s):
            if (s[0] == "_"):
                s = "-" + s[1:]
            return s

        if (cpp_arg != None):
            cpp_args = map(under_to_dash, cpp_arg)
        else:
            cpp_args = []
        # I used to call gcc-4; I'm not sure what machine had two versions
        # or what the issue was, but I leave this comment here as a possible
        # solution to the next sucker who runs into that problem.
        cmd_list = ["gcc"] + cpp_args + ["-E", filename, "-o", tmpname]
        print("cmd_list %s" % (" ".join(cmd_list)))
        sp = subprocess.Popen(cmd_list) #run gcc to preprocess the c file
        sp.wait()
        return tmpname

    def preprocess_line(self, line):
        if (line == '' or line[0] == '#'):
            return ''
        return line.split("//")[0]

    def preprocess(self, lines):
        return map(self.preprocess_line, lines)

    def create_global_symtab(self):
		#symtab = Symtab()

        for (iname,obj) in self.root_ast.children():
            if (isinstance(obj, c_ast.Decl)):
#				print
#				print obj.__dict__
#				print ast_show(obj)
                if (isinstance(obj.type, c_ast.FuncDecl)):
                    if (self.verbose):
                        print("Ignoring funcdecl %s for now" % (obj.name))
                    pass
                else:
                    if (self.verbose):
                        print
						#print(ast_show(obj))
                    #symtab = self.declare_variable(obj, symtab)
            elif (isinstance(obj, c_ast.FuncDef)):
                if (self.verbose):
                    print("Ignoring funcdef %s for now" % (obj.decl.name))
                pass
            else:
                print(obj.__class__)
                assert(False)
		#symtab.declare(PseudoSymbol("_unroll"), self.dfg(Undefined))
		#return symtab
        return

def main(argv):
    parser = argparse.ArgumentParser(description='Compile C to QSP/QAP')
    parser.add_argument('cfile', metavar='<cfile>',help='a C file to compile')
    parser.add_argument('--cpparg', dest='cpp_arg', nargs="*", help='extra arguments to C preprocessor')
    args = parser.parse_args(argv)
    s = symtab(args.cfile, args)
	#timing = Timing(args.cfile, enabled=False)

if __name__ == "__main__":
    main(sys.argv[1:])

