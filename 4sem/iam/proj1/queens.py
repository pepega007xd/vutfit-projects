#!/bin/env python3
import sys

"""
Usage:
python3 queens.py <n>
"""

# represents a single position on the board, indexed from zero
Cell = tuple[int, int]

# side length of board
N: int


def get_variable(cell: Cell) -> str:
    """
    converts Cell to CNF variable name (number),
    variable numbers describe the chess board in this way:
    1 2 3
    4 5 6
    7 8 9
    """
    global N
    return str(cell[0] * N + cell[1] + 1)


def at_least_one(cells: list[Cell]) -> list[str]:
    """
    generates clauses that guarantee that at least
    one of the input cells is occupied
    """
    clause = ""
    for cell in cells:
        clause += (get_variable(cell) + " ")
    clause += "0"
    return [clause]


def at_most_one(cells: list[Cell]) -> list[str]:
    """
    generates clauses that guarantee that at most one
    of the input cells is occupied - for each pair of cells,
    we add the clause (or (not cell_1) (not cell_2))
    """
    clauses = []
    for i, cell_1 in enumerate(cells):
        for cell_2 in cells[(i + 1):]:
            clauses.append(
                f"-{get_variable(cell_1)} -{get_variable(cell_2)} 0")
    return clauses


def parse_arg() -> int:
    """
    returns the side length of board 'N' parsed from argv
    """
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <n>\n'n' is the side length of the board",
              file=sys.stderr)
        exit(1)

    try:
        if int(sys.argv[1]) > 0:
            return int(sys.argv[1])
        else:
            raise ValueError
    except ValueError:
        print("'n' must be positive integer", file=sys.stderr)
        exit(1)


def main() -> int:
    global N
    N = parse_arg()

    clauses = []

    # iterates through all rows and columns
    for y in range(N):
        row = []
        column = []

        for x in range(N):
            row.append((y, x))
            column.append((x, y))

        # by guaranteeing that in each row there is at least one queen, we set
        # the total number of queens to N (there cannot be less than N because
        # of this rule, and not more than N because of the other contstraints)
        clauses += at_least_one(row)

        # there cannot be two queens in a single row or column
        clauses += at_most_one(row)
        clauses += at_most_one(column)

    # assert that in each diagonal longer than one cell, there can
    # be at most one queen
    for i in range(1, N-1):
        diagonal_length = N - i
        major_upper = []
        major_lower = []
        minor_upper = []
        minor_lower = []

        # these are only the "off-center" diagonals, the longest two are below
        for j in range(diagonal_length):
            major_upper.append((j, i + j))
            major_lower.append((i + j, j))
            minor_upper.append((j, N - 1 - i - j))
            minor_lower.append((i + j, N - 1 - j))

        clauses += at_most_one(major_upper)
        clauses += at_most_one(major_lower)
        clauses += at_most_one(minor_upper)
        clauses += at_most_one(minor_lower)

    # main diagonals are done separately to simplify the code above
    main_major = []
    main_minor = []

    for i in range(N):
        main_major.append((i, i))
        main_minor.append((i, N - 1 - i))

    clauses += at_most_one(main_major)
    clauses += at_most_one(main_minor)

    # print DIMACS header and clauses

    print(f"p cnf {N * N} {len(clauses)}")
    print("\n".join(clauses))

    return 0


if __name__ == "__main__":
    sys.exit(main())
