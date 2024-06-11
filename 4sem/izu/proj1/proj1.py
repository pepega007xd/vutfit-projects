Z = -1
MAP: list[list[int]] = [
    [8, 8, 6, 7, Z, 9, 9, 9, 9, 9],
    [9, 9, 9, 9, Z, 9, 9, 9, 9, 9],
    [9, Z, Z, Z, Z, Z, Z, Z, 9, 7],
    [9, Z, 6, 2, Z, 8, 9, 9, 9, 6],
    [9, Z, 5, 4, Z, 3, 3, 3, 3, 3],
    [3, 3, 3, 3, 3, 9, 4, Z, 9, 7],
    [9, 9, 8, 8, Z, 9, 2, Z, 8, 9],
    [6, 6, 5, 7, Z, Z, Z, Z, 7, 8],
    [7, 7, 7, 5, Z, 8, 7, 8, 7, 9],
    [8, 8, 8, 8, Z, 9, 7, 7, 8, 9],
]

Point = tuple[int, int]
Step = tuple[Point, int, Point | None]

OPEN: list[Step] = [((3, 3), 0, None)]
CLOSED: list[Step] = []


def step_to_str(step: Step) -> str:
    # x goes before y WTF
    parent = "NULL" if step[2] is None else f"[{step[2][1]}, {step[2][0]}]"
    return f"([{step[0][1]}, {step[0][0]}], {step[1]}, {parent})"


start = (3, 3)
end = (6, 6)


def add_neighbors(origin: Step):
    global MAP, OPEN, CLOSED

    for y_offset in range(-1, 2):
        for x_offset in range(-1, 2):
            if x_offset == 0 and y_offset == 0:
                continue

            y = y_offset + origin[0][0]
            x = x_offset + origin[0][1]

            # skip already closed points
            if len(list(
                    filter(lambda point: point[0] == (y, x), CLOSED))) > 0:
                continue

            try:
                if y < 0 or x < 0:
                    raise IndexError()

                cost = MAP[y][x]
            except IndexError:
                # skip points outside of map
                continue

            # skip walls
            if cost == Z:
                continue

            try:
                find_idx = list(
                    map(lambda o: o[0] == (y, x), OPEN)).index(True)
                if OPEN[find_idx][1] > origin[1] + cost:
                    OPEN[find_idx] = ((y, x), origin[1] + cost, origin[0])
            except ValueError:
                OPEN.append(((y, x), origin[1] + cost, origin[0]))


i = 0
while True:
    print(f"Iteration {i}")
    i += 1
    print("open:\n" + ", ".join([step_to_str(o) for o in OPEN]))
    print("closed:\n" + ", ".join([step_to_str(o) for o in CLOSED]))
    print("")

    try:
        lowest_cost = sorted(OPEN, key=lambda x: x[1]).pop(0)
        index = OPEN.index(lowest_cost)
        OPEN.pop(index)
    except IndexError:
        print("not found")
        break

    if lowest_cost[0] == end:
        CLOSED.append(lowest_cost)
        break

    add_neighbors(lowest_cost)
    CLOSED.append(lowest_cost)

    # print(f"open {open}")
    # print(f"closed {closed}")


def backtrace(point: Point | None) -> list[Point | None]:
    global CLOSED
    global start

    previous = next(filter(lambda cl: cl[0] == point, CLOSED))[2]

    if previous == start:
        return [start]

    return backtrace(previous) + [previous]


# [print(c) for c in closed]
[print(c) for c in backtrace(end)]
