%{
#include <stdio.h>
#include "parser.h"
#include "lexer.h"
#include <string>
#include <iostream>
%}

%header
%token ID LEFT_ARROW INIT_STATEMENTS END_OF_LINE LEFT_BRACE RIGHT_BRACE IS NEEDED BELONG TO
%define api.value.type { std::string }

%%

all:
  depend all
| init all
| need_statement all
| belong_to_statement all
| %empty
;

depend: ID LEFT_ARROW ID END_OF_LINE { 
      add_depend(std::move($1), std::move($3));
}
;

init: ID init_statements END_OF_LINE {
    add_init(std::move($1), std::move($2));
}
;

init_statements:
	       %empty	{ $$ = ""; }
|	LEFT_BRACE init_statements RIGHT_BRACE init_statements {
		$$ = "{" + std::move($2) + "}" + std::move($4);
}
|	INIT_STATEMENTS init_statements {
		$$ = std::move($1) + std::move($2);
}

need_statement: ID IS NEEDED END_OF_LINE {
    add_needed_node($1);
}

belong_to_statement: ID BELONG TO ID END_OF_LINE {
    add_belong_to($1, $4);
}

%%

void yyerror(const char* err) {
    fprintf(stderr, "err: %s\n", err);
}
