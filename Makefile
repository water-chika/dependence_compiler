all : dependence_compiler


lex.yy.c : lexer.l
	flex --noyywrap --header=lexer.h lexer.l

parser.tab.c : parser.y
	bison parser.y

dependence_compiler : attribute_dependence_parser.cpp lex.yy.c parser.tab.c
	c++ attribute_dependence_parser.cpp lex.yy.c parser.tab.c --std=c++23 -o dependence_compiler
