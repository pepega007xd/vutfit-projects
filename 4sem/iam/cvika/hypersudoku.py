from z3 import *
# 9x9 matrix of integer variables
X = [[Int("x_%s_%s" % (i+1, j+1)) for j in range(9)]
     for i in range(9)]

# each cell contains a value in {1, ..., 9}
cells_c = [And(1 <= X[i][j], X[i][j] <= 9)
           for i in range(9) for j in range(9)]

# each row contains a digit at most once
rows_c = [Distinct(X[i]) for i in range(9)]

# each column contains a digit at most once
cols_c = [Distinct([X[i][j] for i in range(9)])
          for j in range(9)]

# each 3x3 square contains a digit at most once
sq_c = [Distinct([X[3*i0 + i][3*j0 + j]
                  for i in range(3) for j in range(3)])
        for i0 in range(3) for j0 in range(3)]

# those 4 extra squares for hypersudoku
hypersudoku1 = [Distinct([X[1 + i][1 + j]
                          for i in range(3) for j in range(3)])]
hypersudoku2 = [Distinct([X[1 + i][5 + j]
                          for i in range(3) for j in range(3)])]
hypersudoku3 = [Distinct([X[5 + i][1 + j]
                          for i in range(3) for j in range(3)])]
hypersudoku4 = [Distinct([X[5 + i][5 + j]
                          for i in range(3) for j in range(3)])]

sudoku_c = cells_c + rows_c + cols_c + sq_c + \
    hypersudoku1 + hypersudoku2 + hypersudoku3 + hypersudoku4

# sudoku instance, we use '0' for empty cells
instance = ((0, 0, 0, 7, 9, 5, 0, 0, 0),
            (0, 0, 0, 0, 0, 2, 0, 0, 4),
            (5, 0, 3, 0, 0, 0, 0, 0, 6),
            (0, 0, 0, 0, 0, 0, 1, 0, 9),
            (8, 0, 0, 0, 0, 0, 3, 7, 0),
            (3, 0, 0, 0, 0, 0, 4, 0, 0),
            (9, 1, 0, 0, 6, 0, 0, 0, 2),
            (0, 0, 0, 2, 0, 0, 0, 0, 0),
            (0, 5, 0, 0, 1, 0, 0, 0, 0))

instance_c = [If(instance[i][j] == 0,
                 True,
                 X[i][j] == instance[i][j])
              for i in range(9) for j in range(9)]

s = Solver()
s.add(sudoku_c + instance_c)
if s.check() == sat:
    m = s.model()
    r = [[m.evaluate(X[i][j]) for j in range(9)]
         for i in range(9)]
    print_matrix(r)
else:
    print("failed to solve")
