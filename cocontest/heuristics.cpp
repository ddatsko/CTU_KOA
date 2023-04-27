#include "heuristics.h"
#include <vector>
#include <stack>
#include "Problem.h"
#include <algorithm>
#include <random>
#include <tuple>
#include <iostream>

using solution_t = std::vector<node_idx_t>;

namespace {
    // Using the same buffer all the time
    // TODO: remove this. Maybe, move heuristics to some class
    std::vector<bool> visited;
    std::vector<node_idx_t> cur_permutation;
    std::vector<node_idx_t> cycle_marks;

    // Set edge from node from index "from" to node to index "to" in the solution
    void set_solution_edge(solution_t &solution, const Problem &p, node_idx_t from, node_idx_t to) {
        for (node_idx_t i = 0; i < p.adj_l[from].size(); ++i) {
            if (p.adj_l[from][i].first == to) {
                solution[from] = i;
            }
        }
    }

    template<typename T>
    T randint(T min, T max) {
        // TODO: consider using std::uniform_distribution here as a better source of randomness
        return min + std::rand() % max;
    }

    // Set the length of visited vector to n and each value to false
    void zero_visited(size_t n) {
        visited.resize(n);
        std::fill(visited.begin(), visited.end(), false);
    }

    // Reshuffle the cur_permutation variable to change the order of nodes traversal
    void reshuffle_nodes(size_t n) {
        if (cur_permutation.size() != n) {
            cur_permutation = std::vector<node_idx_t>(n);
            for (node_idx_t i = 0; i < n; ++i) {
                cur_permutation[i] = i;
            }
        }
        static std::random_device dev;
        static std::mt19937 rng(dev());
        std::shuffle(cur_permutation.begin(), cur_permutation.end(), rng);
    }


    /*!
     * Assign a number to each node in the following order:
     *   - 0 if the node does not belong to any cycle in the solution
     *   - N, where N is an integer representing a cycle number. Two nodes in the same cycle will have it the same
     * @param p Problem
     * @param solution solution, marking for which should be produced
     */
    void mark_cycles(const Problem &p, const solution_t &solution) {
        cycle_marks = std::vector<node_idx_t>(p.n);
        node_idx_t cur_cycle_idx = 1;
        zero_visited(p.n);

        for (node_idx_t i = 0; i < p.n; ++i) {
            if (visited[i]) {
                continue;
            }
            ++cur_cycle_idx;

            // Try to find the cycle in the solution starting from node i
            bool not_full_cycle_found = false;
            bool full_cycle_found = false;
            node_idx_t cycle_start;
            node_idx_t v = i;
            for (int j = 0; j < p.n; ++j) {
                if (visited[v]) {
                    break;
                }
                visited[v] = true;
                cycle_marks[v] = cur_cycle_idx;
                auto next = p.adj_l[v][solution[v]].first;

                // If the next node is the one from current search -- make a cycle but "cut" tail in the beginning
                if (cycle_marks[next] == cur_cycle_idx && next != i) {
                    not_full_cycle_found = true;
                    cycle_start = next;
                    break;
                }
                // If the cycle closes at node i, cycle found. Perfect scenario
                if (next == i) {
                    full_cycle_found = true;
                    break;
                }
                v = next;
            }
            // If only a partial cycle was found -- "Cut" the tail at the beginning while preserving the found cycle
            if (not_full_cycle_found) {
                v = i;
                for (node_idx_t j = 0; j < p.n; ++j) {
                    cycle_marks[v] = 0;
                    auto next = p.adj_l[v][solution[v]].first;
                    if (next == cycle_start) {
                        break;
                    }
                    v = next;
                }
                // If no cycle was found, just replace all the marks back to 0
            } else if (!full_cycle_found) {
                v = i;
                for (node_idx_t j = 0; j < p.n; ++j) {
                    cycle_marks[v] = 0;
                    auto next = p.adj_l[v][solution[v]].first;
                    if (cycle_marks[next] != cur_cycle_idx) {
                        break;
                    }
                    v = next;
                }
            }
        }
    }
}

std::vector<std::vector<node_idx_t>> find_cycles(const Problem &p, const solution_t &solution) {
    mark_cycles(p, solution);
    zero_visited(p.n);
    std::vector<std::vector<node_idx_t>> res;
    for (node_idx_t i: cur_permutation) {
        if (cycle_marks[i] != 0 && !visited[cycle_marks[i]]) {
            visited[cycle_marks[i]] = true;
            res.emplace_back();
            size_t current_cycle_idx = res.size() - 1;
            res[current_cycle_idx].emplace_back(i);
            node_idx_t cur_node = p.adj_l[i][solution[i]].first;
            while (cur_node != i) {
                res[current_cycle_idx].emplace_back(cur_node);
                cur_node = p.adj_l[cur_node][solution[cur_node]].first;
            }

        }
    }
    return res;
}

namespace {

    // Reshuffle nodes order for first n nodes
    std::vector<node_idx_t>  random_nodes_order(size_t n) {
        std::vector<node_idx_t> nodes_order(n);
        for (node_idx_t i = 0; i < n; ++i) {
            nodes_order[i] = i;
        }
        static std::random_device dev;
        static std::mt19937 rng(dev());
        std::shuffle(nodes_order.begin(), nodes_order.begin() + n, rng);
        return nodes_order;
    }


    // DFS iteration for disjoint cycle creation
    bool
    create_disjoint_cycle_dfs(const Problem &p, solution_t &solution, node_idx_t v, node_idx_t depth, node_idx_t init, bool random_order=false) {
        if (visited[v] || depth > p.L) {
            return false;
        }
        visited[v] = true;
        std::vector<node_idx_t> nodes_order;
        if (random_order) {
            nodes_order = random_nodes_order(p.adj_l[v].size());
        }
        for (node_idx_t i = 0; i < p.adj_l[v].size(); ++i) {
            std::pair<node_idx_t, weight_t> to;

            if (random_order) {
                to = p.adj_l[v][nodes_order[i]];
            } else {
                to = p.adj_l[v][i];
            }

            if (to.first == init) {
                solution[v] = i;
                return true;
            }
            if (visited[to.first]) {
                continue;
            }
            auto next_res = create_disjoint_cycle_dfs(p, solution, to.first, depth + 1, init, random_order);
            if (next_res) {
                solution[v] = i;
                return true;
            }
        }
        return false;
    }
}

bool create_random_cycle(const Problem &p, solution_t &cur_solution, bool random_order) {
    reshuffle_nodes(p.n);
    mark_cycles(p, cur_solution);
    for (size_t i = 0; i < p.n; ++i) {
        if (cycle_marks[i] != 0) {
            visited[i] = true;
        } else {
            visited[i] = false;
        }
    }
    auto visited_init = visited;

    for (node_idx_t i = 0; i < p.n; ++i) {
        auto v = cur_permutation[i];
        if (visited[v]) {
            continue;
        }
        if (create_disjoint_cycle_dfs(p, cur_solution, v, 1, v, random_order)) {
            return true;
        }
        // Set visited to the initial value
        // TODO: this copying can be avoided by using the same visited vector, but marking visited nodes differently
        visited = visited_init;
    }
    return false;
}

bool shorten_long_cycles(const Problem &p, solution_t &cur_solution) {
    const auto cycles = find_cycles(p, cur_solution);
    bool shortened = false;
    for (const auto &cycle: cycles) {
        // SHorten only cycles that are too long
//        if (cycle.size() <= p.L) {
//            continue;
//        }
        auto best_break_score = std::numeric_limits<weight_t>::lowest();
        std::pair<size_t, size_t> best_break_idx;
        auto s = cycle.size();
        for (size_t i = 0; i < cycle.size(); ++i) {
            for (size_t j = i + 2; j < s; ++j) {
                auto &c_i = cycle[i];
                auto &c_j = cycle[j];

                // Try to break the cycle and glue it back into two cycles
                auto i_next = cycle[(i + 1) % s];
                auto j_next = cycle[(j + 1) % s];
                if (p.adj_m[c_j][i_next] && p.adj_m[c_i][j_next]) {
                    auto score = p.w[c_j][i_next] + p.w[c_i][j_next] - p.w[c_i][i_next] - p.w[c_j][j_next];
                    if (score > best_break_score) {
                        best_break_score = score;
                        best_break_idx = {i, j};
                    }
                }
            }
        }
        if (best_break_score != std::numeric_limits<weight_t>::lowest()) {
            set_solution_edge(cur_solution, p, cycle[best_break_idx.first], cycle[(best_break_idx.second + 1) % s]);
            set_solution_edge(cur_solution, p, cycle[best_break_idx.second], cycle[(best_break_idx.first + 1) % s]);
            shortened = true;
        }
    }
    return shortened;
}

bool add_to_cycles(const Problem &p, solution_t &cur_solution, bool constrain_cycle_length) {
    // Get the list of nodes not belonging to any cycle in a random order
    zero_visited(p.n);
    reshuffle_nodes(p.n);
    const auto cycles = find_cycles(p, cur_solution);
    std::vector<node_idx_t> not_in_cycles;
    for (node_idx_t i = 0; i < p.n; ++i) {
        auto idx = cur_permutation[i];
        if (cycle_marks[idx] == 0) {
            not_in_cycles.emplace_back(idx);
        }
    }
    bool inserted = false;

    // Loop through each cycle and try to find the best node that can be inserted into that loop
    for (const auto &cycle: cycles) {
        const auto &s = cycle.size();
        // D not insert anything into a long loop
        if (s >= p.L && constrain_cycle_length) {
            continue;
        }
        weight_t best_insertion_score = std::numeric_limits<weight_t>::lowest();
        std::tuple<node_idx_t, node_idx_t, node_idx_t> best_insertion;
        for (size_t i = 0; i < cycle.size(); ++i) {
            const auto &c_i = cycle[i];
            const auto &c_next = cycle[(i + 1) % s];

            for (const auto &n_i: not_in_cycles) {
                if (visited[n_i]) continue;
                if (p.adj_m[c_i][n_i] && p.adj_m[n_i][c_next]) {
                    auto score = p.w[c_i][n_i] + p.w[n_i][c_next] - p.w[c_i][c_next];
                    if (score > best_insertion_score) {
                        best_insertion_score = score;
                        best_insertion = {c_i, n_i, c_next};
                    }
                }
            }
        }
        if (best_insertion_score != std::numeric_limits<weight_t>::lowest()) {
            visited[std::get<1>(best_insertion)] = true;
            set_solution_edge(cur_solution, p, std::get<0>(best_insertion), std::get<1>(best_insertion));
            set_solution_edge(cur_solution, p, std::get<1>(best_insertion), std::get<2>(best_insertion));
            inserted = true;
        }
    }
    return inserted;
}

bool break_random_cycle(const Problem &p, solution_t &cur_solution) {
    reshuffle_nodes(p.n);
    auto cycles = find_cycles(p, cur_solution);
    if (cycles.empty()) {
        return false;
    }
    for (const auto &c_i: cycles[0]) {
        auto random_next_node = randint(static_cast<node_idx_t>(0), static_cast<node_idx_t>(p.adj_l[c_i].size()));
        cur_solution[c_i] = random_next_node;
    }
    return true;
}
