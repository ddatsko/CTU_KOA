#include "Problem.h"
#include <vector>
#include "common_types.h"
#include "tabu_search.h"
#include <stack>
#include "heuristics.h"
#include <list>
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>

using solution_t = std::vector<node_idx_t>;

namespace {
    const int P_CYCLE = 25;
    int P_BREAK = 60;
    const int P_SHORTEN = 7;
    const int RANDOM_CYCLE_ORDER_PROB = 5;
    int INITIAL_SOLUTIONS = 20;
    const int P_ADD = 50;
    const size_t MAX_TABU_LIST_SIZE = 1000;
    const int TIME_MEASUREMENT_ITERATIONS = 1;
    int ITERATIONS_PER_NEIGHBOURHOOD_SEARCH = 15;


    // Using the same buffer all the time
    std::vector<bool> visited;

    void zero_visited(size_t n) {
        visited.resize(n);
        std::fill(visited.begin(), visited.end(), false);
    }

    int get_random_prob() {
        static std::random_device dev;
        static std::mt19937 rng(dev());
        static std::uniform_int_distribution<int> dist(0, 100);
        return dist(rng);
    }

    /*!
    * Hash function for a vector as array of integers. Not a well checked one. May produce too many collisions
    * @param vec Vector to be hashed
    * @return hash for the vector
    */
    size_t hash_vector(std::vector<node_idx_t> const &vec) {
        std::size_t seed = vec.size();
        for (auto x_original: vec) {
            uint32_t x = static_cast<uint32_t>(x_original);
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

}

weight_t get_solution_cost(const Problem &p, const solution_t &solution) {
    weight_t cost = 0;
    auto cycles = find_cycles(p, solution);
    for (const auto &c: cycles) {
        if (c.size() > p.L) {
            continue;
        }
        for (size_t i = 0; i < c.size(); ++i) {
            cost += p.w[c[i]][c[(i + 1) % c.size()]];
        }
    }

    return cost;
}


solution_t solve_tabu_search(const Problem &p, solution_t solution, long long max_time_us) {
    auto start_time = std::chrono::high_resolution_clock::now();

    long long threshold_time = std::max(100000ll, max_time_us / 200000 * p.n);
    max_time_us = max_time_us - threshold_time;
    // If the instance is very large, decrease some parameters to make more iterations with less random
    if (p.n > 5000) {
        ITERATIONS_PER_NEIGHBOURHOOD_SEARCH /= 5;
        INITIAL_SOLUTIONS = 3;
        P_BREAK = 20;
        max_time_us -= 1000000;
    }

    solution_t best_solution = solution;
    auto best_solution_cost = get_solution_cost(p, best_solution);

    for (int i = 0; i < INITIAL_SOLUTIONS; ++i) {
        auto init_solution = solution;
        while (create_random_cycle(p, init_solution, false)) {}
        while (add_to_cycles(p, init_solution)) {};
        auto cost = get_solution_cost(p, init_solution);
        if (cost > best_solution_cost) {
            best_solution_cost = cost;
            best_solution = init_solution;
        }
    }

    weight_t best_neighbourhood_cost = std::numeric_limits<weight_t>::lowest();
    solution_t best_neighbourhood_solution = best_solution;

    std::list<uint32_t> tabu_list;
    size_t iteration = 0;

    while (true) {
        if (iteration % TIME_MEASUREMENT_ITERATIONS == 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now() - start_time);
            if (std::chrono::duration<long long, std::micro>(elapsed).count() >= max_time_us) {
                break;
            }
        }
        ++iteration;
        for (size_t i = 0; i < ITERATIONS_PER_NEIGHBOURHOOD_SEARCH; ++i) {
            auto tabu_solution = best_neighbourhood_solution;
            auto prob = get_random_prob();

            // Break cycles many times
            break_random_cycle(p, tabu_solution);
            while (prob < P_BREAK && break_random_cycle(p, tabu_solution)) {
                prob = get_random_prob();
            }

            bool heuristic_res = true;
            while (heuristic_res) {
                prob = get_random_prob();
                if (prob < P_CYCLE) {
                    heuristic_res = create_random_cycle(p, tabu_solution, get_random_prob() < RANDOM_CYCLE_ORDER_PROB);
                } else if (prob < P_CYCLE + P_SHORTEN) {
                    heuristic_res = shorten_long_cycles(p, tabu_solution);
                } else {
                    heuristic_res = add_to_cycles(p, tabu_solution);
                }
            }


            auto tabu_solution_cost = get_solution_cost(p, tabu_solution);
//            auto solution_hash = hash_vector(tabu_solution);
            if (tabu_solution_cost > best_neighbourhood_cost) {// && std::find(tabu_list.begin(), tabu_list.end(), solution_hash) == tabu_list.end()) {
                best_neighbourhood_cost = tabu_solution_cost;
                best_neighbourhood_solution = tabu_solution;
//                best_neighbourhood_hash = solution_hash;
            }
        }
        if (best_neighbourhood_cost > best_solution_cost) {
#ifdef DEBUG
            std::cout << "New best solution cost: " << best_neighbourhood_cost << std::endl;
            std::cout << iteration << std::endl;
#endif
            best_solution_cost = best_neighbourhood_cost;
            best_solution = best_neighbourhood_solution;
        }

//        if (tabu_list.size() > MAX_TABU_LIST_SIZE) {
//            tabu_list.pop_front();
//        }
//        tabu_list.push_back(best_neighbourhood_hash);
    }
#ifdef DEBUG
    std::cout << "Final iterations: " << iteration << std::endl;
#endif
    return best_solution;
}