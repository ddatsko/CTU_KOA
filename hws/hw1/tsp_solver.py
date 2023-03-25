import gurobipy as g
import numpy as np


def solve_tsp(c):
    def integer_solution_callback(model_, where_):
        if where_ == g.GRB.Callback.MIPSOL:
            n_ = c.shape[0]
            visited_ = [False] * n_
            # Iterate through each node and if it does not belong to any previous cycle, find a loop from it
            for i in range(n):
                if not visited_[i]:
                    # Run dfs (knowing that from each node there is only one edge) to find loop starting from node i
                    loop_nodes = [i]
                    visited_[i] = True

                    loop_found = False
                    while not loop_found:
                        for j in range(n_):
                            x_ij = model_.cbGetSolution(x[loop_nodes[-1], j])
                            if x_ij >= 0.5:
                                loop_nodes.append(j)
                                if visited_[j]:
                                    loop_found = True
                                visited_[j] = True
                                break
                    if len(loop_nodes) == n + 1:
                        break

                    loop_pairs = [(loop_nodes[j], loop_nodes[j + 1]) for j in range(len(loop_nodes) - 1)]
                    model_.cbLazy(g.quicksum([x[l[0], l[1]] for l in loop_pairs]) <= len(loop_pairs) - 1)

    model = g.Model()
    model.Params.lazyConstraints = 1

    n = c.shape[0]
    x = model.addVars(n, n, vtype=g.GRB.BINARY)

    for i in range(n):
        model.addConstr(g.quicksum([x[i, j] for j in range(n)]) == 1)
        model.addConstr(g.quicksum([x[j, i] for j in range(n)]) == 1)

    model.setObjective(g.quicksum([x[i, j] * c[i][j] for i in range(n) for j in range(n)]))
    model.optimize(integer_solution_callback)

    res = [0]
    for i in range(n):
        for j in range(n):
            if x[res[-1], j].x == 1:
                res.append(j)
                break
    return res[1:-1]


def solve_shortest_hamiltonian_path(diff_matrix):
    n, _ = diff_matrix.shape
    diff_matrix = np.hstack([np.zeros((n, 1)), diff_matrix])
    diff_matrix = np.vstack([np.zeros((1, n + 1)), diff_matrix])
    solution = solve_tsp(diff_matrix)
    return np.array(solution) - 1
