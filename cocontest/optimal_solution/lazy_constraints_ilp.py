import numpy as np
import gurobipy as g
from typing import List, Tuple


def solve_with_lazy_constraints(L: int, adjacency_matrix: np.array) -> (float, List[Tuple[int, int]]):
    """
    Find the optimal solution to the problem using lazy constraints' method
    :param L: maximum allowed loop length
    :param adjacency_matrix: adjacency matrix of compatibility graph
    :return: list of pairs [(i, j), ...] of selected edges (may be empty)
    """

    def integer_solution_callback(model_, where_):
        if where_ == g.GRB.Callback.MIPSOL:
            n_ = len(adjacency_matrix)
            visited_ = [False] * n_
            # Iterate through each node and if it does not belong to any previous cycle, find a loop from it
            for i in range(n_):
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
                        else:
                            # Should reach here only for single unused nodes
                            loop_found = True
                            break
                    if len(loop_nodes) <= L + 1:
                        continue

                    loop_pairs = [(loop_nodes[j], loop_nodes[j + 1]) for j in range(len(loop_nodes) - 1)]
                    model_.cbLazy(g.quicksum([x[l[0], l[1]] for l in loop_pairs]) <= len(loop_pairs) - 1)

    model = g.Model()
    model.Params.lazyConstraints = 1
    n = len(adjacency_matrix)

    x = model.addVars(n, n, vtype=g.GRB.BINARY, obj=adjacency_matrix)

    # Add constraints
    for i in range(n):
        model.addConstr(x[i, i] == 0)
        model.addConstr(g.quicksum([x[i, j] for j in range(n)]) <= 1)  # Not more than one leaving edge
        # The same amount of leaving edges as incoming ones
        model.addConstr(g.quicksum([x[j, i] for j in range(n)]) == g.quicksum([x[i, j] for j in range(n)]))
        for j in range(n):
            # If compatibility is 0, make picking the edge impossible
            if adjacency_matrix[i][j] == 1:
                model.addConstr(x[i, j] == 0)

    model.optimize(integer_solution_callback)

    res = []
    for i in range(n):
        for j in range(n):
            # 0.5 juts to make sure that no numerical problems cause mistakes
            if x[i, j].x >= 0.5:
                res.append((i, j))
    return -model.getObjective().getValue(), res
