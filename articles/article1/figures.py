import matplotlib.pyplot as plt
from matplotlib.patches import Arc
from mpl_toolkits.mplot3d import Axes3D, proj3d
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from matplotlib.patches import FancyArrowPatch
import matplotlib.colors as colors
from matplotlib.text import Annotation
import numpy as np
import math
import sys


class Arrow3D(FancyArrowPatch):
    def __init__(self, x, y, z, dx, dy, dz, *args, **kwargs):
        super().__init__((0,0), (0,0), *args, **kwargs)
        self._xyz = (x,y,z)
        self._dxdydz = (dx,dy,dz)

    def draw(self, renderer):
        x1,y1,z1 = self._xyz
        dx,dy,dz = self._dxdydz
        x2,y2,z2 = (x1+dx,y1+dy,z1+dz)

        xs, ys, zs = proj3d.proj_transform((x1,x2),(y1,y2),(z1,z2), renderer.M)
        self.set_positions((xs[0],ys[0]),(xs[1],ys[1]))
        super().draw(renderer)

class Annotation3D(Annotation):
    '''Annotate the point xyz with text s'''

    def __init__(self, s, xyz, *args, **kwargs):
        Annotation.__init__(self,s, xy=(0,0), *args, **kwargs)
        self._verts3d = xyz        

    def draw(self, renderer):
        xs3d, ys3d, zs3d = self._verts3d
        xs, ys, zs = proj3d.proj_transform(xs3d, ys3d, zs3d, renderer.M)
        self.xy=(xs,ys)
        Annotation.draw(self, renderer)

star_scale = 5

star = []
for i in range(5):
    angle = (i / 5) * math.pi * 2.0 + math.pi / 2
    star.append([math.cos(angle) * star_scale, math.sin(angle) * star_scale])

cube_vertices = [[-1, -1, -1], [1, -1, -1], [1, 1, -1], [-1, 1, -1],
                 [-1, -1, 1], [1, -1, 1], [1, 1, 1], [-1, 1, 1]]
cube_edges = [[0, 1], [1, 2], [2, 3], [3, 0],
              [4, 5], [5, 6], [6, 7], [7, 4],
              [0, 4], [1, 5], [2, 6], [3, 7]]


def dotProduct(a, b):
    return sum(x * y for x, y in zip(a, b))

def crossProduct(a, b):
    return [(a[1] * b[2]) - (a[2] * b[1]),
            (a[2] * b[0]) - (a[0] * b[2]),
            (a[0] * b[1]) - (a[1] * b[0])]

def normalized(v):
    l = math.sqrt(dotProduct(v, v))
    return [x / l for x in v]


def draw_star(ax, M: np.ndarray, alpha: float = 1):
    t_star = [np.dot(M, np.array(v + [1])) for v in star]
    t_star = [(v[0] / v[2], v[1] / v[2]) for v in t_star]
    x = []
    y = []
    for i in range(5):
        i1 = (i * 2) % 5
        i2 = (i1 + 2) % 5
        x.append(t_star[i1][0])
        x.append(t_star[i2][0])
        y.append(t_star[i1][1])
        y.append(t_star[i2][1])
    ax.plot(x, y, color='#0077FF', alpha=alpha)


def get_rotation_matrix(angles: tuple):
    cos_a, sin_a = math.cos(angles[0]), math.sin(angles[0])
    cos_b, sin_b = math.cos(angles[1]), math.sin(angles[1])
    cos_c, sin_c = math.cos(angles[2]), math.sin(angles[2])
    return np.array[[cos_a * cos_c - sin_a * cos_b * sin_c,
                     - cos_a * sin_c - sin_a * cos_b * cos_c,
                     sin_a * sin_b],
                    [sin_a * cos_c + cos_a * cos_b * sin_c,
                     - sin_a * sin_c + cos_a * cos_b * cos_c,
                     - cos_a * sin_b],
                    [sin_b * sin_c, sin_b * cos_c, cos_b]];


def create_plot_with_grid():
    fig, ax = plt.subplots(figsize=(5, 5))
    ax = fig.add_axes([0.06, 0.06, 0.88, 0.88])
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

def create_plot_with_grid_3d():
    fig = plt.figure(figsize=(8, 8))
    ax = fig.add_subplot(111, projection='3d')
    ax.margins(0.05)
    ax.set_xlim(-1, 4)
    ax.set_ylim(-1, 4)
    ax.set_zlim(-1, 4)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    major_ticks = [-1, 0, 4]
    minor_ticks = [x for x in range(-1, 4)]
    ax.set_xticks(major_ticks, minor=False)
    ax.set_yticks(major_ticks, minor=False)
    ax.set_zticks(major_ticks, minor=False)
    ax.set_xticks(minor_ticks, minor=True)
    ax.set_yticks(minor_ticks, minor=True)
    ax.set_zticks(minor_ticks, minor=True)

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
    ax.figure.savefig('./assets/Figure_1.png')
    

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
    ax.annotate(r'$O$', xy=(0, 0),
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24)

    ax.scatter([v_a[0], ], [v_a[1], ], 20, color='gray')
    ax.annotate(r'$A$', xy=v_a,
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24)

    ax.scatter([v_b[0], ], [v_b[1], ], 20, color='gray')
    ax.annotate(r'$B$', xy=v_b,
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24)

    ax.scatter([v_d[0], ], [v_d[1], ], 20, color='gray')
    ax.annotate(r'$C$', xy=v_d,
                xycoords='data', xytext=(0.0, 10.0), textcoords='offset points', fontsize=24)

    angle_dir = ((nv_a[0] + nv_b[0]) * 0.5, (nv_a[1] + nv_b[1]) * 0.5)
    angle_dir_length = math.sqrt(angle_dir[0] * angle_dir[0] + angle_dir[1] * angle_dir[1])
    angle_dir = (angle_dir[0] / angle_dir_length, angle_dir[1] / angle_dir_length)
    ax.annotate(r'$\alpha$', xy=(angle_dir[0] * 0.6, angle_dir[1] * 0.6),
                xycoords='data', xytext=(0.0, 0.0), textcoords='offset points', fontsize=18)

    ax.annotate(r'$\vec c$', xy=(v_d[0] * 0.5, v_d[1] * 0.5),
                xycoords='data', xytext=(0.0, 15.0), textcoords='offset points', fontsize=24, color='b')
    ax.arrow(0, 0, *v_d, head_width=0.3, head_length=0.3, fc='b', ec='b', zorder=20.0, length_includes_head=True)
    ax.figure.savefig('./assets/Figure_2.png')


def picture3():
    fig, ax = create_plot_with_grid()
    draw_star(ax, np.identity(3))
    ax.figure.savefig('./assets/Figure_3.png')

def picture4():
    alpha = 20 * math.pi / 180
    M = [[math.cos(alpha), - math.sin(alpha), 0],
         [math.sin(alpha), math.cos(alpha), 0],
         [0, 0, 1]]
    fig, ax = create_plot_with_grid()
    draw_star(ax, np.identity(3), 0.2)
    draw_star(ax, M, 1)
    ax.figure.savefig('./assets/Figure_4.png')

def picture5():
    alpha = 20 * math.pi / 180
    M1 = [[math.cos(alpha), - math.sin(alpha), 0],
          [math.sin(alpha), math.cos(alpha), 0],
          [0, 0, 1]]
    beta = 10 * math.pi / 180
    M2 = [[math.cos(beta), - math.sin(beta), 0],
          [math.sin(beta), math.cos(beta), 0],
          [0, 0, 1]]
    fig, ax = create_plot_with_grid()
    draw_star(ax, np.identity(3), 0.15)
    draw_star(ax, M1, 0.3)
    draw_star(ax, np.dot(M2, M1), 1.0)
    ax.figure.savefig('./assets/Figure_5.png')

def picture6():
    alpha = 30 * math.pi / 180
    R = [[math.cos(alpha), - math.sin(alpha), 0],
         [math.sin(alpha), math.cos(alpha), 0],
         [0, 0, 1]]
    S = [[1.5, 0, 0], 
         [0, 0.5, 0],
         [0, 0, 1]]

    fig, ax = create_plot_with_grid()
    draw_star(ax, np.identity(3), 0.075)
    draw_star(ax, R, 0.2)
    draw_star(ax, np.dot(S, R), 1.0)
    ax.figure.savefig('./assets/Figure_6.png')

def picture7():
    alpha = 30 * math.pi / 180
    R = [[math.cos(alpha), - math.sin(alpha), 0],
         [math.sin(alpha), math.cos(alpha), 0],
         [0, 0, 1]]
    S = [[1.5, 0, 0], 
         [0, 0.5, 0],
         [0, 0, 1]]

    fig, ax = create_plot_with_grid()
    draw_star(ax, np.identity(3), 0.075)
    draw_star(ax, S, 0.2)
    draw_star(ax, np.dot(R, S), 1.0)
    ax.figure.savefig('./assets/Figure_7.png')

def picture8():
    o = [0, 0, 0]
    v_a = [2, 0, 1]
    v_b = [0.5, 1.5, -0.5]
    v_c = crossProduct(v_a, v_b)
    v_d = [a + b for a, b in zip(v_a, v_b)]

    nv_a = normalized(v_a)
    nv_b = normalized(v_b)
    nv_c = normalized(v_c)

    fig, ax = create_plot_with_grid_3d()
    ax.plot(*[[a, b] for a, b in zip(v_a, v_d)], linestyle='--', c="gray", zorder=0)
    ax.plot(*[[a, b] for a, b in zip(v_b, v_d)], linestyle='--', c="gray", zorder=0)
    ax.plot(*[[a, b, c] for a, b, c in zip(np.multiply(nv_a, 0.3), 
                                           np.multiply(nv_a, 0.3) + np.multiply(nv_c, 0.3), 
                                           np.multiply(nv_c, 0.3))], linestyle='-', c="gray", zorder=0)
    ax.plot(*[[a, b, c] for a, b, c in zip(np.multiply(nv_b, 0.3), 
                                           np.multiply(nv_b, 0.3) + np.multiply(nv_c, 0.3), 
                                           np.multiply(nv_c, 0.3))], linestyle='-', c="gray", zorder=0)
    ax.add_artist(Arrow3D(0, 0, 0, *v_a, mutation_scale=20,
                  arrowstyle="-|>", fc='r', ec='r'))
    ax.add_artist(Arrow3D(0, 0, 0, *v_b, mutation_scale=20,
                  arrowstyle="-|>", fc='g', ec='g'))
    ax.add_artist(Arrow3D(0, 0, 0, *v_c, mutation_scale=15,
                  fc='b', ec='b'))

    tri = Poly3DCollection([o, v_a, v_d, o, v_d, v_b], color='gray', linewidths=0, alpha=0.3)
    ax.add_collection3d(tri)

    ax.scatter(*list(zip(o, v_a, v_b, v_d)),
                    s=20,
                    marker='o',
                    color='black')
    ax.add_artist(Annotation3D('O', o, 
                    xytext=(0, 20),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="lightyellow")))
    ax.add_artist(Annotation3D('A', v_a, 
                    xytext=(0, 10),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="lightyellow")))
    ax.add_artist(Annotation3D('B', v_b, 
                    xytext=(0, 15),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="lightyellow")))
    ax.add_artist(Annotation3D('D', v_d, 
                    xytext=(0, 10),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="lightyellow")))

    ax.add_artist(Annotation3D(r'$\vec{a}$', np.multiply(v_a, 0.5), 
                    xytext=(0, 10),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="r"))

    ax.add_artist(Annotation3D(r'$\vec{b}$', np.multiply(v_b, 0.5), 
                    xytext=(5, 10),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="g"))

    ax.add_artist(Annotation3D(r'$\vec{c}$', np.multiply(v_c, 0.5), 
                    xytext=(0, 10),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="b"))
    ax.view_init(elev=15., azim=60)
    ax.figure.savefig('./assets/Figure_8.png')


def picture9():
    v = np.array([-0.5, 0.3, 0.8])
    n = np.array(normalized([1, 0, 3]))
    angle = 140 * (math.pi / 180)

    dz = np.array(n) * dotProduct(n, v)
    dx = v - dz
    dy = np.array(crossProduct(n, dx))
    v_dx_dy = dx * math.cos(angle) + dy * math.sin(angle)
    v_ = v_dx_dy + dz

    axisX, axisY = normalized(dx), normalized(dy)

    fig, ax = create_plot_with_grid_3d()
    ax.set_xlim(-1, 1)
    ax.set_ylim(-1, 1)
    ax.set_zlim(0, 1)
    ax.add_artist(Arrow3D(0, 0, 0, *list(axisX), mutation_scale=20, arrowstyle="->", fc='gray', ec='gray'))
    ax.add_artist(Arrow3D(0, 0, 0, *list(axisY), mutation_scale=20, arrowstyle="->", fc='gray', ec='gray'))
    ax.add_artist(Arrow3D(0, 0, 0, *list(v), mutation_scale=15, fc='r', ec='r'))
    ax.add_artist(Arrow3D(0, 0, 0, *list(n), mutation_scale=20, arrowstyle="-|>", fc='b', ec='b'))
    ax.add_artist(Arrow3D(0, 0, 0, *list(v_), mutation_scale=15, fc='g', ec='g'))
    ax.add_artist(Arrow3D(0, 0, 0, *list(dx), mutation_scale=20, arrowstyle="->", fc='r', ec='r'))
    ax.add_artist(Arrow3D(0, 0, 0, *list(dy), mutation_scale=20, arrowstyle="->", fc='y', ec='y'))
    ax.add_artist(Arrow3D(0, 0, 0, *list(dz), mutation_scale=20, arrowstyle="->", fc='r', ec='g'))
    ax.add_artist(Arrow3D(0, 0, 0, *list(v_dx_dy), mutation_scale=20, arrowstyle="->", fc='g', ec='g'))

    ax.plot(*[[a, b] for a, b in zip(np.multiply(n, -10), np.multiply(n, 10))], linestyle="--", c="gray")
    ax.plot(*[[a, b] for a, b in zip(v, dz)], linestyle="--", c="r")
    ax.plot(*[[a, b] for a, b in zip(v, dx)], linestyle="--", c="r")
    ax.plot(*[[a, b] for a, b in zip(dx, dx + dy)], linestyle="--", c="r")
    ax.plot(*[[a, b] for a, b in zip(dy, dx + dy)], linestyle="--", c="y")
    ax.plot(*[[a, b] for a, b in zip(dz, v_)], linestyle="--", c="g")
    ax.plot(*[[a, b] for a, b in zip(v_dx_dy, v_)], linestyle="--", c="g")

    arc_verts = []
    for i in range(16):
        a = (i / 15) * angle
        t = (dx * math.cos(a) + dy * math.sin(a)) * 0.3
        arc_verts.append(t)
    ax.plot(*[[*l] for l in zip(*arc_verts)], linestyle="-", c="gray", zorder=0.0)

    ax.add_artist(Annotation3D(r'$\vec{n}$', np.multiply(n, 0.75), 
                    xytext=(0, 5),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="b"))

    ax.add_artist(Annotation3D(r'$\vec{v_\parallel}$', np.multiply(dz, 0.5), 
                    xytext=(0, 5),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white")))

    ax.add_artist(Annotation3D(r'$\vec{v_\perp}$', np.multiply(dx, 0.5), 
                    xytext=(0, 5),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="r"))

    ax.add_artist(Annotation3D(r'$\vec{n} \times \vec{v_\perp}$', np.multiply(dy, 0.5), 
                    xytext=(0, -5),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="y"))

    ax.add_artist(Annotation3D(r'$\vec{v}$', np.multiply(v, 0.5), 
                    xytext=(0, 5),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="r"))

    ax.add_artist(Annotation3D(r"$\vec{v'}$", np.multiply(v_, 0.5), 
                    xytext=(0, 5),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="g"))

    ax.add_artist(Annotation3D(r"$\vec{v_{\perp rot}}$", np.multiply(v_dx_dy, 0.5), 
                    xytext=(0, 7),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="g"))

    ax.add_artist(Annotation3D(r"$\alpha^{\circ}$", np.multiply(normalized(dx + v_dx_dy), 0.4), 
                    xytext=(0, -10),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white")))

    ax.add_artist(Annotation3D(r"$\vec{aX}$", np.multiply(axisX, 1.0), 
                    xytext=(0, 15),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="gray"))

    ax.add_artist(Annotation3D(r"$\vec{aY}$", np.multiply(axisY, 1.0), 
                    xytext=(5, 15),
                    textcoords='offset points',
                    bbox=dict(boxstyle="round", fc="white"), c="gray"))

    ax.view_init(elev=50, azim=-130)
    ax.figure.savefig('./assets/Figure_9.png')

def picture10():
    alpha = 30 * math.pi / 180
    R = [[math.cos(alpha), - math.sin(alpha), 0],
         [math.sin(alpha), math.cos(alpha), 0],
         [0, 0, 1]]
    S = [[0.5, 0, 0], 
         [0, 0.5, 0],
         [0, 0, 1]]
    T = [[1, 0, 6],
         [0, 1, 4],
         [0, 0, 1]]

    fig, ax = create_plot_with_grid()
    draw_star(ax, np.identity(3), 0.075)
    draw_star(ax, S, 0.2)
    draw_star(ax, np.dot(R, S), 0.4)
    draw_star(ax, np.dot(T, np.dot(R, S)), 1.0)
    ax.figure.savefig('./assets/Figure_10.png')

picture1()
picture2()
picture3()
picture4()
picture5()
picture6()
picture7()
picture8()
picture9()
picture10()
#plt.show()
sys.exit()
