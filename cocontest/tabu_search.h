
#ifndef COCONTEST_HEURISTICS_TABU_SEARCH_H
#define COCONTEST_HEURISTICS_TABU_SEARCH_H

#include <vector>
#include "common_types.h"
#include "Problem.h"

/*!
 * Get the cost of the solution. Finds cycles of valid length and sums all the node in them
 * @param p Problem for which the solution is found
 * @param solution Solution to the corresponding problem
 * @return
 */
weight_t get_solution_cost(const Problem &p, const std::vector<node_idx_t> &solution);

/*!
 * Solve the problem using tabu search
 * @param p Problem to solve
 * @param solution Initial solution
 * @param max_time_us Time limit for the function in microseconds. NOTE: the larger the input problem is the bigger deviation of run time from this value may be
 * @return Solution to the problem as list of successor indices for each node
 */
std::vector<node_idx_t> solve_tabu_search(const Problem &p, std::vector<node_idx_t> solution, long long max_time_us);

#endif //COCONTEST_HEURISTICS_TABU_SEARCH_H
