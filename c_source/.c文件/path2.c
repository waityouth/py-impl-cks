#include "lvgl.h"
#include "stdio.h"
#include "string.h"

#define RECT_POINT_NUM 5
extern lv_obj_t *F2_screen;
extern lv_obj_t *path_line;
extern lv_obj_t *dir_label;

static const lv_point_t rect_points[RECT_POINT_NUM] = {
    {155, 130},  {155, 440},  {800, 440},  {800, 130},  {155, 130}
};

#define LINE1_POINT_NUM 2
static const lv_point_t line1_points[LINE1_POINT_NUM] = {
    {600, 130},  {600, 440}
};

#define LINE2_POINT_NUM 2
static const lv_point_t line2_points[LINE2_POINT_NUM] = {
    {155, 285},  {600, 285}
};

#define PATH_WIDTH 20
#define PATH_TOLERANCE (PATH_WIDTH / 2)

static lv_obj_t *coord_label;
static lv_obj_t *click_point;

static void create_all_path(void);
static void update_coordinate_display(int16_t x, int16_t y);
static bool on_path(int16_t x, int16_t y);
static void screen_click_callback(lv_event_t *e);

lv_obj_t *path_get_click_point_f2(void)
{
    return click_point;
}

void path_navigate_init_F2(void)
{
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;
    click_point = NULL;
    coord_label = NULL;
    create_all_path();
    update_coordinate_display(rect_points[0].x, rect_points[0].y);
    lv_obj_add_event_cb(F2_screen, screen_click_callback, LV_EVENT_CLICKED, NULL);
}

static void create_all_path(void)
{
    lv_obj_t *line_rect = lv_line_create(F2_screen);
    lv_line_set_points(line_rect, rect_points, RECT_POINT_NUM);
    lv_obj_set_style_line_color(line_rect, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_line_width(line_rect, PATH_WIDTH, 0);
    lv_obj_clear_flag(line_rect, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *line_ver = lv_line_create(F2_screen);
    lv_line_set_points(line_ver, line1_points, LINE1_POINT_NUM);
    lv_obj_set_style_line_color(line_ver, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_line_width(line_ver, PATH_WIDTH, 0);
    lv_obj_clear_flag(line_ver, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *line_hor = lv_line_create(F2_screen);
    lv_line_set_points(line_hor, line2_points, LINE2_POINT_NUM);
    lv_obj_set_style_line_color(line_hor, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_line_width(line_hor, PATH_WIDTH, 0);
    lv_obj_clear_flag(line_hor, LV_OBJ_FLAG_CLICKABLE);
}

static void update_coordinate_display(int16_t x, int16_t y)
{
    if (coord_label == NULL) {
        coord_label = lv_label_create(F2_screen);
        lv_obj_set_style_text_font(coord_label, LV_FONT_DEFAULT, 0);
        lv_obj_set_style_text_color(coord_label, lv_color_make(255, 255, 0), 0);
        lv_obj_set_pos(coord_label, 800, 20);
    }
    char buf[64];
    sprintf(buf, "X: %d  Y: %d", x, y);
    lv_label_set_text(coord_label, buf);
}

static bool on_path(int16_t x, int16_t y)
{
    // 上边
    if((y >= 130-PATH_TOLERANCE) && (y <= 130+PATH_TOLERANCE) && (x >=155) && (x <=800)) return true;
    // 右边
    if((x >= 800-PATH_TOLERANCE) && (x <= 800+PATH_TOLERANCE) && (y >=130) && (y <=440)) return true;
    // 下边
    if((y >= 440-PATH_TOLERANCE) && (y <= 440+PATH_TOLERANCE) && (x >=155) && (x <=800)) return true;
    // 左边
    if((x >= 155-PATH_TOLERANCE) && (x <= 155+PATH_TOLERANCE) && (y >=130) && (y <=440)) return true;
    // 中间竖线
    if((x >= 600-PATH_TOLERANCE) && (x <= 600+PATH_TOLERANCE) && (y >=130) && (y <=440)) return true;
    // 中间横线
    if((y >= 285-PATH_TOLERANCE) && (y <= 285+PATH_TOLERANCE) && (x >=155) && (x <=600)) return true;
    return false;
}

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
    click_point = lv_btn_create(F2_screen);
    lv_obj_set_size(click_point, 20, 20);
    lv_obj_set_pos(click_point, click_x - 10, click_y - 10);
    lv_obj_set_style_bg_color(click_point, lv_color_make(255, 0, 0), 0);
    lv_obj_set_style_bg_opa(click_point, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(click_point, 0, 0);
    lv_obj_clear_flag(click_point, LV_OBJ_FLAG_CLICKABLE);
    update_coordinate_display(click_x, click_y);
}
