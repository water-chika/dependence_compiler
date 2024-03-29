all : dependence_compiler


lex.yy.c : lexer.l
	flex --noyywrap --header=lexer.h lexer.l

parser.tab.c : parser.y
	bison parser.y

dependence_compiler : main.cpp lex.yy.c parser.tab.c dependence_compiler.hpp
	c++ main.cpp lex.yy.c parser.tab.c --std=c++23 -g -o dependence_compiler
