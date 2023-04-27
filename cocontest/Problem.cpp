#include "Problem.h"
#include <fstream>
#include <algorithm>

Problem::Problem(node_idx_t n, node_idx_t L): n{n}, L{L} {
    w = std::vector<std::vector<weight_t>>(n);
    adj_m = std::vector<std::vector<bool>>(n);
    adj_l = std::vector<std::vector<std::pair<node_idx_t, weight_t>>>(n);

    for (node_idx_t i = 0; i < n; ++i) {
        w[i] = std::vector<weight_t>(n);
        adj_m[i] = std::vector<bool>(n);
    }
}

Problem Problem::from_config_file(const std::string &filename) {
    std::ifstream fs{filename};
    node_idx_t n, m, L;
    fs >> n >> m >> L;
    Problem p{n, L};

    node_idx_t from, to;
    weight_t weight;
    for (size_t i = 0; i < m; ++i) {
        fs >> from >> to >> weight;
        p.adj_m[from][to] = true;
        p.w[from][to] = weight;
        p.adj_l[from].emplace_back(to, weight);
    }
    for (auto &l: p.adj_l) {
        std::sort(l.begin(), l.end(), [&](const std::pair<node_idx_t, weight_t> &p1, const std::pair<node_idx_t, weight_t> &p2){return p1.second > p2.second;});
    }

    return p;
}
