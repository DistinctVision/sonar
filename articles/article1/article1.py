import matplotlib.pyplot as plt
from matplotlib.patches import Arc
import numpy as np
import math
import sys

star_scale = 5

star = []
for i in range(5):
    angle = (i / 5) * math.pi * 2.0 + math.pi / 2
    star.append([math.cos(angle) * star_scale, math.sin(angle) * star_scale])


def draw_star(ax, M: np.ndarray):
    t_star = [np.dot(M, np.array(v)) for v in star]
    x = []
    y = []
    for i in range(5):
        i1 = (i * 2) % 5
        i2 = (i1 + 2) % 5
        x.append(t_star[i1][0])
        x.append(t_star[i2][0])
        y.append(t_star[i1][1])
        y.append(t_star[i2][1])
    ax.plot(x, y)


def create_plot_with_grid():
    fig, ax = plt.subplots(figsize=(7, 7))
    ax.set_xlim(-10, 10)
    ax.set_ylim(-10, 10)
    ax.minorticks_on()

    major_ticks = [x for x in range(-10, 11, 10)]
    minor_ticks = [x for x in range(-10, 11, 1)]

    ax.set_xticks(major_ticks, minor=False)
    ax.set_xticks(minor_ticks, minor=True)
    ax.set_yticks(major_ticks, minor=False)
    ax.set_yticks(minor_ticks, minor=True)

    ax.grid(which='major', color='#555555', linewidth=2.0)
    ax.grid(which='minor', color='#CCCCCC')

    return fig, ax


def picture1():
    fig, ax = create_plot_with_grid()
    ax.set_xlim(-2, 4)
    ax.set_ylim(-1, 5)
    ax.arrow(0, 0, 2.5, 4.0, head_width=0.5, head_length=0.5, fc='k', ec='k', zorder=10.0, length_includes_head=True)

    ax.plot([2.5, 2.5], [0.0, 4.0], linestyle="--", color="gray")
    ax.scatter([2.5, ], [0.0, ], 20, color='gray')
    ax.annotate('x', xy=(2.5, 0.0), xycoords='data', xytext=(2.5, 0.0), textcoords='offset points', fontsize=16)

    ax.plot([0.0, 2.5], [4.0, 4.0], linestyle="--", color="gray")
    ax.scatter([0, ], [4.0, ], 20, color='gray')
    ax.annotate('y', xy=(0.0, 4.0), xycoords='data', xytext=(0.0, 4.0), textcoords='offset points', fontsize=16)


def picture2():
    v_a = (5.0, 4.0)
    v_b = (5.0, 1.0)
    v_a_length = math.sqrt(v_a[0] * v_a[0] + v_a[1] * v_a[1])
    nv_a = (v_a[0] / v_a_length, v_a[1] / v_a_length)
    v_b_length = math.sqrt(v_b[0] * v_b[0] + v_b[1] * v_b[1])
    nv_b = (v_b[0] / v_b_length, v_b[1] / v_b_length)
    p = sum([a * b for a, b in zip(v_a, v_b)]) / (v_a_length * v_a_length)
    v_d = (v_a[0] * p, v_a[1] * p)
    v_d_length = math.sqrt(v_d[0] * v_d[0] + v_d[1] * v_d[1])
    nv_d = (v_d[0] / v_d_length, v_d[1] / v_d_length)
    fig, ax = create_plot_with_grid()
    ax.set_xlim(-1, 6)
    ax.set_ylim(-2, 5)

    # draw right angle
    v_bd = (v_d[0] - v_b[0], v_d[1] - v_b[1])
    v_bd_length = math.sqrt(v_bd[0] * v_bd[0] + v_bd[1] * v_bd[1])
    nv_bd = (v_bd[0] / v_bd_length, v_bd[1] / v_bd_length)
    ax.plot([v_d[0] - nv_a[0] * 0.4, v_d[0] - (nv_a[0] + nv_bd[0]) * 0.4, v_d[0] - nv_bd[0] * 0.4],
            [v_d[1] - nv_a[1] * 0.4, v_d[1] - (nv_a[1] + nv_bd[1]) * 0.4, v_d[1] - nv_bd[1] * 0.4],
            linestyle="-", color="gray")

    angle_v_a = math.atan2(v_a[1], v_a[0]) * (180 / math.pi)
    angle_v_b = math.atan2(v_b[1], v_b[0]) * (180 / math.pi)
    ax.add_artist(Arc((0, 0), 1.0, 1.0, 0.0, angle_v_b, angle_v_a))

    ax.annotate(r'$\vec a$', xy=(nv_a[0] * v_a_length * 0.5, nv_a[1] * v_a_length * 0.5),
                xycoords='data', xytext=(0.0, 15.0), textcoords='offset points', fontsize=24, c='r')
    ax.arrow(0, 0, *v_a, head_width=0.5, head_length=0.5, fc='r', ec='r', zorder=10.0, length_includes_head=True)

    ax.annotate(r'$\vec b$', xy=(nv_b[0] * v_b_length * 0.5, nv_b[1] * v_b_length * 0.5),
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24, c='g')
    ax.arrow(0, 0, *v_b, head_width=0.5, head_length=0.5, fc='g', ec='g', zorder=10.0, length_includes_head=True)

    ax.plot([v_d[0], v_b[0]], [v_d[1], v_b[1]], linestyle="--", color="gray")
    ax.scatter([v_d[0], ], [v_d[1], ], 20, color='gray')

    ax.scatter([0.0, ], [0.0, ], 20, color='gray')
    ax.annotate(r'$A$', xy=(0, 0),
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24)

    ax.scatter([v_a[0], ], [v_a[1], ], 20, color='gray')
    ax.annotate(r'$B$', xy=v_a,
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24)

    ax.scatter([v_b[0], ], [v_b[1], ], 20, color='gray')
    ax.annotate(r'$C$', xy=v_b,
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24)

    ax.scatter([v_d[0], ], [v_d[1], ], 20, color='gray')
    ax.annotate(r'$D$', xy=v_d,
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24)

    angle_dir = ((nv_a[0] + nv_b[0]) * 0.5, (nv_a[1] + nv_b[1]) * 0.5)
    angle_dir_length = math.sqrt(angle_dir[0] * angle_dir[0] + angle_dir[1] * angle_dir[1])
    angle_dir = (angle_dir[0] / angle_dir_length, angle_dir[1] / angle_dir_length)
    ax.annotate(r'$\alpha$', xy=(angle_dir[0] * 0.6, angle_dir[1] * 0.6),
                xycoords='data', xytext=(0.0, 0.0), textcoords='offset points', fontsize=18)

    ax.annotate(r'$\vec c$', xy=(v_d[0] * 0.5, v_d[1] * 0.5),
                xycoords='data', xytext=(0.0, 15.0), textcoords='offset points', fontsize=24, color='b')
    ax.arrow(0, 0, *v_d, head_width=0.3, head_length=0.3, fc='b', ec='b', zorder=20.0, length_includes_head=True)


def picture3():
    fig, ax = create_plot_with_grid()
    draw_star(ax, np.identity(2))


picture1()
picture2()
picture3()
plt.show()
sys.exit()
