#pragma once
#include <algorithm>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <string>

void generate_init_all_func(
    std::multimap<std::string, std::string> dependences,
    std::map<std::string, std::string> init_funcs)
{
    std::map<std::string, bool> are_inited{};
    for (auto &[k, v] : init_funcs)
    {
        are_inited.emplace(k, false);
    }
    for (auto &[k, v] : dependences)
    {
        are_inited.emplace(k, false);
        are_inited.emplace(v, false);
    }
    while (std::ranges::any_of(
        are_inited, [](bool is_inited)
        { return false == is_inited; },
        [](auto &k_v)
        { return k_v.second; }))
    {
        for (auto &[k, v] : are_inited)
        {
            if (false == are_inited[k])
            {
                if (auto [b, e] = dependences.equal_range(k);
                    std::all_of(b, e, [&are_inited](auto &k_v)
                                { return are_inited[k_v.second]; }))
                {
                    if (init_funcs.contains(k))
                    {
                        std::cout << "//" << k << std::endl;
                        std::cout << init_funcs[k] << std::endl;
                    }
                    are_inited[k] = true;
                }
            }
        }
    }
}

void generate_needed_nodes_init_func(
    std::multimap<std::string, std::string> dependences,
    std::map<std::string, std::string> init_funcs,
    std::set<std::string> needed_nodes)
{
    std::map<std::string, bool> are_inited{};
    for (auto &[k, v] : init_funcs)
    {
        are_inited.emplace(k, false);
    }
    for (auto &[k, v] : dependences)
    {
        are_inited.emplace(k, false);
        are_inited.emplace(v, false);
    }
    while (std::ranges::any_of(needed_nodes, [&are_inited](auto &node)
                               { return false == are_inited[node]; }))
    {
        for (auto &node : needed_nodes)
        {
            if (false == are_inited[node])
            {
                auto [b, e] = dependences.equal_range(node);
                std::for_each(b, e, [&needed_nodes](auto &k_v) mutable
                              { needed_nodes.emplace(k_v.second); });
                if (auto [b, e] = dependences.equal_range(node);
                    std::all_of(b, e, [&are_inited](auto &k_v)
                                { return are_inited[k_v.second]; }))
                {
                    if (init_funcs.contains(node))
                    {
                        std::cout << init_funcs[node] << std::endl;
                    }
                    are_inited[node] = true;
                }
            }
        }
    }
}

template <bool all_node_needed = false, bool no_belong_to = false>
struct node_depend_and_belong_to_graph
{
    std::multimap<std::string, std::string> depend_relations;
    std::map<std::string, std::string> nodes;
    std::map<std::string, std::string> belong_to_relations;
    std::set<std::string> needed_nodes;
};
template <>
struct node_depend_and_belong_to_graph<true>
{
    std::multimap<std::string, std::string> depend_relations;
    std::map<std::string, std::string> nodes;
    std::map<std::string, std::string> belong_to_relations;
};
template <>
struct node_depend_and_belong_to_graph<true, true>
{
    std::multimap<std::string, std::string> depend_relations;
    std::map<std::string, std::string> nodes;
};

void generate_sorted_nodes(node_depend_and_belong_to_graph<true, true> graph)
{
    generate_init_all_func(graph.depend_relations, graph.nodes);
}

std::string get_ultimate_node(
    std::map<std::string, std::string> &ultimate_node_map,
    std::map<std::string, std::string> &belong_to_relations,
    const std::string node)
{
    if (!ultimate_node_map.contains(node))
    {
        if (belong_to_relations.contains(node))
        {
            auto &parent_node = belong_to_relations[node];
            ultimate_node_map.emplace(
                node,
                get_ultimate_node(
                    ultimate_node_map, belong_to_relations, parent_node));
        }
        else
        {
            ultimate_node_map.emplace(
                node,
                node);
        }
    }
    return ultimate_node_map[node];
}

void generate_sorted_nodes(node_depend_and_belong_to_graph<true> graph)
{
    std::map<std::string, node_depend_and_belong_to_graph<true>> ultimate_node_graphs{};
    std::map<std::string, std::string> ultimate_node_map{};
    for (auto [node, init] : graph.nodes)
    {
        auto ultimate_node = get_ultimate_node(ultimate_node_map, graph.belong_to_relations, node);
        if (!ultimate_node_graphs.contains(ultimate_node))
        {
            ultimate_node_graphs.emplace(ultimate_node, node_depend_and_belong_to_graph<true>{});
        }
        auto &ultimate_node_graph = ultimate_node_graphs[ultimate_node];
        ultimate_node_graph.nodes.emplace(node, init);
        if (graph.belong_to_relations.contains(node)
            && graph.belong_to_relations[node] != ultimate_node)
        {
            ultimate_node_graph
                .belong_to_relations
                .emplace(
                    node,
                    graph.belong_to_relations[node]);
        }
        for (auto [ite, end] = graph.depend_relations.equal_range(node); ite != end; ite++)
        {
            auto &[n, depend_node] = *ite;
            ultimate_node_graph.depend_relations.emplace(node, depend_node);
        }
    }

    for (auto &[ultimate_node, ultimate_node_graph] : ultimate_node_graphs)
    {
        if (!ultimate_node_graph.belong_to_relations.empty())
        {
            generate_sorted_nodes(ultimate_node_graph);
        }
        else
        {
            node_depend_and_belong_to_graph<true, true> graph{};
            graph.depend_relations = ultimate_node_graph.depend_relations;
            graph.nodes = ultimate_node_graph.nodes;
            generate_sorted_nodes(std::move(graph));
        }
    }
}

void generate_sorted_nodes(node_depend_and_belong_to_graph<> graph)
{
    node_depend_and_belong_to_graph<true> refined_graph{};
    std::set<std::string> needed_nodes{};
    for (std::set<std::string> next_needed_nodes{std::move(graph.needed_nodes)};
         false == next_needed_nodes.empty();)
    {
        std::set<std::string> next_next_needed_nodes{};
        std::ranges::for_each(next_needed_nodes,
                              [&graph, &refined_graph, &next_next_needed_nodes](auto &node)
                              {
                                  auto [begin, end] = graph.depend_relations.equal_range(node);
                                  for (auto ite = begin; ite != end; ite++)
                                  {
                                      auto &[node, depend_node] = *ite;
                                      refined_graph.depend_relations.emplace(node, depend_node);
                                      next_next_needed_nodes.emplace(depend_node);
                                  }
                                  if (graph.belong_to_relations.contains(node))
                                  {
                                      refined_graph.belong_to_relations.emplace(
                                          node,
                                          graph.belong_to_relations[node]);
                                      next_next_needed_nodes.emplace(graph.belong_to_relations[node]);
                                  }
                                  refined_graph.nodes.emplace(node, graph.nodes[node]);
                              });
        next_needed_nodes = std::move(next_next_needed_nodes);
    }
    generate_sorted_nodes(refined_graph);
}
