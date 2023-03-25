#!/usr/bin/env python3

import sys
import gurobipy as g
import numpy as np
from typing import List
import matplotlib.pyplot as plt
from tsp_solver import solve_shortest_hamiltonian_path

DEBUG = False


def read_stripes_from_file(input_filename: str) -> List[np.array]:
    """
    Read stripes from input file into list of np.arrays
    :param input_filename: input file of the format described in HW assignment
    :return: list of np.arrays for each stripe. Each of the stripe haas shape (h, w, 3)
    """
    file_data = open(input_filename).readlines()
    n, w, h = map(int, file_data[0].split())
    res = [np.zeros((h, w, 3), dtype=np.int32) for _ in range(n)]
    for i in range(n):
        stripe_gen = map(int, file_data[i + 1].strip().split())
        for h_i in range(h):
            for w_i in range(w):
                for c_i in range(3):
                    res[i][h_i][w_i][c_i] = next(stripe_gen)
    return res


def write_output_to_tile(output_filename, permutation):
    with open(output_filename, 'w') as f:
        print(' '.join(map(str, permutation + 1)), file=f)


def visualize_stripes(stripes: List[np.array], permutation: List[int]) -> None:
    """
    Visualize stripes
    :param stripes: Stripes encoding RGB colors, each of shape (h, w, 3)
    :param permutation: array with numbers from 0 to len(stripes) describing permutation of stripes
    """
    h, w, _ = stripes[0].shape
    image = np.hstack([stripes[i] for i in permutation])
    plt.imshow(image)
    plt.show()


def get_difference_matrix(stripes: List[np.array]):
    """
    Get NxN matrix where M[i, j] is the difference between right edges of stripe i and left edge of stripe j
    :param stripes: stripes of shape (h, w, 3)
    :return:
    """
    n = len(stripes)

    left_edges = np.hstack([[s[:, 0, :] for s in stripes]]).transpose(1, 0, 2)
    right_edges = np.hstack([[s[:, -1, :] for s in stripes]]).transpose(1, 0, 2)
    diff_matrix = np.zeros((n, n))

    # TODO: maybe this can be done without this cycle
    for i in range(n):
        edges_diff = np.sum(np.abs(left_edges - right_edges[:, i:(i + 1), :]), axis=(0, 2))
        diff_matrix[i] = edges_diff

    return diff_matrix


def main(input_file: str, output_file: str):
    stripes = read_stripes_from_file(input_file)
    if DEBUG:
        visualize_stripes(stripes, list(range(len(stripes))))
    diff_matrix = get_difference_matrix(stripes)
    solution_perm = solve_shortest_hamiltonian_path(diff_matrix)
    if DEBUG:
        visualize_stripes(stripes, solution_perm)
    write_output_to_tile(output_file, solution_perm)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Error. Too few input arguments")
    else:
        main(sys.argv[1], sys.argv[2])
