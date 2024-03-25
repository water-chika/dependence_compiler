#include "dependence_compiler.hpp"

std::multimap<std::string, std::string> dependences{};
std::map<std::string, std::string> init_funcs{};
std::set<std::string> needed_nodes{};
void add_depend(std::string a, std::string b)
{
    dependences.emplace(std::move(a), std::move(b));
}
void add_init(std::string a, std::string b)
{
    init_funcs.emplace(std::move(a), std::move(b));
}
void add_needed_node(std::string a) { needed_nodes.emplace(std::move(a)); }
void yyparse();

int main()
{
    yyparse();
    if (needed_nodes.empty())
    {
        generate_init_all_func(dependences, init_funcs);
    }
    else
    {
        generate_needed_nodes_init_func(dependences, init_funcs, needed_nodes);
    }
    return 0;
}
