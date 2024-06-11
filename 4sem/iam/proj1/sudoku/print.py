#!/bin/env python3
import sys

Cell = tuple[int, int]


def get_variable(cell: Cell, number: int) -> int:
    return cell[0] * 100 + cell[1] * 10 + number


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


def check_validity(board: list[list[int]], group: list[Cell]) -> bool:
    numbers = set()
    for cell in group:
        numbers.add(board[cell[0]][cell[1]])
    return numbers == set([1, 2, 3, 4, 5, 6, 7, 8, 9])


def main() -> int:
    [result, *rest] = sys.stdin.read().splitlines()

    all_valid = True

    if result != "SAT":
        print(result)
        return 0

    values = list(map(int, rest[0].split()))

    board: list[list[int]] = [[] for _ in range(10)]

    # indexing from 1 :wicked:
    for y in range(1, 10):
        board[y].append(0)
        for x in range(1, 10):
            for number in range(1, 10):
                if values.count(get_variable((y, x), number)) == 1:
                    board[y].append(number)
                    break

    try:
        for y in range(1, 10):
            if y % 3 == 1:
                print("--" * 12)
            for x in range(1, 10):
                if x % 3 == 1:
                    print(" |", end="")
                print(f" {board[y][x]}", end="")
            print("")
    except IndexError:
        print(f"\nERROR: no valid number found on position y={y} x={x}")
        exit(1)

    # groups of cells, in which the sudoku rule must hold
    # (one of each number must be present)
    groups = []
    groups += get_all_columns()
    groups += get_all_rows()
    groups += get_all_subsquares()

    for group in groups:
        if not check_validity(board, group):
            print(f"\ncell values at positions {group} are invalid")
            all_valid = False

    print("")
    if all_valid:
        print("Valid solution")
        return 0
    else:
        print("Invalid solution")
        return 1


if __name__ == "__main__":
    exit(main())
