#include <string>

void add_depend(std::string a, std::string b);
void add_init(std::string a, std::string b);
void add_needed_node(std::string a);
void add_belong_to(std::string a, std::string b);
void yyerror(const char* str);
#define YY_NO_UNISTD_H 1
