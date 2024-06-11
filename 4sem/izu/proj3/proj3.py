import sys
from dataclasses import dataclass
import math

MD = True

ATTRIBUTES = {
    "teplota": ["nizka", "stredni", "vysoka"],
    "den": ["pracovni", "vikend", "svatek"],
    "obdobi": ["jaro", "leto", "podzim", "zima"],
    "vhkost": ["mala", "velka"],
}

CLASSES = [
    "Nizka",
    "Stredni",
    "Velka",
]


@dataclass
class Obj:
    id: int
    cls: str
    attrs: dict[str, str]


OBJECTS = [
    Obj(1, 'Velka', {'teplota': 'stredni', 'den': 'pracovni',
        'obdobi': 'podzim', 'vhkost': 'mala'}),
    Obj(2, 'Velka', {'teplota': 'vysoka', 'den': 'svatek',
        'obdobi': 'zima', 'vhkost': 'velka'}),
    Obj(3, 'Stredni', {'teplota': 'stredni',
        'den': 'svatek', 'obdobi': 'zima', 'vhkost': 'velka'}),
    Obj(4, 'Stredni', {'teplota': 'stredni',
        'den': 'pracovni', 'obdobi': 'zima', 'vhkost': 'velka'}),
    Obj(5, 'Nizka', {'teplota': 'nizka', 'den': 'vikend',
        'obdobi': 'podzim', 'vhkost': 'mala'}),
    Obj(6, 'Stredni', {'teplota': 'vysoka', 'den': 'svatek',
        'obdobi': 'jaro', 'vhkost': 'velka'}),
    Obj(7, 'Stredni', {'teplota': 'nizka', 'den': 'pracovni',
        'obdobi': 'podzim', 'vhkost': 'mala'}),
    Obj(8, 'Stredni', {'teplota': 'stredni',
        'den': 'svatek', 'obdobi': 'jaro', 'vhkost': 'velka'}),
    Obj(9, 'Stredni', {'teplota': 'stredni',
        'den': 'vikend', 'obdobi': 'jaro', 'vhkost': 'velka'}),
    Obj(10, 'Velka', {'teplota': 'stredni', 'den': 'pracovni',
        'obdobi': 'leto', 'vhkost': 'velka'}),
    Obj(11, 'Velka', {'teplota': 'stredni', 'den': 'svatek',
        'obdobi': 'jaro', 'vhkost': 'mala'}),
    Obj(12, 'Stredni', {'teplota': 'vysoka', 'den': 'svatek',
        'obdobi': 'podzim', 'vhkost': 'mala'}),
    Obj(13, 'Nizka', {'teplota': 'stredni', 'den': 'vikend',
        'obdobi': 'podzim', 'vhkost': 'mala'}),
    Obj(14, 'Nizka', {'teplota': 'vysoka', 'den': 'pracovni',
        'obdobi': 'podzim', 'vhkost': 'mala'}),
    Obj(15, 'Nizka', {'teplota': 'nizka', 'den': 'svatek',
        'obdobi': 'jaro', 'vhkost': 'velka'}),
]


def eprint(content: str):
    print(content, file=sys.stderr)


def entropy(objects: list[Obj]) -> float:
    if len(objects) == 0:
        return 0

    sum = 0.0
    for cls in CLASSES:
        prob = len(list(filter(lambda obj: obj.cls == cls, objects))
                   ) / len(objects)
        if prob == 0.0:
            continue  # avoid log2 of zero
        sum -= prob * math.log2(prob)

    return sum


def information_gain(objects: list[Obj], attr: str) -> float:
    entropy_attr = 0.0
    for attr_value in ATTRIBUTES[attr]:
        filtered_objs = list(
            filter(lambda obj: obj.attrs[attr] == attr_value, objects))
        entropy_attr += len(filtered_objs) * \
            entropy(filtered_objs) / len(objects)

    return entropy(objects) - entropy_attr


def pick_attribute(objects: list[Obj], attributes: list[str], id: str) -> str:
    attrs_with_gain: dict[str, float] = {attr: information_gain(
        objects, attr) for attr in attributes}

    best_attr = max(attrs_with_gain, key=attrs_with_gain.get)

    fields = "|".join(
        map(lambda a: f"{a[0]}={a[1]:.4f}", attrs_with_gain.items()))

    if MD:
        print(f"{id} [label=\"{best_attr}|{{{fields}}}\"]")
    else:
        print(f"{id} [shape=record, label=\"{best_attr}|{{{fields}}}\"]")

    return best_attr


static_id = 0


def get_id() -> str:
    global static_id
    static_id += 1
    return f"node{static_id}"


def id3(objects: list[Obj], attributes: list[str], id: str):
    nr_classes = len(set(map(lambda obj: obj.cls, objects)))

    if nr_classes == 1:
        if MD:
            print(f"{id} [label=\"{objects[0].cls}\"]")
        else:
            print(
                f"{id} [shape=box, style=rounded, label=\"{objects[0].cls}\"]")
        return

    if len(attributes) == 0:
        print("TODO: leaf - no attributes")
        return

    next_attr = pick_attribute(objects, attributes, id)

    for attr_value in ATTRIBUTES[next_attr]:
        filtered_objs = list(
            filter(lambda obj: obj.attrs[next_attr] == attr_value, objects))
        filtered_attrs = attributes.copy()
        filtered_attrs.remove(next_attr)

        child_id = get_id()
        obj_ids = ",".join(map(lambda obj: str(obj.id), filtered_objs))

        if len(filtered_objs) == 0:
            continue

        print(f"{id} -> {child_id} [label=\"{attr_value} {{{obj_ids}}}\"]")

        id3(filtered_objs, filtered_attrs, child_id)


if __name__ == "__main__":
    if not MD:
        print("digraph {")
    id3(OBJECTS, list(ATTRIBUTES.keys()), get_id())
    if not MD:
        print("}")
