#include "dependence_compiler.hpp"

std::multimap<std::string, std::string> dependences{};
std::map<std::string, std::string> init_funcs{};
std::set<std::string> needed_nodes{};
std::map<std::string, std::string> belong_to_relations{};
void add_depend(std::string a, std::string b)
{
    dependences.emplace(std::move(a), std::move(b));
}
void add_init(std::string a, std::string b)
{
    init_funcs.emplace(std::move(a), std::move(b));
}
void add_needed_node(std::string a) { needed_nodes.emplace(std::move(a)); }
void add_belong_to(std::string a, std::string b) {
    belong_to_relations.emplace(a, b);
}
void yyparse();

int main()
{
    yyparse();
    node_depend_and_belong_to_graph<> graph{};
    graph.nodes = init_funcs;
    graph.needed_nodes = needed_nodes;
    graph.belong_to_relations = belong_to_relations;
    graph.depend_relations = dependences;
    generate_sorted_nodes(graph);
    return 0;
}
