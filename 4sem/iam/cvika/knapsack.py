from z3 import Int, Or, Solver, sat
import random

# dataset P08 from https://people.sc.fsu.edu/~jburkardt/datasets/knapsack_01/knapsack_01.html
NUMBER_OF_ITEMS = 24
TARGET_COST = 13_549_094
BAG_MAXIMUM_WEIGHT = 6_404_180

costs = [825594, 1677009, 1676628, 1523970, 943972, 97426, 69666, 1296457, 1679693, 1902996, 1844992, 1049289,
         1252836, 1319836, 953277, 2067538, 675367, 853655, 1826027, 65731, 901489, 577243, 466257, 369261]
weights = [382745, 799601, 909247, 729069, 467902, 44328, 34610, 698150, 823460, 903959, 853665,
           551830, 610856, 670702, 488960, 951111, 323046, 446298, 931161, 31385, 496951, 264724, 224916, 169684]

# variables with value 0 or 1, which enable/disable each item in collection
vars = [Int(f"x_{i}") for i in range(NUMBER_OF_ITEMS)]

# each item's weight and cost is multiplied by either 1 or 0
value_constraints = [Or(vars[i] == 0, vars[i] == 1)
                     for i in range(NUMBER_OF_ITEMS)]

# selected items fit into bag's maximum carrying capacity
bag_capacity_constraint = sum(
    [vars[i] * weights[i] for i in range(NUMBER_OF_ITEMS)]) <= BAG_MAXIMUM_WEIGHT

# selected items add up to targeted cost
cost_constraint = sum(
    [vars[i] * costs[i] for i in range(NUMBER_OF_ITEMS)]) == TARGET_COST

s = Solver()
constraint = value_constraints
constraint.append(bag_capacity_constraint)
constraint.append(cost_constraint)
s.add(constraint)

if s.check() == sat:
    print(s.model())

else:
    print("failed to solve")

# result (10 seconds on Merlin):
# [x_16 = 0,
#  x_9 = 1,
#  x_2 = 0,
#  x_20 = 0,
#  x_13 = 0,
#  x_6 = 0,
#  x_17 = 0,
#  x_10 = 1,
#  x_3 = 1,
#  x_21 = 1,
#  x_14 = 0,
#  x_7 = 0,
#  x_0 = 1,
#  x_18 = 0,
#  x_11 = 0,
#  x_4 = 1,
#  x_22 = 1,
#  x_15 = 1,
#  x_8 = 0,
#  x_1 = 1,
#  x_19 = 0,
#  x_12 = 1,
#  x_5 = 1,
#  x_23 = 1]
