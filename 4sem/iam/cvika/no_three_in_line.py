from z3 import *

GRID_SIZE = 10
POINTS_TO_PLACE = 20


# True iff [y, x] is inside the current grid
def is_inside(y, x):
    return And(y < GRID_SIZE,
               y >= 0,
               x < GRID_SIZE,
               x >= 0)


# True iff pairs of ints pos1 and pos2 are distinct in at least one component
def distinct_pair(pos1, pos2):
    return Or(Distinct(pos1[0], pos2[0]),
              Distinct(pos1[1], pos2[1]))


# true iff all pairs of ints in `pairs` are distinct to all other pairs
def distinct_pairs(pairs):
    constraints = []
    for i, pos1 in enumerate(pairs):
        for pos2 in pairs[i+1:]:
            constraints.append(distinct_pair(pos1, pos2))
    return And(*constraints)


# occupied: (y, x) -> Bool
# true iff there is a point at the position [y, x]
# this is how we encode the solution
occupied = Function("occupied", IntSort(), IntSort(), BoolSort())

solver = Solver()

##############################################
# Constraints:

# no points are outside the grid
y, x = Ints("y x")
solver.add(ForAll([y, x], Implies(Not(is_inside(y, x)), Not(occupied(y, x)))))


# there are `POINTS_TO_PLACE` distinct points, where `occupied` is True
xs = [Int(f"x{i}") for i in range(POINTS_TO_PLACE)]
ys = [Int(f"y{i}") for i in range(POINTS_TO_PLACE)]
positions = list(zip(ys, xs))

solver.add(Exists(xs + ys, And(
    distinct_pairs(positions),
    *[occupied(y, x) for y, x in positions]
)))


# for each row, there are no three distinct values x1,x2,x3, for which `occupied` is True
x1, x2, x3 = Ints("x1 x2 x3")
y = Int("y")
solver.add(ForAll([y],
                  Not(Exists([x1, x2, x3], And(
                      Distinct(x1, x2),
                      Distinct(x2, x3),
                      Distinct(x1, x3),
                      occupied(y, x1),
                      occupied(y, x2),
                      occupied(y, x3),
                  )))
                  ))


# for each column, there are no three distinct values y1,y2,y3, for which `occupied` is True
y1, y2, y3 = Ints("y1 y2 y3")
y = Int("x")
solver.add(ForAll([x],
                  Not(Exists([y1, y2, y3], And(
                      Distinct(y1, y2),
                      Distinct(y2, y3),
                      Distinct(y1, y3),
                      occupied(y1, x),
                      occupied(y2, x),
                      occupied(y3, x),
                  )))
                  ))

##############################################

result = solver.check()

if result == sat:
    model = solver.model()
    for y in range(GRID_SIZE):
        for x in range(GRID_SIZE):
            print("██", end="") if model.evaluate(
                occupied(y, x)) else print("[]", end="")
        print("")

else:
    print(result)
