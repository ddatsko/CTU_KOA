#ifndef COCONTEST_HEURISTICS_PROBLEM_H
#define COCONTEST_HEURISTICS_PROBLEM_H

#include <vector>
#include "common_types.h"
#include <string>

/*!
 * Struct representing the problem instance.
 */
struct Problem {
    node_idx_t n{}, L{}; // Number of vertices, edges, max loop length
    // Using both adjacency matrix and adjacency list for both fast retrieving of edge value and node neighbours
    std::vector<std::vector<bool>> adj_m;
    std::vector<std::vector<weight_t>> w;

    // Vector of pairs in format <node, weight>
    std::vector<std::vector<std::pair<node_idx_t, weight_t>>> adj_l;

    // Constructors. Not all are needed but let it be
    Problem() = default;
    Problem(Problem &p) = default;
    Problem(const Problem &p) = default;
    Problem(node_idx_t n, node_idx_t L);

    /*!
     * Factory method to create a problem from a config file
     * @param filename Path to the configuration file of format specified in the task
     * @return properly initialized Problem instance
     */
    static Problem from_config_file(const std::string &filename);
};


#endif //COCONTEST_HEURISTICS_PROBLEM_H
