import matplotlib.pyplot as plt
import numpy as np
import itertools
import sys
import math

def project(v: list or np.array):
    if math.fabs(v[-1]) < 1e-3:
        return [0 for _ in range(len(v) - 1)]
    if type(v) is list:
        return [x / v[-1] for x in v[:-1]]
    return v[:-1] / v[-1]

def figure1():
    fig, ax = plt.subplots(figsize=(5, 5))
    ax.set_xlim(-5, 5)
    ax.set_ylim(-5, 5)
    #ax.get_xaxis().set_visible(False)
    #ax.get_yaxis().set_visible(False)
    ax.axis('off')

    K = np.array([[3, 0, 0.5], 
                  [0, 3, 0.5],
                  [0, 0, 1]], dtype=np.float32)

    points_a, points_b = [], []
    for i in range(-1000, 1000):
       x = i * 5
       points_a.append(project(np.dot(K, [x, -10, 1])))
       points_b.append(project(np.dot(K, [x, -10, 1e5])))
    for p_a, p_b in zip(points_a, points_b):
       ax.plot([p_a[0], p_b[0]], [p_a[1], p_b[1]], linestyle="-", color="black", linewidth=0.1)

    points_a, points_b = [], []
    for i in range(0, 100):
        z = i * 5
        points_a.append(project(np.dot(K, [-1e3, -10, z])))
        points_b.append(project(np.dot(K, [1e3, -10, z])))
    for p_a, p_b in zip(points_a, points_b):
        ax.plot([p_a[0], p_b[0]], [p_a[1], p_b[1]], linestyle="-", color="black", linewidth=0.1)
    ax.figure.savefig('./assets/Figure_1.png', transparent = False, bbox_inches = 'tight', pad_inches = 0)

figure1()
#plt.show()
sys.exit()
