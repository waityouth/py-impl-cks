# -*- coding: utf-8 -*-
"""
SKP Mall Navigation System - Flask Web 版
从 C 语言 LVGL 课程设计完整复刻
"""

import json
from flask import Flask, render_template, request, jsonify

from data import STORES, CATEGORIES, FLOORS
from pathfinding import (
    find_path_f1, find_path_f2,
    find_nearest_serve_f1, find_nearest_escape_f1, find_nearest_lift_f1,
    find_nearest_wc_f2, find_nearest_escape_f2, find_nearest_lift_f2,
    is_on_path_f1, is_on_path_f2,
)

app = Flask(__name__)

# 运行时评论数据（深拷贝初始数据）
store_comments = {s["name"]: [dict(c) for c in s["comments"]] for s in STORES}


def get_store(name):
    for s in STORES:
        if s["name"] == name:
            return s
    return None


def get_store_rating(name):
    comments = store_comments.get(name, [])
    valid = [c for c in comments if c.get("name")]
    if not valid:
        return 0.0
    return sum(c["score"] for c in valid) / len(valid)


def fuzzy_match(store_name, input_str):
    """模糊匹配（忽略大小写，不要求连续）"""
    if not input_str:
        return True
    sl = store_name.lower()
    il = input_str.lower()
    i = 0
    for ch in sl:
        if i < len(il) and ch == il[i]:
            i += 1
    return i == len(il)


# ==================== 路由 ====================

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/stores")
def api_stores():
    floor = request.args.get("floor", "")
    category = request.args.get("category", "")
    result = []
    for s in STORES:
        if floor and s["floor"] != floor:
            continue
        if category and s["category"] != category:
            continue
        result.append({
            "name": s["name"],
            "desc": s["desc"],
            "floor": s["floor"],
            "category": s["category"],
            "btn_x": s["btn_x"],
            "btn_y": s["btn_y"],
            "btn_w": s["btn_w"],
            "btn_h": s["btn_h"],
            "x": s["x"],
            "y": s["y"],
            "rating": round(get_store_rating(s["name"]), 1),
        })
    return jsonify(result)


@app.route("/api/store/<name>")
def api_store_detail(name):
    s = get_store(name)
    if not s:
        return jsonify({"error": "not found"}), 404
    comments = store_comments.get(name, [])
    rating = get_store_rating(name)
    return jsonify({
        "name": s["name"],
        "desc": s["desc"],
        "floor": s["floor"],
        "category": s["category"],
        "x": s["x"],
        "y": s["y"],
        "rating": round(rating, 1),
        "comments": [c for c in comments if c.get("name")],
    })


@app.route("/api/search")
def api_search():
    q = request.args.get("q", "")
    results = []
    for s in STORES:
        if fuzzy_match(s["name"], q):
            results.append({
                "name": s["name"],
                "floor": s["floor"],
                "category": s["category"],
                "rating": round(get_store_rating(s["name"]), 1),
            })
    return jsonify(results)


@app.route("/api/categories")
def api_categories():
    return jsonify(CATEGORIES)


@app.route("/api/floors")
def api_floors():
    return jsonify(FLOORS)


@app.route("/api/comment", methods=["POST"])
def api_add_comment():
    data = request.json
    name = data.get("store_name", "")
    score = float(data.get("score", 0))
    comment_text = data.get("comment", "")
    if not name or not (0.5 <= score <= 5.0):
        return jsonify({"error": "invalid data"}), 400

    comments = store_comments.get(name, [])
    comments.insert(0, {"name": "user:", "score": score, "comment": comment_text})
    if len(comments) > 20:
        comments.pop()
    store_comments[name] = comments

    return jsonify({"success": True, "rating": round(get_store_rating(name), 1)})


@app.route("/api/pathfind")
def api_pathfind():
    floor = request.args.get("floor", "F1")
    try:
        sx = int(request.args.get("sx", 0))
        sy = int(request.args.get("sy", 0))
    except (ValueError, TypeError):
        return jsonify({"error": "invalid coordinates"}), 400

    target = request.args.get("target", "")

    if floor == "F1":
        if target == "serve":
            dest, msg = find_nearest_serve_f1(sx, sy)
        elif target == "escape":
            dest, msg = find_nearest_escape_f1(sx, sy)
        elif target == "lift":
            dest, msg = find_nearest_lift_f1(sx, sy)
        else:
            try:
                tx = int(request.args.get("tx", 0))
                ty = int(request.args.get("ty", 0))
            except (ValueError, TypeError):
                return jsonify({"error": "invalid target coordinates"}), 400
            dest = {"x": tx, "y": ty}
            msg = ""
            # Check that dest is a store's coordinates
            pass

        if target in ("serve", "escape", "lift"):
            pts, directions = find_path_f1(sx, sy, dest["x"], dest["y"])
        else:
            pts, directions = find_path_f1(sx, sy, dest["x"], dest["y"])
        return jsonify({
            "path": pts,
            "directions": directions,
            "message": msg,
        })

    elif floor == "F2":
        if target == "wc":
            dest, msg = find_nearest_wc_f2(sx, sy)
        elif target == "escape":
            dest, msg = find_nearest_escape_f2(sx, sy)
        elif target == "lift":
            dest, msg = find_nearest_lift_f2(sx, sy)
        else:
            try:
                tx = int(request.args.get("tx", 0))
                ty = int(request.args.get("ty", 0))
            except (ValueError, TypeError):
                return jsonify({"error": "invalid target coordinates"}), 400
            dest = {"x": tx, "y": ty}
            msg = ""

        pts, directions = find_path_f2(sx, sy, dest["x"], dest["y"])
        return jsonify({
            "path": pts,
            "directions": directions,
            "message": msg,
        })

    return jsonify({"error": "invalid floor"}), 400


@app.route("/api/check_path")
def api_check_path():
    floor = request.args.get("floor", "F1")
    try:
        x = int(request.args.get("x", 0))
        y = int(request.args.get("y", 0))
    except (ValueError, TypeError):
        return jsonify({"on_path": False})

    if floor == "F1":
        on = is_on_path_f1(x, y)
    elif floor == "F2":
        on = is_on_path_f2(x, y)
    else:
        on = False

    return jsonify({"on_path": on})


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=5000)
