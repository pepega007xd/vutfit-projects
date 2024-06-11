from z3 import Int, Or, Solver, sat, ForAll, And, Implies
import random

# dataset P08 from https://people.sc.fsu.edu/~jburkardt/datasets/knapsack_01/knapsack_01.html
NUMBER_OF_ITEMS = 24
BAG_MAXIMUM_WEIGHT = 6_404_180

costs = [825594, 1677009, 1676628, 1523970, 943972, 97426, 69666, 1296457, 1679693, 1902996, 1844992, 1049289,
         1252836, 1319836, 953277, 2067538, 675367, 853655, 1826027, 65731, 901489, 577243, 466257, 369261]
weights = [382745, 799601, 909247, 729069, 467902, 44328, 34610, 698150, 823460, 903959, 853665,
           551830, 610856, 670702, 488960, 951111, 323046, 446298, 931161, 31385, 496951, 264724, 224916, 169684]


def value_constraints(vars):
    return [Or(var == 0, var == 1) for var in vars]


def capacity_constraint(vars, weights, max_weight):
    return sum([var * weight for (var, weight) in zip(vars, weights)]) <= max_weight


def total_cost(vars, costs):
    return sum([var * cost for (var, cost) in zip(vars, costs)])


solver = Solver()

# variables with value 0 or 1, which enable/disable each item in collection
optimal_selection = [Int(f"x_{i}") for i in range(NUMBER_OF_ITEMS)]

# each item's weight and cost is multiplied by either 1 or 0
solver.add(And(*value_constraints(optimal_selection)))

# selected items fit into bag's maximum carrying capacity
solver.add(capacity_constraint(optimal_selection, weights, BAG_MAXIMUM_WEIGHT))

# any other selection of items
other_selection = [Int(f"y_{i}") for i in range(NUMBER_OF_ITEMS)]

# condition for optimal solution
solver.add(ForAll(other_selection, Implies(
    And(
        *value_constraints(other_selection),
        capacity_constraint(other_selection, weights, BAG_MAXIMUM_WEIGHT)
    ),
    total_cost(optimal_selection, costs) >= total_cost(other_selection, costs)
)
))

if solver.check() == sat:
    model = solver.model()
    print(model)

    total_cost = sum(model[Int(f"x_{i}")].as_long() * costs[i]
                     for i in range(NUMBER_OF_ITEMS))
    # expected value is 13549094
    print(f"maximum cost of bag: {total_cost}")

else:
    print("failed to solve")
