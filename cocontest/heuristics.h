#ifndef COCONTEST_HEURISTICS_HEURISTICS_H
#define COCONTEST_HEURISTICS_HEURISTICS_H

#include <vector>
#include "common_types.h"
#include "Problem.h"

/*!
 * Find all cycles in the solution
 * NOTE: cycles in solution are unique and disjoint. There are not two ways to find two different sets of disjoint cycles when one does not fully contain another
 * @param p Problem to which ths solution is found
 * @param solution Solution to the problem for which cycles will be found
 * @return Cycles as vectors of nodes. The first and last elements are different
 */
std::vector<std::vector<node_idx_t>> find_cycles(const Problem &p, const std::vector<node_idx_t> &solution);

/*!
 * Create a cycle in the problem that is disjoint with all the existing cycles in the solution
 * @param p Problem to which the solution is found
 * @param cur_solution Solution which will be updated
 * @param random_order True if the order of successors visiting in DFS is random. False leads to visiting nodes from the one with
 * largest weight to the one with smallest
 * @return True if a new cycle was formed
 */
bool create_random_cycle(const Problem &p, std::vector<node_idx_t> &cur_solution, bool random_order=false);

/*!
 * Add nodes to existing cycles. Selects nodes that are not in any cycle and tries to insert them into existing cycles
 * @param p Problem solution to which was found
 * @param cur_solution solution that will by updated
 * @param constrain_cycle_length If true, nodes can be added only to cycles with length <= p.L - 1 to create only valid cycles
 * @return True if at least one node was added to a cycle
 */
bool add_to_cycles(const Problem &p, std::vector<node_idx_t> &cur_solution, bool constrain_cycle_length=false);

/*!
 * Break a random cycle in solution by setting successors of each node of that cycle to a random index
 * @param p Problem solution to which was found
 * @param cur_solution solution that will be updated
 * @return True if a cycle was broken
 */
bool break_random_cycle(const Problem &p, std::vector<node_idx_t> &cur_solution);

/*!
 * Shorten long cycles by splitting them into two in the best possible position
 * @param p Problem solution to which was found
 * @param cur_solution solution that will be updated
 * @return True if a cycle was shortened
 */
bool shorten_long_cycles(const Problem &p, std::vector<node_idx_t> &cur_solution);


#endif //COCONTEST_HEURISTICS_HEURISTICS_H
