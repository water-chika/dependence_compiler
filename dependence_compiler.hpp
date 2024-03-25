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
