import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
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


def get_rotation_matrix(angles: tuple):
    cos_a, sin_a = math.cos(angles[0]), math.sin(angles[0])
    cos_b, sin_b = math.cos(angles[1]), math.sin(angles[1])
    cos_c, sin_c = math.cos(angles[2]), math.sin(angles[2])
    return np.array([[cos_a * cos_c - sin_a * cos_b * sin_c,
                      - cos_a * sin_c - sin_a * cos_b * cos_c,
                      sin_a * sin_b],
                     [sin_a * cos_c + cos_a * cos_b * sin_c,
                      - sin_a * sin_c + cos_a * cos_b * cos_c,
                      - cos_a * sin_b],
                     [sin_b * sin_c, sin_b * cos_c, cos_b]])

def create_plot_with_grid_3d():
    fig = plt.figure(figsize=(4, 4))
    ax = fig.add_subplot(111, projection='3d')
    ax.margins(0.05)
    ax.set_xlim(-2, 2)
    ax.set_ylim(0, 4)
    ax.set_zlim(-1, 3)
    ax.set_xlabel('X')
    ax.set_ylabel('Z')
    ax.set_zlabel('Y')
    ax.set_xticks([-2, 0, 2], minor=False)
    ax.set_yticks([0, 2, 4], minor=False)
    ax.set_zticks([-1, 1, 3], minor=False)
    ax.set_xticks([x for x in range(-2, 2)], minor=True)
    ax.set_yticks([x for x in range(0, 4)], minor=True)
    ax.set_zticks([x for x in range(-1, 3)], minor=True)

    return fig, ax


def draw_line(ax, p_a, p_b, *args, **kwargs):
    ax.plot(*[[a, b] for a, b in zip(p_a, p_b)], *args, **kwargs)


def figure1():
    fig, ax = plt.subplots(figsize=(6, 6))
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


def figure2():
    def swap_zy(v):
        return [v[0], v[2], v[1]]

    fig, ax = create_plot_with_grid_3d()
    ax.axis("off")

    cube_vertices = np.array([[-1, -1, -1], [1, -1, -1], [1, 1, -1], [-1, 1, -1],
                              [-1, -1, 1], [1, -1, 1], [1, 1, 1], [-1, 1, 1]])
    cube_edges = [(0, 1), (1, 2), (2, 3), (3, 0), 
                  (4, 5), (5, 6), (6, 7), (7, 4),
                  (0, 4), (1, 5), (2, 6), (3, 7)]
    R = get_rotation_matrix((0, 30 * (math.pi / 180), 10 * (math.pi / 180)))
    t = np.array([0, 0, 3])

    K = np.array([[1, 0, 0], 
                  [0, 1, 0],
                  [0, 0, 1]], dtype=np.float32)

    world_cube_vertices = [np.dot(R, v) + t for v in cube_vertices]
    projected_points = [v / v[2] for v in world_cube_vertices]

    world_cube_vertices = [swap_zy(v) for v in world_cube_vertices]
    projected_points = [swap_zy(v) for v in projected_points]

    # draw cube edges
    for e_a, e_b in cube_edges:
        draw_line(ax, world_cube_vertices[e_a], world_cube_vertices[e_b], linestyle='-', c="black", zorder=1, linewidth=1.0)

    # draw projection rays
    for world_vertex, projected_vertex in zip(world_cube_vertices, projected_points):
        draw_line(ax, world_vertex, projected_vertex, linestyle='--', c="gray", zorder=0, linewidth=0.4)

    # draw cube edges on projection
    for e_a, e_b in cube_edges:
        draw_line(ax, projected_points[e_a], projected_points[e_b], linestyle='-', c="gray", zorder=1, linewidth=0.5)

    # draw rays to center of projection
    for projected_vertex in projected_points:
        draw_line(ax, (0, 0, 0), projected_vertex, linestyle=':', c="gray", zorder=-1, linewidth=0.4)

    plane_tri = [(-2, -2, 1), (2, -2, 1), (-2, 2, 1), (2, -2, 1), (2, 2, 1), (-2, 2, 1)]
    plane_tri = [swap_zy(v) for v in plane_tri]
    tri = Poly3DCollection(plane_tri, color='gray', linewidths=0, alpha=0.3)
    ax.add_collection3d(tri)

    ax.view_init(elev=12., azim=20.)
    ax.figure.savefig('./assets/Figure_2.png', transparent = False, bbox_inches = 'tight', pad_inches = 0)


figure1()
figure2()
plt.show()
sys.exit()
