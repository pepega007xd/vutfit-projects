#!/bin/env python

import matplotlib.pyplot as plt
import numpy as np

file = open("result.out", "r")
lines = file.read().split("\n")
numbers = []
data = []

for line in lines:
    if line == "":
        continue
    parts = line.split(",")
    numbers.append(float(parts[0].split(" ")[2]))
    data.append(float(parts[1].split(" ")[3]))

plt.plot(numbers, data)
plt.scatter(numbers, data)

plt.xlabel("Delay (minutes)")
plt.ylabel("Average time (minutes)")
plt.title("Average time based on delay")
plt.xticks(np.arange(0, max(numbers) + 1, 5)) 

plt.show()

file.close()
