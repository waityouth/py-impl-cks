# -*- coding: utf-8 -*-
"""
Dijkstra 最短路径寻路模块 —— 从 C 语言 find.c / find2.c 迁移
"""

import math

INF = 100000

# ==================== F1 层寻路 ====================

F1_BASE_NODES = [
    {"x": 135, "y": 160},
    {"x": 320, "y": 160},
    {"x": 750, "y": 160},
    {"x": 135, "y": 305},
    {"x": 320, "y": 305},
    {"x": 750, "y": 305},
    {"x": 135, "y": 425},
    {"x": 320, "y": 425},
    {"x": 750, "y": 425},
]

F1_CONN = [
    [0, 1, 0, 1, 0, 0, 0, 0, 0],
    [1, 0, 1, 0, 1, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 1, 0, 0, 0],
    [1, 0, 0, 0, 0, 0, 1, 0, 0],
    [0, 1, 0, 0, 0, 1, 0, 1, 0],
    [0, 0, 1, 0, 1, 0, 0, 0, 1],
    [0, 0, 0, 1, 0, 0, 0, 1, 0],
    [0, 0, 0, 0, 1, 0, 1, 0, 1],
    [0, 0, 0, 0, 0, 1, 0, 1, 0],
]

F1_VX = [135, 320, 750]
F1_HY = [160, 305, 425]

# 目标点坐标
F1_SERVE1 = {"x": 115, "y": 100}
F1_SERVE2 = {"x": 315, "y": 480}
F1_ESCAPE = {"x": 115, "y": 360}
F1_LIFT1 = {"x": 115, "y": 240}
F1_LIFT2 = {"x": 645, "y": 480}


def snap_f1(x, y):
    """吸附到最近的走廊节点"""
    best_x = min(F1_VX, key=lambda v: abs(x - v))
    best_y = min(F1_HY, key=lambda v: abs(y - v))
    dx = abs(x - best_x)
    dy = abs(y - best_y)
    if dx < dy:
        return (best_x, y)
    else:
        return (x, best_y)


def project_to_corridor_f1(tx, ty):
    """投影到走廊边界内"""
    px, py = tx, ty
    if tx < 135:
        px = 135
    elif tx > 750:
        px = 750
    if ty < 160:
        py = 160
    elif ty > 425:
        py = 425
    return (px, py)


def build_graph_f1(nodes):
    """构建邻接矩阵"""
    n = 11
    g = [[INF] * 11 for _ in range(11)]
    nodes[:] = F1_BASE_NODES[:] + [{"x": 0, "y": 0}, {"x": 0, "y": 0}]
    for i in range(9):
        for j in range(9):
            if F1_CONN[i][j]:
                w = abs(nodes[i]["x"] - nodes[j]["x"]) + abs(nodes[i]["y"] - nodes[j]["y"])
                g[i][j] = w
    return g


def connect_point_f1(nodes, g, idx):
    """将新节点连接到图中"""
    for i in range(idx):
        if nodes[i]["y"] == nodes[idx]["y"]:
            ok = True
            if nodes[i]["y"] == 305:
                if (nodes[i]["x"] < 320 < nodes[idx]["x"]) or (nodes[idx]["x"] < 320 < nodes[i]["x"]):
                    ok = False
            if ok:
                w = abs(nodes[i]["x"] - nodes[idx]["x"])
                g[i][idx] = g[idx][i] = w
        if nodes[i]["x"] == nodes[idx]["x"]:
            w = abs(nodes[i]["y"] - nodes[idx]["y"])
            g[i][idx] = g[idx][i] = w


def dijkstra(g, n, s):
    """Dijkstra 最短路径"""
    dist = [INF] * n
    vis = [False] * n
    prev = [-1] * n
    dist[s] = 0
    for _ in range(n):
        u = -1
        min_d = INF
        for j in range(n):
            if not vis[j] and dist[j] < min_d:
                min_d = dist[j]
                u = j
        if u == -1:
            break
        vis[u] = True
        for v in range(n):
            if g[u][v] < INF and dist[v] > dist[u] + g[u][v]:
                dist[v] = dist[u] + g[u][v]
                prev[v] = u
    return prev


def find_path_f1(sx, sy, tx, ty):
    """计算 F1 层路径，返回点列表和方向指引"""
    sx, sy = snap_f1(sx, sy)
    nodes = F1_BASE_NODES[:]
    g = build_graph_f1(nodes)
    px, py = project_to_corridor_f1(tx, ty)
    nodes[9] = {"x": sx, "y": sy}
    nodes[10] = {"x": px, "y": py}
    connect_point_f1(nodes, g, 9)
    connect_point_f1(nodes, g, 10)
    prev = dijkstra(g, 11, 9)

    path = []
    cur = 10
    while cur != -1:
        path.append(cur)
        cur = prev[cur]
    path.reverse()

    pts = [{"x": nodes[p]["x"], "y": nodes[p]["y"]} for p in path]
    pts.append({"x": tx, "y": ty})

    directions = gen_directions(pts)
    return pts, directions


def find_nearest_serve_f1(x, y):
    d1 = abs(x - F1_SERVE1["x"]) + abs(y - F1_SERVE1["y"])
    d2 = abs(x - F1_SERVE2["x"]) + abs(y - F1_SERVE2["y"])
    if d1 < d2:
        return F1_SERVE1, "The nearest one is serve1"
    return F1_SERVE2, "The nearest one is serve2"


def find_nearest_escape_f1(x, y):
    return F1_ESCAPE, "Go to escape exit"


def find_nearest_lift_f1(x, y):
    d1 = abs(x - F1_LIFT1["x"]) + abs(y - F1_LIFT1["y"])
    d2 = abs(x - F1_LIFT2["x"]) + abs(y - F1_LIFT2["y"])
    if d1 < d2:
        return F1_LIFT1, "The nearest one is lift1"
    return F1_LIFT2, "The nearest one is lift2"


# ==================== F2 层寻路 ====================

F2_BASE_NODES = [
    {"x": 155, "y": 130},
    {"x": 600, "y": 130},
    {"x": 800, "y": 130},
    {"x": 155, "y": 285},
    {"x": 600, "y": 285},
    {"x": 800, "y": 285},
    {"x": 155, "y": 440},
    {"x": 600, "y": 440},
    {"x": 800, "y": 440},
]

F2_CONN = [
    [0, 1, 0, 1, 0, 0, 0, 0, 0],
    [1, 0, 1, 0, 1, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 1, 0, 0, 0],
    [1, 0, 0, 0, 1, 0, 1, 0, 0],
    [0, 1, 0, 1, 0, 0, 0, 1, 0],
    [0, 1, 0, 0, 1, 0, 0, 0, 1],
    [0, 0, 1, 0, 0, 0, 0, 1, 0],
    [0, 0, 0, 0, 1, 0, 1, 0, 1],
    [0, 0, 0, 0, 0, 1, 0, 1, 0],
]

F2_VX = [155, 600, 800]
F2_HY = [130, 285, 440]

F2_WC1 = {"x": 130, "y": 190}
F2_WC2 = {"x": 820, "y": 310}
F2_ESCAPE1 = {"x": 130, "y": 420}
F2_ESCAPE2 = {"x": 820, "y": 190}
F2_LIFT1 = {"x": 130, "y": 310}
F2_LIFT2 = {"x": 820, "y": 420}


def snap_f2(x, y):
    best_x = min(F2_VX, key=lambda v: abs(x - v))
    best_y = min(F2_HY, key=lambda v: abs(y - v))
    dx = abs(x - best_x)
    dy = abs(y - best_y)
    if dx < dy:
        return (best_x, y)
    return (x, best_y)


def project_to_corridor_f2(tx, ty):
    px, py = tx, ty
    if tx < 155:
        px = 155
    elif tx > 800:
        px = 800
    if ty < 130:
        py = 130
    elif ty > 440:
        py = 440
    return (px, py)


def build_graph_f2(nodes):
    n = 11
    g = [[INF] * 11 for _ in range(11)]
    nodes[:] = F2_BASE_NODES[:] + [{"x": 0, "y": 0}, {"x": 0, "y": 0}]
    for i in range(9):
        for j in range(9):
            if F2_CONN[i][j]:
                w = abs(nodes[i]["x"] - nodes[j]["x"]) + abs(nodes[i]["y"] - nodes[j]["y"])
                g[i][j] = w
    return g


def connect_point_f2(nodes, g, idx):
    for i in range(idx):
        if nodes[i]["y"] == nodes[idx]["y"]:
            ok = True
            if nodes[i]["y"] == 285:
                if (nodes[i]["x"] < 600 < nodes[idx]["x"]) or (nodes[idx]["x"] < 600 < nodes[i]["x"]):
                    ok = False
            if ok:
                w = abs(nodes[i]["x"] - nodes[idx]["x"])
                g[i][idx] = g[idx][i] = w
        if nodes[i]["x"] == nodes[idx]["x"]:
            w = abs(nodes[i]["y"] - nodes[idx]["y"])
            g[i][idx] = g[idx][i] = w


def find_path_f2(sx, sy, tx, ty):
    """计算 F2 层路径"""
    sx, sy = snap_f2(sx, sy)
    nodes = F2_BASE_NODES[:]
    g = build_graph_f2(nodes)
    px, py = project_to_corridor_f2(tx, ty)
    nodes[9] = {"x": sx, "y": sy}
    nodes[10] = {"x": px, "y": py}
    connect_point_f2(nodes, g, 9)
    connect_point_f2(nodes, g, 10)
    prev = dijkstra(g, 11, 9)

    path = []
    cur = 10
    while cur != -1:
        path.append(cur)
        cur = prev[cur]
    path.reverse()

    pts = [{"x": nodes[p]["x"], "y": nodes[p]["y"]} for p in path]
    pts.append({"x": tx, "y": ty})

    directions = gen_directions(pts)
    return pts, directions


def find_nearest_wc_f2(x, y):
    d1 = abs(x - F2_WC1["x"]) + abs(y - F2_WC1["y"])
    d2 = abs(x - F2_WC2["x"]) + abs(y - F2_WC2["y"])
    if d1 < d2:
        return F2_WC1, "The nearest one is WC1"
    return F2_WC2, "The nearest one is WC2"


def find_nearest_escape_f2(x, y):
    d1 = abs(x - F2_ESCAPE1["x"]) + abs(y - F2_ESCAPE1["y"])
    d2 = abs(x - F2_ESCAPE2["x"]) + abs(y - F2_ESCAPE2["y"])
    if d1 < d2:
        return F2_ESCAPE1, "The nearest one is escape1"
    return F2_ESCAPE2, "The nearest one is escape2"


def find_nearest_lift_f2(x, y):
    d1 = abs(x - F2_LIFT1["x"]) + abs(y - F2_LIFT1["y"])
    d2 = abs(x - F2_LIFT2["x"]) + abs(y - F2_LIFT2["y"])
    if d1 < d2:
        return F2_LIFT1, "The nearest one is lift1"
    return F2_LIFT2, "The nearest one is lift2"


# ==================== 通用：方向指引生成 ====================

def gen_directions(pts):
    """生成从起点到终点的方向指引文本"""
    if len(pts) <= 1:
        return ""
    limit = len(pts) - 2 if len(pts) >= 3 else len(pts) - 1

    results = []
    now_dx = pts[1]["x"] - pts[0]["x"]
    now_dy = pts[1]["y"] - pts[0]["y"]

    for i in range(1, limit):
        ndx = pts[i + 1]["x"] - pts[i]["x"]
        ndy = pts[i + 1]["y"] - pts[i]["y"]
        same_dir = ((ndx > 0 and now_dx > 0) or (ndx < 0 and now_dx < 0) or
                    (ndy > 0 and now_dy > 0) or (ndy < 0 and now_dy < 0))
        if same_dir:
            now_dx += ndx
            now_dy += ndy
        else:
            if now_dx > 0:
                results.append(f"Go east {now_dx}")
            elif now_dx < 0:
                results.append(f"Go west {-now_dx}")
            elif now_dy > 0:
                results.append(f"Go south {now_dy}")
            else:
                results.append(f"Go north {-now_dy}")
            now_dx = ndx
            now_dy = ndy

    if now_dx > 0:
        results.append(f"Go east {now_dx}")
    elif now_dx < 0:
        results.append(f"Go west {-now_dx}")
    elif now_dy > 0:
        results.append(f"Go south {now_dy}")
    else:
        results.append(f"Go north {-now_dy}")

    return ", ".join(results)


# ==================== 走廊碰撞检测 ====================

def is_on_path_f1(x, y):
    """检测 F1 层的 (x, y) 是否在走廊上"""
    # Path width = 20, tolerance = 10
    T = 10
    # 上边
    if 160 - T <= y <= 160 + T and 135 <= x <= 750:
        return True
    # 右边
    if 750 - T <= x <= 750 + T and 160 <= y <= 425:
        return True
    # 下边
    if 425 - T <= y <= 425 + T and 135 <= x <= 750:
        return True
    # 左边
    if 135 - T <= x <= 135 + T and 160 <= y <= 425:
        return True
    # 中间竖线
    if 320 - T <= x <= 320 + T and 160 <= y <= 425:
        return True
    # 中间横线（右半段）
    if 305 - T <= y <= 305 + T and 320 <= x <= 750:
        return True
    return False


def is_on_path_f2(x, y):
    """检测 F2 层的 (x, y) 是否在走廊上"""
    T = 10
    if 130 - T <= y <= 130 + T and 155 <= x <= 800:
        return True
    if 800 - T <= x <= 800 + T and 130 <= y <= 440:
        return True
    if 440 - T <= y <= 440 + T and 155 <= x <= 800:
        return True
    if 155 - T <= x <= 155 + T and 130 <= y <= 440:
        return True
    if 600 - T <= x <= 600 + T and 130 <= y <= 440:
        return True
    if 285 - T <= y <= 285 + T and 155 <= x <= 600:
        return True
    return False
