#!/usr/bin/env python3
import sys
import numpy as np
from lazy_constraints_ilp import solve_with_lazy_constraints


def read_input_data(input_filename: str) -> (int, list):
    """
    Read problem input from file of specified format in the task assignment
    :param input_filename: path to the input file
    :return: longest allowed cycle distance and an adjacency matrix with weights equal to compatibility
    """
    lines = open(input_filename).readlines()
    n, m, L = map(int, lines[0].split())
    adjacency_matrix = np.ones((n, n))
    for i in range(1, m + 1):
        source, dest, weight = lines[i].split()
        source, dest = int(source), int(dest)
        adjacency_matrix[source][dest] = -float(weight)

    return L, adjacency_matrix


def write_output_to_file(output_filename: str, result) -> None:
    with open(output_filename, 'w') as f:
        if not result[0]:
            print('0.0', file=f)
            return
        print(round(result[0], 2), file=f)
        for (i, j) in result[1]:
            print(i, j, file=f)


def solve_optimal(input_filename, output_filename):
    problem_instance = read_input_data(input_filename)
    if problem_instance[0] == 1:
        solution = [0, []]
    else:
        solution = solve_with_lazy_constraints(*problem_instance)
    write_output_to_file(output_filename, solution)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Too few arguments...")
        exit(1)
    solve_optimal(sys.argv[1], sys.argv[2])
