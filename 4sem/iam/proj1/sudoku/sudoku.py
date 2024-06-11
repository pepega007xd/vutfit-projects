#!/bin/env python3
import sys

"""
Usage:
python3 sudoku.py
"""

# represents a single position, indexed from one,
# row number is first, column number is second
Cell = tuple[int, int]


def get_variable(cell: Cell, number: int) -> str:
    """
    converts Cell and number to CNF variable name (number)
    according to assignment (123 is row 1, column 2, occupied by number 3)
    """
    return f"{cell[0]}{cell[1]}{number}"


def at_least_one(cell: Cell) -> str:
    """
    generates clauses that guarantee that there is
    at least one number in the cell
    """
    clause = ""
    for number in range(1, 10):
        clause += get_variable(cell, number) + " "
    clause += "0"
    return clause


def at_most_one(cells: list[Cell], number: int) -> list[str]:
    """
    generates clauses that guarantee that at most one
    of the input cells is occupied by the number - for each pair of cells,
    we add the clause (or (not cell_1) (not cell_2))
    """
    clauses = []
    for i, cell_1 in enumerate(cells):
        for cell_2 in cells[(i + 1):]:
            clauses.append(
                f"-{get_variable(cell_1, number)} -{get_variable(cell_2, number)} 0")
    return clauses


def get_all_rows() -> list[list[Cell]]:
    """
    returns list of all rows, each row is a list of its cells
    """
    rows = []
    for row in range(1, 10):
        row_cells = []

        for col in range(1, 10):
            row_cells.append((row, col))

        rows.append(row_cells)

    return rows


def get_all_columns() -> list[list[Cell]]:
    """
    returns list of all columns, each column is a list of its cells
    """
    cols = []
    for col in range(1, 10):
        col_cells = []

        for row in range(1, 10):
            col_cells.append((row, col))

        cols.append(col_cells)

    return cols


def get_subsquare(y: int, x: int) -> list[Cell]:
    """
    returns a list of cells inside a subsquare at position (y,x),
    indexed from zero
    """
    cells = []
    for row in range(1, 4):
        row += 3 * y
        for col in range(1, 4):
            col += 3 * x
            cells.append((row, col))

    return cells


def get_all_subsquares() -> list[list[Cell]]:
    """
    returns list of all subsquares, which divide the sudoku into 9 parts,
    each subsquare is a list of its cells
    """
    subsquares = []
    for y in range(3):
        for x in range(3):
            subsquares.append(get_subsquare(y, x))

    return subsquares


def get_all_cells() -> list[Cell]:
    cells = []
    for row in range(1, 10):
        for col in range(1, 10):
            cells.append((row, col))
    return cells


def print_help():
    """
    prints command line help and exits when arguments are found
    """
    if len(sys.argv) > 1:
        print(f"""
Usage: {sys.argv[0]}
    problem input should be passed via stdin in the form specified by assignment:
    "1 2 3" means "row 1, column 2 is occupied by number 3"
        """,
            file=sys.stderr)
        exit(1)


def line_to_constraint(line: str) -> str:
    [row, col, number] = list(map(int, line.split()))
    return f"{get_variable((row, col), number)} 0"


def main() -> int:
    print_help()

    clauses = []

    # groups of cells, in which the sudoku rule must hold
    # (one of each number must be present)
    groups = []
    groups += get_all_columns()
    groups += get_all_rows()
    groups += get_all_subsquares()

    # each cell must be occupied by a number
    cells = get_all_cells()
    for cell in cells:
        for number in range(1, 10):
            clauses.append(at_least_one(cell))

    # general rules for sudoku (no two same numbers in row/column/subsquare)
    for group in groups:
        for number in range(1, 10):
            # in each group (row, column, subsquare),
            # there has to be at most one of each number (1-9)
            clauses += at_most_one(group, number)

    # additional contraints from input (pre-filled numbers)
    lines = sys.stdin.read().splitlines()
    clauses += list(map(line_to_constraint, lines))

    # print DIMACS header and clauses
    print(f"p cnf {9 * 9 * 9} {len(clauses)}")
    print("\n".join(clauses))

    return 0


if __name__ == "__main__":
    sys.exit(main())
