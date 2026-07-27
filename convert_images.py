# -*- coding: utf-8 -*-
"""将 c_source 中的 .bin 图像文件转换为 PNG"""

import struct
import os
from PIL import Image

# LVGL color format enum values
LV_COLOR_FORMAT_RGB565 = 4    # 2 bytes/pixel
LV_COLOR_FORMAT_ARGB8565 = 5  # 3 bytes/pixel

# 已知尺寸的图片（从 C 代码和文件大小推算）
KNOWN_SIZES = {
    "F0_screen": (1024, 600),
    "stars": (290, 51),
    "emptystars": (290, 51),
    # 非标准尺寸商铺图片
    "Gucci": (440, 325),
    "Rolex": (440, 295),
    "Miu_Miu": (383, 330),
    "Sk2": (440, 249),
}

# 推测商铺图片尺寸：290,400 像素(RGB565) → 尝试常见分辨率
# 400*363=145200, 440*330=145200, 381*381≈145161
# 多数图片在 290404 字节附近，此处用 440×330 试试

BIN_DIR = os.path.join("c_source", ".bin文件")
OUT_DIR = os.path.join("static", "img")


def rgb565_to_rgb888(pixel):
    """RGB565 (16bit) -> (R, G, B)"""
    r = (pixel >> 11) & 0x1F
    g = (pixel >> 5) & 0x3F
    b = pixel & 0x1F
    return (
        (r << 3) | (r >> 2),
        (g << 2) | (g >> 4),
        (b << 3) | (b >> 2),
    )


def argb8565_to_rgba888(pixel_high, pixel_low):
    """ARGB8565 (24bit) -> (R, G, B, A)
    Layout: 3 bytes = [A8][R5+G6+B5]
    Actually LVGL stores as: byte0=A, byte1=R5|G3, byte2=G3|B5
    Or as a 24-bit uint: bits 23-16=A, 15-11=R, 10-5=G, 4-0=B
    """
    # Treat 3 bytes as little-endian 24-bit
    val = pixel_low | (pixel_high << 16)  # hmm, depends on layout
    val = pixel_high | (pixel_low << 8)
    # Actually let's read as 3 bytes: b0,b1,b2
    # LVGL ARGB8565: b0=A, b1=upper, b2=lower, or packed bits
    # Simplified: treat as RGB565 in low 16 bits + alpha in high 8
    a = (pixel_high >> 8) & 0xFF
    rgb16 = (pixel_high & 0xFF) | ((pixel_low & 0xFF) << 8)
    r = (rgb16 >> 11) & 0x1F
    g = (rgb16 >> 5) & 0x3F
    b = rgb16 & 0x1F
    return (
        (r << 3) | (r >> 2),
        (g << 2) | (g >> 4),
        (b << 3) | (b >> 2),
        a,
    )


def detect_size(filepath, raw_len):
    """根据像素总数推测图片尺寸"""
    basename = os.path.splitext(os.path.basename(filepath))[0]

    if basename in KNOWN_SIZES:
        return KNOWN_SIZES[basename]

    # 商铺图片：尝试常见宽度
    for w in [440, 400, 383, 381]:
        if raw_len % w == 0:
            h = raw_len // w
            if 50 < h < 600:
                return (w, h)

    # 回退：最接近正方形的尺寸
    import math
    side = int(math.sqrt(raw_len))
    for w in range(side, max(side - 100, 1), -1):
        if raw_len % w == 0:
            return (w, raw_len // w)

    # 最后尝试
    h = raw_len // side
    return (side, h) if h > 0 else (1, raw_len)


def convert_bin_to_png(bin_path, png_path):
    """将单个 .bin 文件转换为 PNG"""
    with open(bin_path, "rb") as f:
        data = f.read()

    if len(data) < 4:
        print(f"  SKIP {bin_path}: too small")
        return False

    cf = data[0]

    # 跳过 4 字节头部
    pixel_data = data[4:]

    if cf == LV_COLOR_FORMAT_RGB565:
        bytes_per_pixel = 2
        pixel_count = len(pixel_data) // 2
    elif cf == LV_COLOR_FORMAT_ARGB8565:
        bytes_per_pixel = 3
        pixel_count = len(pixel_data) // 3
    else:
        print(f"  SKIP {bin_path}: unknown cf={cf}")
        return False

    w, h = detect_size(bin_path, pixel_count)
    total = w * h

    if total != pixel_count:
        print(f"  WARN {bin_path}: guessed {w}x{h}={total}, actual pixels={pixel_count}")
        # 调整高度
        h = pixel_count // w
        if w * h != pixel_count:
            print(f"  FAIL {bin_path}: cannot determine dimensions (pixels={pixel_count})")
            return False

    img = Image.new("RGBA", (w, h))
    pixels = img.load()

    idx = 0
    for y in range(h):
        for x in range(w):
            if cf == LV_COLOR_FORMAT_RGB565:
                if idx + 1 >= len(pixel_data):
                    break
                val = struct.unpack_from("<H", pixel_data, idx)[0]
                r, g, b = rgb565_to_rgb888(val)
                pixels[x, y] = (r, g, b, 255)
                idx += 2
            else:  # ARGB8565
                if idx + 2 >= len(pixel_data):
                    break
                b0, b1, b2 = pixel_data[idx], pixel_data[idx+1], pixel_data[idx+2]
                # ARGB8565: 24 bits = A(8) | R(5)G(6)B(5)=16 bits as RGB565
                a = b0
                rgb16 = b1 | (b2 << 8)
                r = (rgb16 >> 11) & 0x1F
                g = (rgb16 >> 5) & 0x3F
                b_val = rgb16 & 0x1F
                pixels[x, y] = (
                    (r << 3) | (r >> 2),
                    (g << 2) | (g >> 4),
                    (b_val << 3) | (b_val >> 2),
                    a,
                )
                idx += 3

    os.makedirs(os.path.dirname(png_path), exist_ok=True)
    img.save(png_path)
    return True


def main():
    os.makedirs(OUT_DIR, exist_ok=True)

    files = sorted(os.listdir(BIN_DIR))
    success = 0
    fail = 0

    for fname in files:
        if not fname.endswith(".bin"):
            continue
        bin_path = os.path.join(BIN_DIR, fname)
        png_name = os.path.splitext(fname)[0] + ".png"
        png_path = os.path.join(OUT_DIR, png_name)

        result = convert_bin_to_png(bin_path, png_path)
        if result:
            print(f"  OK {fname} -> {png_name}")
            success += 1
        else:
            fail += 1

    print(f"\nDone: {success} converted, {fail} failed")


if __name__ == "__main__":
    main()
