#pragma once
#include <algorithm>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <vector>
#include <cassert>

template <bool all_node_needed = false, bool no_belong_to = false>
struct node_depend_and_belong_to_graph
{
    std::set<std::string> nodes;
    std::multimap<std::string, std::string> depend_relations;
    std::map<std::string, std::string> belong_to_relations;
    std::set<std::string> needed_nodes;
};
template <>
struct node_depend_and_belong_to_graph<true>
{
    std::set<std::string> nodes;
    std::multimap<std::string, std::string> depend_relations;
    std::map<std::string, std::string> belong_to_relations;
};
template <>
struct node_depend_and_belong_to_graph<true, true>
{
    std::set<std::string> nodes;
    std::multimap<std::string, std::string> depend_relations;
};

bool is_nodes_sorted(node_depend_and_belong_to_graph<true, true> graph, std::vector<std::string> nodes)
{
    bool result = true;
    std::set<std::string> previous_nodes{};
    for (auto &node : nodes)
    {
        result = graph.nodes.contains(node);
        if (result == false)
            break;
        for (auto [ite, end] = graph.depend_relations.equal_range(node);
             ite != end;
             ite++)
        {
            auto [n, depend_node] = *ite;
            result = previous_nodes.contains(depend_node);
            if (result == false)
                break;
        }
        previous_nodes.emplace(node);
    }
    return result;
}

std::vector<std::string> generate_sorted_nodes(node_depend_and_belong_to_graph<true, true> graph)
{
    std::multimap<std::string, std::string> dependences = graph.depend_relations;
    std::vector<std::string> sorted_nodes{};
    std::map<std::string, bool> are_inited{};
    for (auto &node : graph.nodes)
    {
        are_inited.emplace(node, false);
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
                    sorted_nodes.emplace_back(k);
                    are_inited[k] = true;
                }
            }
        }
    }

    assert(is_nodes_sorted(graph, sorted_nodes));

    return sorted_nodes;
}

std::string get_ultimate_node(
    std::map<std::string, std::string> &ultimate_node_map,
    std::map<std::string, std::string> &belong_to_relations,
    std::string node)
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

bool is_nodes_sorted(node_depend_and_belong_to_graph<true> graph, std::vector<std::string> nodes) {
    return false;
}

std::vector<std::string> generate_sorted_nodes(node_depend_and_belong_to_graph<true> graph)
{
    std::vector<std::string> sorted_nodes{};

    std::map<std::string, node_depend_and_belong_to_graph<true>> ultimate_node_graphs{};
    node_depend_and_belong_to_graph<true, true> ultimate_node_graph_graph{};
    std::map<std::string, std::string> ultimate_node_map{};

    for (auto node : graph.nodes)
    {
        auto ultimate_node = get_ultimate_node(ultimate_node_map, graph.belong_to_relations, node);
        if (!ultimate_node_graphs.contains(ultimate_node))
        {
            ultimate_node_graphs.emplace(ultimate_node, node_depend_and_belong_to_graph<true>{});
            ultimate_node_graph_graph.nodes.emplace(ultimate_node);
        }
        if (ultimate_node != node)
        {
            auto &ultimate_node_graph = ultimate_node_graphs[ultimate_node];
            ultimate_node_graph.nodes.emplace(node);
            if (graph.belong_to_relations.contains(node) && graph.belong_to_relations[node] != ultimate_node)
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
                auto depend_node_ultimate_node = get_ultimate_node(ultimate_node_map, graph.belong_to_relations, depend_node);
                if (ultimate_node == depend_node_ultimate_node)
                {
                    ultimate_node_graph.depend_relations.emplace(node, depend_node);
                }
                else
                {
                    ultimate_node_graph_graph.depend_relations.emplace(ultimate_node, depend_node_ultimate_node);
                }
            }
        }
    }

    auto sorted_ultimate_nodes = generate_sorted_nodes(ultimate_node_graph_graph);

    for (auto &ultimate_node : sorted_ultimate_nodes)
    {
        sorted_nodes.emplace_back(ultimate_node);
        auto &ultimate_node_graph = ultimate_node_graphs[ultimate_node];
        std::vector<std::string> graph_sorted_nodes{};
        if (!ultimate_node_graph.belong_to_relations.empty())
        {
            graph_sorted_nodes = generate_sorted_nodes(ultimate_node_graph);
        }
        else
        {
            node_depend_and_belong_to_graph<true, true> graph{};
            graph.nodes = ultimate_node_graph.nodes;
            graph.depend_relations = ultimate_node_graph.depend_relations;
            graph_sorted_nodes = generate_sorted_nodes(std::move(graph));
        }
        for (auto &node : graph_sorted_nodes)
        {
            sorted_nodes.emplace_back(node);
        }
    }

    return sorted_nodes;
}

bool is_nodes_sorted(node_depend_and_belong_to_graph<> graph, std::vector<std::string> nodes)
{
    bool result = true;
    return result;
}

std::vector<std::string> generate_sorted_nodes(
    node_depend_and_belong_to_graph<> graph)
{
    node_depend_and_belong_to_graph<true> refined_graph{};
    for (std::set<std::string> next_needed_nodes{graph.needed_nodes};
         false == next_needed_nodes.empty();)
    {
        std::set<std::string> next_next_needed_nodes{};
        std::ranges::for_each(next_needed_nodes,
                              [&graph, &refined_graph, &next_next_needed_nodes](auto &node)
                              {
                                  refined_graph.nodes.emplace(node);
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
                              });
        next_needed_nodes = next_next_needed_nodes;
    }
    std::vector<std::string> sorted_nodes = generate_sorted_nodes(refined_graph);
    assert(is_nodes_sorted(graph, sorted_nodes));
    return sorted_nodes;
}
