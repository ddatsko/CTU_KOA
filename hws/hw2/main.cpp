#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <limits>

// Global variables used throughout the whole solution. Not a good practice, but OK for such application
int C, P;
const int INF = std::numeric_limits<int>::max();

enum VISITED {
    NOT_VISITED = 0,
    DIRECT = 1,
    BACK = 2
};

struct Edge {
    int to;
    int l;
    int u;
    int f;
};

struct Problem {
    int n;
    int s;
    int t;
    // Keeping both out and in adjacent edges for each node. Not a nice solution as when updating two of them need to be updated but let it be for now
    std::vector<std::vector<Edge>> out;
    std::vector<std::vector<Edge>> in;

    explicit Problem(int n) : n{n} {
        out = std::vector<std::vector<Edge>>(n);
        in = std::vector<std::vector<Edge>>(n);
        s = n - 2;
        t = n - 1;
    }

    void add_flow(int from, int to, int f) {
        // Find the right edge in out edges
        for (auto &e: out[from]) {
            if (e.to == to) {
                e.f += f;
                break;
            }
        }

        // Find the edge in the in edges
        for (auto &e: in[to]) {
            if (e.to == from) {
                e.f += f;
                break;
            }
        }
    }

    void print_flow() const {
        for (size_t i = 0; i < n; ++i) {
            std::cout << "Node: " << i << ": ";
            for (const auto &e: out[i]) {
                if (e.f != 0) {
                    std::cout << "| " << e.to << ", " << e.f << " |   ";
                }
            }
            std::cout << std::endl;
        }
    }

    void print_upper() const {
        for (size_t i = 0; i < n; ++i) {
            std::cout << "Node: " << i << ": ";
            for (const auto &e: out[i]) {
                std::cout << "| " << e.to << ", " << e.u << " |   ";
            }
            std::cout << std::endl;
        }
    }
};

//! Read the problem instance from file and add 2 additional nodes in the end:
//! s, t
//! \param input_filename Path to the input file in format specified in the task
//! \return Problem instance with 2 additional nodes added in the end: s, tint this order
Problem read_extended_problem(const std::string &input_filename) {
    std::ifstream is{input_filename};
    is >> C >> P;
    // Create problem and indices of additional_nodes
    Problem problem{C + P + 2};

    std::string line;
    std::getline(is, line);  // Skip the first row with no numbers left
    int l, u, p;
    for (int i = 0; i < C; ++i) {
        std::getline(is, line);
        std::stringstream ss{line};
        ss >> l >> u;
        problem.out[problem.s].push_back(Edge{i, l, u, 0});
        problem.in[i].push_back(Edge{problem.s, l, u, 0});
        while (ss >> p) {
            --p;
            problem.out[i].push_back(Edge{C + p, 0, 1, 0});
            problem.in[C + p].push_back(Edge{i, 0, 1, 0});
        }
    }

    // Read product reviews needs
    std::getline(is, line);
    std::stringstream ss{line};
    for (p = 0; p < P; ++p) {
        int need;
        ss >> need;
        problem.out[C + p].push_back(Edge{problem.t, need, INF, 0});
        problem.in[problem.t].push_back(Edge{C + p, need, INF, 0});
    }

    return problem;
}

void write_output_to_file(const std::string &output_filename, bool solved, const Problem &p) {
    std::ofstream os{output_filename};
    if (!solved) {
        os << -1;
        os.close();
        return;
    }
    for (int i = 0; i < C; ++i) {
        bool first = true; // To avoid space in the end of the row
        for (const auto &e: p.out[i]) {
            if (e.f == 1) {
                os << (first ? "" : " ") << e.to - C + 1;
                first = false;
            }
        }
        if (i != C - 1) os << "\n";
    }
}

//! Solve the problem using Edmonds Karp algorithm
//! \param p A valid initial solution, for which values of f will be assigned
void solve_edmonds_karp(Problem &p) {
    while (true) {

        // Run the BFS while filling predecessors
        std::vector<int> pred(static_cast<size_t>(p.n));
        std::vector<VISITED> visited(static_cast<size_t>(p.n));
        std::queue<std::pair<int, int>> q;
        visited[p.s] = DIRECT;
        q.emplace(p.s, INF);
        int augmenting_flow = 0;

        while (!q.empty()) {
//            auto [v, possible_flow] = q.front();
            auto v = q.front().first;
            auto possible_flow = q.front().second;
            if (v == p.t) {
                augmenting_flow = possible_flow;
                break;
            }
            q.pop();
            // Go through outcoming edges
            for (const auto &to: p.out[v]) {
                if (visited[to.to] || to.f == to.u) {
                    continue;
                }
                visited[to.to] = DIRECT;
                q.emplace(to.to, std::min(possible_flow, to.u - to.f));
                pred[to.to] = v;
            }

            // Go through incoming edges
            for (const auto &to: p.in[v]) {
                if (visited[to.to] || to.f == to.l) {
                    continue;
                }
                visited[to.to] = BACK;
                q.emplace(to.to, std::min(possible_flow, to.f - to.l));
                pred[to.to] = v;
            }
        }

        // Stop when there is no augmenting path found
        if (visited[p.t] == NOT_VISITED) {
            break;
        }

        // Back propagate through the path and update the flow along it
        auto v = p.t;
        while (v != p.s) {
            auto prev = pred[v];
            if (visited[v] == DIRECT) {
                p.add_flow(prev, v, augmenting_flow);
            } else {
                p.add_flow(v, prev, -augmenting_flow);
            }
            v = prev;
        }
    }
}


bool solve_with_lower_bounds(Problem &p) {
    Problem extended_problem{p.n + 2};
    // Copy all the edges from the original problem but replace flow bounds
    for (size_t i = 0; i < p.n; ++i) {
        for (const auto &e: p.out[i]) {
            extended_problem.out[i].push_back(Edge{e.to, 0, e.u - e.l, 0});
        }
        for (const auto &e: p.in[i]) {
            extended_problem.in[i].push_back(Edge{e.to, 0, e.u - e.l, 0});
        }
    }

    // Add an edge from t to s
    extended_problem.out[p.t].push_back(Edge{p.s, 0, INF, 0});
    extended_problem.in[p.s].push_back(Edge{p.t, 0, INF, 0});

    // Add edges from s' and t' to nodes
    for (size_t i = 0; i < p.n; ++i) {
        int l_v = 0;
        for (const auto &e: p.in[i]) {
            l_v += e.l;
        }
        for (const auto &e: p.out[i]) {
            l_v -= e.l;
        }
        if (l_v > 0) {
            extended_problem.out[extended_problem.s].push_back(Edge{static_cast<int>(i), 0, l_v, 0});
            extended_problem.in[i].push_back(Edge{extended_problem.s, 0, l_v, 0});
        } else if (l_v < 0) {
            extended_problem.out[i].push_back(Edge{extended_problem.t, 0, -l_v, 0});
            extended_problem.in[extended_problem.t].push_back(Edge{static_cast<int>(i), 0, -l_v, 0});
        }
    }

    solve_edmonds_karp(extended_problem);

    // If the flow does not saturate all the out nodes from the source -- no feasible solution exists
    for (const auto &e: extended_problem.out[extended_problem.s]) {
        if (e.f != e.u) {
            return false;
        }
    }

    // Update the initial feasible solution to the original problem
    for (size_t i = 0; i < p.n; ++i) {
        for (size_t j = 0; j < p.out[i].size(); ++j) {
            p.out[i][j].f = p.out[i][j].l + extended_problem.out[i][j].f;
        }
        for (size_t j = 0; j < p.in[i].size(); ++j) {
            p.in[i][j].f = p.in[i][j].l + extended_problem.in[i][j].f;
        }
    }

    solve_edmonds_karp(p);
    return true;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Wrong number of arguments. Must be 2";
        return -1;
    }
    auto problem = read_extended_problem(argv[1]);
    auto solved = solve_with_lower_bounds(problem);
    write_output_to_file(argv[2], solved, problem);

    return 0;
}
