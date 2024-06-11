#!/bin/env python3
#
# author: vit
#
# usage: z3 -smt2 queens.smt | python3 view.py
#
# works for
#  - IAM 2024 queens.smt
#  - Z3 version 4.8.10 - 64 bit
#  - Python 3.11.4
#
import re
import sys
import itertools

z3_output_lines = sys.stdin.read().splitlines()
z3_output = " ".join(z3_output_lines)

for result in ["unsat", "unknown", "timeout"]:
    if result in z3_output_lines[0]:
        print(f"no can do ({result})")
        exit()

# pattern to find bits like this example:
# "(define-fun y-pos-g () Int  3)"
PATTERN = r"\(\s*define-fun\s+y-pos-\w\s*\(\s*\)\s*Int\s+\d\s*\)"

# make array
arr = re.findall(PATTERN, z3_output)

# remove bloat
for i in range(len(arr)):
    s = arr[i]
    s = s.replace("define-fun", "")
    s = s.replace("y-pos-", "")
    s = s.replace("Int", "")
    s = s.replace("(", "")
    s = s.replace(")", "")
    s = s.replace(" ", "")
    arr[i] = s

# sort array
arr.sort()

# make sure array ok
assert len(arr) == 8
assert all(len(s) == 2 for s in arr)
assert all(s[0].isalpha() for s in arr)
assert all(s[1].isdigit() for s in arr)
assert all(isinstance(s, str) for s in arr)

# print array
print(arr)

# print the chessboard
for i in range(8):
    row = 8 - i
    print(row, end=" ")
    for col in range(1, 9):
        print("x" if int(arr[col - 1][1]) == row else " ", end="|")
    print()
print("  a b c d e f g h")

# functions below
###############################################################################


def indices(pos: str) -> tuple[int, int]:
    assert len(pos) == 2
    assert pos[0].isalpha()
    assert pos[1].isdigit()
    x, y = "abcdefgh".index(pos[0]), int(pos[1]) - 1
    assert in_range(x, y)
    return x, y


def pfi(pos: tuple[int, int]) -> str:
    """position from indices"""
    x, y = pos
    return "abcdefgh"[x] + str(y + 1)


def in_range(x, y) -> bool:
    return 0 <= x <= 8 and 0 <= y <= 8


def is_ok() -> bool:

    ok = True

    # positions
    queens = [indices(s) for s in arr]

    # queen count
    qc = 0

    # check rows
    for col_idx in range(8):
        qc = 0
        for row_idx in range(8):
            if (row_idx, col_idx) in queens:
                qc += 1
        if qc > 1:
            print(f"{qc} queens on row {pfi((0, col_idx))[1]}")

    # checking cols is not necessary

    # all the pairs of queens
    combinations = itertools.combinations(queens, 2)

    # check diagonals
    for q1, q2 in combinations:
        for i in range(-7, 8):
            if (q1[0] + i == q2[0] and q1[1] + i == q2[1]) or \
               (q1[0] + i == q2[0] and q1[1] - i == q2[1]):
                print(f"conflicting queens {pfi(q1)}, {pfi(q2)}")
                ok = False

    return ok


# code below
###############################################################################

if is_ok():
    print("valid solution")
else:
    print("not a valid solution :(")
