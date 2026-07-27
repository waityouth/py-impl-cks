#include "lvgl.h"
#include "stdio.h"
#include "string.h"

#define RECT_POINT_NUM 5
extern lv_obj_t *F1_screen;
extern lv_obj_t *path_line;
extern lv_obj_t *dir_label;

// 绘制走廊矩形（四边）
static const lv_point_t rect_points[RECT_POINT_NUM] = {
    {135, 160},  {750, 160},  {750, 425},  {135, 425},  {135, 160}
};

// 中间竖线
#define LINE1_POINT_NUM 2
static const lv_point_t line1_points[LINE1_POINT_NUM] = {
    {320, 160},  {320, 425}
};

// 中间横线（右半部分）
#define LINE2_POINT_NUM 2
static const lv_point_t line2_points[LINE2_POINT_NUM] = {
    {320, 305},  {750, 305}
};

#define PATH_WIDTH 20
#define PATH_TOLERANCE (PATH_WIDTH / 2)  // 点击差错

static lv_obj_t *coord_label;   // 显示坐标的标签
static lv_obj_t *click_point;   // 点击后显示红点

static void create_all_path(void);
static void update_coordinate(int16_t x, int16_t y);
static bool on_path(int16_t x, int16_t y);
static void screen_click_callback(lv_event_t *e);

// 外部获取当前点击的红点对象
lv_obj_t *path_get_click_point(void)
{
    return click_point;
}

// 初始化路径（画线，绑定点击事件）
void path_navigate_init(void)
{
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;
    click_point = NULL;
    coord_label = NULL;
    create_all_path();
    update_coordinate(rect_points[0].x, rect_points[0].y);
    lv_obj_add_event_cb(F1_screen, screen_click_callback, LV_EVENT_CLICKED, NULL);
}

// 画出所有路径（灰色线条）
static void create_all_path(void)
{
    lv_obj_t *line_rect = lv_line_create(F1_screen);
    lv_line_set_points(line_rect, rect_points, RECT_POINT_NUM);
    lv_obj_set_style_line_color(line_rect, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_line_width(line_rect, PATH_WIDTH, 0);
    lv_obj_clear_flag(line_rect, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *line_ver = lv_line_create(F1_screen);
    lv_line_set_points(line_ver, line1_points, LINE1_POINT_NUM);
    lv_obj_set_style_line_color(line_ver, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_line_width(line_ver, PATH_WIDTH, 0);
    lv_obj_clear_flag(line_ver, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *line_hor = lv_line_create(F1_screen);
    lv_line_set_points(line_hor, line2_points, LINE2_POINT_NUM);
    lv_obj_set_style_line_color(line_hor, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_line_width(line_hor, PATH_WIDTH, 0);
    lv_obj_clear_flag(line_hor, LV_OBJ_FLAG_CLICKABLE);
}

// 更新屏幕上的坐标显示
static void update_coordinate(int16_t x, int16_t y)
{
    if (coord_label == NULL) {
        coord_label = lv_label_create(F1_screen);
        lv_obj_set_style_text_font(coord_label, LV_FONT_DEFAULT, 0);
        lv_obj_set_style_text_color(coord_label, lv_color_make(255, 255, 0), 0);
        lv_obj_set_pos(coord_label, 800, 20);
    }
    char buf[64];
    sprintf(buf, "X: %d  Y: %d", x, y);
    lv_label_set_text(coord_label, buf);
}

// 判断点击点是否在路径区域内
static bool on_path(int16_t x, int16_t y)
{
    // 上边
    if((y >= 160-PATH_TOLERANCE) && (y <= 160+PATH_TOLERANCE) && (x >=135) && (x <=750)) return true;
    // 右边
    if((x >= 750-PATH_TOLERANCE) && (x <= 750+PATH_TOLERANCE) && (y >=160) && (y <=425)) return true;
    // 下边
    if((y >= 425-PATH_TOLERANCE) && (y <= 425+PATH_TOLERANCE) && (x >=135) && (x <=750)) return true;
    // 左边
    if((x >= 135-PATH_TOLERANCE) && (x <= 135+PATH_TOLERANCE) && (y >=160) && (y <=425)) return true;
    // 中间竖线
    if((x >= 320-PATH_TOLERANCE) && (x <= 320+PATH_TOLERANCE) && (y >=160) && (y <=425)) return true;
    // 中间横线
    if((y >= 305-PATH_TOLERANCE) && (y <= 305+PATH_TOLERANCE) && (x >=320) && (x <=750)) return true;
    return false;
}

// 屏幕点击回调：在路径上点击时显示红点
static void screen_click_callback(lv_event_t *e)
{
    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t click_pos;
    lv_indev_get_point(indev, &click_pos);
    int16_t click_x = click_pos.x;
    int16_t click_y = click_pos.y;

    if(!on_path(click_x, click_y)) {
        return;
    }
    if(click_point != NULL) {
        lv_obj_del(click_point);
    }
click_point = lv_btn_create(F1_screen);
lv_obj_set_size(click_point, 20, 20);          //圆点20×20像素
lv_obj_set_pos(click_point, click_x - 10, click_y - 10); //坐标居中对齐点击位置
lv_obj_set_style_bg_color(click_point, lv_color_make(255, 0, 0), 0);
lv_obj_set_style_bg_opa(click_point, LV_OPA_COVER, 0); //完全不透明
lv_obj_set_style_border_width(click_point, 0, 0); //无边框
lv_obj_clear_flag(click_point, LV_OBJ_FLAG_CLICKABLE); //红点不能再次被点击
}
