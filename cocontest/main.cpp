#include <iostream>
#include "Problem.h"
#include <vector>
#include "tabu_search.h"
#include <map>
#include "heuristics.h"
#include <fstream>

void
write_solution_to_file(const std::string &output_filename, const Problem &p, const std::vector<node_idx_t> &solution) {
    auto cost = get_solution_cost(p, solution);
    std::ofstream os{output_filename};
    os << cost << std::endl;
    std::cout << "Cost: " << cost << std::endl;
    auto cycles = find_cycles(p, solution);
    for (const auto &c: cycles) {
        if (c.size() > p.L) {
            continue;
        }
        auto s = c.size();

        for (size_t i = 0; i < s; ++i) {
          os << c[i] << " " << c[(i + 1) % s] << "\n";
        }
    }
    return;
}


int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Too few arguments. At least two are needed: input_filename and output_filename " << std::endl;
        return -1;
    }
    Problem p = Problem::from_config_file(argv[1]);
#ifdef DEBUG
    std::cout << "Read a problem with n = " << p.n << ", L = " << p.L << std::endl;
#endif
    double time_limit = std::atof(argv[3]);
    std::vector<node_idx_t> solution(p.n);
    solution = solve_tabu_search(p, solution, static_cast<long long>(time_limit * 1000000));
    write_solution_to_file(argv[2], p, solution);
}