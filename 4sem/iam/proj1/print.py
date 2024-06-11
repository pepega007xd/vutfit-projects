#!/bin/env python3
import sys
import math

Board = list[list[bool]]


def is_occupied(board: Board, x: int, y: int):
    N = len(board)

    if x < 0 or x >= N or y < 0 or y >= N:
        return False

    return board[y][x]


def is_valid(board: Board, x: int, y: int) -> bool:
    N = len(board)

    for i in range(1, N):
        if (is_occupied(board, x + i, y)
            or is_occupied(board, x - i, y)
            or is_occupied(board, x, y + i)
            or is_occupied(board, x, y - i)
            or is_occupied(board, x + i, y + i)
            or is_occupied(board, x + i, y - i)
            or is_occupied(board, x - i, y + i)
                or is_occupied(board, x - i, y - i)):
            return False
    return True


def main() -> int:
    [result, *rest] = sys.stdin.read().splitlines()

    all_valid = True

    if result != "SAT":
        print(result)
        return 0

    values = list(map(int, rest[0].split()))
    N = math.isqrt(len(values))

    board: list[list[bool]] = [[] for _ in range(N)]

    for y in range(N):
        for x in range(N):
            board[y].append(values[y * N + x] > 0)

    for y in range(N):
        for x in range(N):
            if board[y][x]:
                if is_valid(board, x, y):
                    print("██", end="")
                else:
                    print("\x1b[31;1mXX\x1b[0m", end="")
                    all_valid = False
            else:
                print("  " if (x + y) % 2 else "░░", end="")
        print("")

    print("")
    if all_valid:
        print("Valid solution")
        return 0
    else:
        print("Invalid solution")
        return 1


if __name__ == "__main__":
    exit(main())
