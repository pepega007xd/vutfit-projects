from z3 import Int, Or, Solver, sat, ForAll, And, Implies
from itertools import batched

NUMBER_OF_EMPLOYEES = 9

days = ("monday", "tuesday", "wednesday",
        "thursday", "friday", "saturday", "sunday")
shifts = ("morning", "afternoon", "night")
positions = ("pos1", "pos2")


def all_distinct(vars):
    """
    helper predicate which says that all variables must be distinct
    from each other
    """
    constraints = []
    for i, var1 in enumerate(vars):
        for var2 in vars[i+1:]:
            constraints.append(var1 != var2)
    return And(*constraints)


def add_negative_constraint(condition, employee):
    """
    adds a contraint, in which `condition` specifies unwanted positions
    for an employee
    """
    global solver, vars

    unwanted_positions = list(filter(condition, vars))
    [solver.add(pos != employee) for pos in unwanted_positions]


def add_exclusive_constraint(condition, employee):
    """
    adds a contraint, in which `condition` specifies the only wanted
    positions for an employee
    """
    global solver, vars

    all_other_positions = list(filter(lambda pos: not condition(pos), vars))
    [solver.add(pos != employee) for pos in all_other_positions]


# each variable describes a single position in a single shift
# in a single day, its integer value is an employee number
vars = []
for day in days:
    for shift in shifts:
        for position in positions:
            vars.append(Int(f"{day}_{shift}_{position}"))

solver = Solver()

# each day/shift/position has a valid employee
[solver.add(And(var > 0, var <= NUMBER_OF_EMPLOYEES)) for var in vars]

# one employee has at most one shift per day
positions_by_days = batched(vars, 6)  # employee numbers split by days
[solver.add(all_distinct(day)) for day in positions_by_days]

################ example constraints ###############

# employee 1 does not want a specific day
add_negative_constraint(lambda pos: "friday" not in str(pos), 1)

# employee 2 does not want night shifts
add_negative_constraint(lambda pos: "night" not in str(pos), 2)

# employee 3 wants only the first position in a any shift
add_exclusive_constraint(lambda pos: "pos1" in str(pos), 3)

if solver.check() == sat:
    model = solver.model()

    for day in days:
        print(f"Day: {day}")
        for shift in shifts:
            print(f"  Shift: {shift}")
            for position in positions:
                employee = model[Int(f"{day}_{shift}_{position}")].as_long()
                print(f"    Employee nr. {employee} on position {position}")

else:
    print("failed to solve")
