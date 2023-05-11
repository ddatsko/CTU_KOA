#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>

struct problem_t {
    int n{};
    std::vector<int> p;
    std::vector<int> r;
    std::vector<int> d;
};

problem_t read_input_from_file(const std::string &filename) {
    problem_t p;
    std::ifstream is{filename};
    is >> p.n;
    p.p.resize(p.n);
    p.r.resize(p.n);
    p.d.resize(p.n);
    for (int i = 0; i < p.n; ++i) {
        is >> p.p[i] >> p.r[i] >> p.d[i];
    }
    return p;
}

std::vector<int> get_visiting_order(const problem_t &p) {
    // If all the release times are equal, the best nodes visiting order is by sorting them by deadlines, so abuse this here
    std::vector<std::pair<int, int>> deadlines;
    deadlines.reserve(p.n);
    for (int i = 0; i < p.n; ++i) {
        deadlines.emplace_back(p.d[i], i);
    }
    std::sort(deadlines.begin(), deadlines.end());
    std::vector<int> res(p.n);
    for (int i = 0; i < p.n; ++i) {
        res[i] = deadlines[i].second;
    }
    return res;
}

namespace {
    // Using global variables here. Not nice, but for such application let it be. Can replace with some state
    // variable changed by each DFS iteration internally
    std::vector<int> g_res;
    std::vector<int> g_visiting_order;
    std::vector<int> g_to_visit;

}

/*!
 * @param p problem
 * @param v current node
 * @param depth depth in the tree
 * @param visited array marking visited before nodes
 * @param c computation time of all the tasks before
 * @return pair {<solution found>, <solution can be found>}. If both are false, the algorithm should terminate as there is no possiblity to find the optimal solution anymore
 */
std::pair<bool, bool> solve_scheduling_dfs(const problem_t &p, int v, int depth, std::vector<bool> &visited, int c) {
    visited[v] = true;

    if (std::max(c, p.r[v]) + p.p[v] > p.d[v]) {
        visited[v] = false;
        return {false, true};
    }

    // If reached a leaf, the solution is feasible. Return true and fill in the res array
    if (depth == p.n) {
        g_res[depth - 1] = v;
        return {true, true};
    }

    int min_release_time = std::numeric_limits<int>::max();
    int max_deadline = 0;
    int total_work_left = 0;
    for (int i = 0; i < p.n; ++i) {
        if (!visited[i]) {
            min_release_time = std::min(min_release_time, p.r[i]);
            max_deadline = std::max(max_deadline, p.d[i]);
            total_work_left += p.p[i];
        }
    }

    int c_new = std::max(c, p.r[v]) + p.p[v];
    // Set if the first part of the solution is already optimal
    bool already_optimal = false;
    if (min_release_time >= c_new) {
        already_optimal = true;
    }


    // If there is no possibility to find any feasible solution in this subtree, return false
    if (std::max(c_new, min_release_time) + total_work_left > max_deadline) {
        visited[v] = false;
        return {false, ~already_optimal};
    }


    for (int i = 0; i < p.n; ++i) {
        int node = g_visiting_order[i];
        if (visited[node])
            continue;

        auto dfs_res = solve_scheduling_dfs(p, node, depth + 1, visited, c_new);
        if (dfs_res.first) {
            g_res[depth - 1] = v;
            visited[v] = false;
            return {true, true};
        }
        if (!dfs_res.second) {
            visited[v] = false;
            return {false, false};
        }
    }
    visited[v] = false;
    return {false, ~already_optimal};
}


std::vector<int> solve_scheduling(const problem_t &p) {
    g_visiting_order = get_visiting_order(p);
    g_res = std::vector<int>(p.n);
    g_to_visit.reserve(p.n);
    std::vector<bool> visited(p.n, false);
    for (int i = 0; i < p.n; ++i) {
        auto solver_res = solve_scheduling_dfs(p, i, 1, visited, 0);
        if (solver_res.first) {
            return g_res;
        }
        if (!solver_res.second) {
            return {};
        }
    }
    return {};
}

void write_solution_to_file(const problem_t &p, const std::vector<int> &sol, const std::string &output_filename) {
    std::ofstream of{output_filename};
    if (sol.empty()) {
        of << -1 << std::endl;
        return;
    }

    int c = 0;
    std::vector<int> start_times{p.n};
    for (int i = 0; i < p.n; ++i) {
        int start_time = std::max(c, p.r[sol[i]]);
        start_times[sol[i]] = start_time;
        c += p.p[sol[i]];
    }

    for (int i = 0; i < p.n; ++i) {
        of << start_times[i] << std::endl;
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Error. Too few arguments" << std::endl;
        return -1;
    }
    auto p = read_input_from_file(argv[1]);
    auto solution = solve_scheduling(p);
    write_solution_to_file(p, solution, argv[2]);
    return 0;
}
