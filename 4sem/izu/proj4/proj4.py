from math import sqrt

Point = tuple[float, float, float]

CENTERS: list[Point] = [(1, -7, 5), (1, -8, 6), (6, 9, 2)]

POINTS: list[Point] = [(1, -8, 6), (-2, 8, -5), (-5, -8, 0),
                       (-2, 0, -3), (6, 4, 9),
                       (-8, 0, -2), (1, -3, -1), (3, 0, 7),
                       (1, -7, 5), (6, 9, 2)]


def distance(p1: Point, p2: Point) -> float:
    return sqrt(
        (p2[0] - p1[0]) ** 2 +
        (p2[1] - p1[1]) ** 2 +
        (p2[2] - p1[2]) ** 2
    )


def partition(centers: list[Point]) -> list[set[Point]]:
    new_clusters: list[set[Point]] = [set() for _ in CENTERS]
    for point in POINTS:
        closest_center = min(centers, key=lambda a: distance(a, point))
        new_clusters[centers.index(closest_center)].add(point)
    return new_clusters


def get_center(cluster: set[Point]) -> Point:
    x, y, z = 0., 0., 0.
    for point in cluster:
        x += point[0]
        y += point[1]
        z += point[2]
    x /= len(cluster)
    y /= len(cluster)
    z /= len(cluster)
    return (x, y, z)


def get_centers(clusters: list[set[Point]]) -> list[Point]:
    return list(map(get_center, clusters))


def kmeans(old_centers: list[Point], iteration: int) -> list[Point]:
    old_clusters = partition(old_centers)

    new_centers = get_centers(old_clusters)
    new_clusters = partition(new_centers)

    print(f"Running k-means iteration {iteration}:")
    print(f"New cluster centers: {new_centers}")

    if old_clusters == new_clusters:
        print("Partitioning did not change, reached final result.")
        return old_centers
    else:
        print("Partitioning changed, new clusters: ")
        [print(cluster) for cluster in new_clusters]
        return kmeans(new_centers, iteration + 1)


if __name__ == "__main__":
    final_centers = kmeans(CENTERS, 0)
    print("\nFinal clusters:")
    [print(cluster) for cluster in partition(final_centers)]
