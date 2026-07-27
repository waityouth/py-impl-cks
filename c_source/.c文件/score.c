#include "drivers.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lvgl.h"

// 星星图片的路径
#define EMPTY_STAR_PATH   "0:/emptystars.bin"
#define FULL_STAR_PATH    "0:/stars.bin"
// 星星控件的大小和位置
#define STAR_W      290
#define STAR_H      50
#define ASTAR       50
#define STAR_P_X    512
#define STAR_P_Y    100
// 评论框的大小和位置
#define COM_C_W 600
#define COM_C_L 400
#define COM_C_P_X 512
#define COM_C_P_Y 400
#define COM_W 600
#define COM_L 50
#define COM_MAX 20     // 最多存20条评论

// 函数声明
void contain_cb(lv_event_t *e);
void write_cb(lv_event_t *e);
void keyboard_cb(lv_event_t *e);
void detail_comments_cb(lv_event_t *e);
void back_cb(lv_event_t *e);
void refresh_comment_list();
void refresh_store_rating();
void back_init();
void back_pushing_switch(lv_obj_t *screen);
void back_clicked();

// 评论结构体
struct comment {
    char name[10];
    float score;
    char comment[20];
};

// 商铺信息结构体
struct store_inform
{
    char *name;
    char *desc;
    char *img_path;
    struct comment *comments;
    int center_x;
    int center_y;
    char *floor;
    char *category;
};

// 全局变量
lv_obj_t *keyboard;           // 软键盘
lv_obj_t *text;               // 输入框
lv_obj_t *comment_list_obj;   // 评论列表的容器
lv_obj_t *rating_text;        // 评分时显示的提示文字
lv_obj_t *store_rating_text;  // 商铺评分文字
lv_obj_t *emptystars;         // 评分页的空星星（可滑动）
lv_obj_t *store_emptystars;   // 商铺详情页的空星星
lv_obj_t *store_stars;        // 商铺详情页的实心星星（显示评分）
lv_obj_t *detail_emptystars;  // 详情页的空星星（可点击）
lv_obj_t *detail_stars = NULL;    // 详情页的实心星星
lv_obj_t *detail_rating = NULL;   // 详情页的评分文字
lv_obj_t *temp_screen=NULL;       // 临时屏幕变量
uint16_t commentnumber=0;     // 当前商铺的评论条数
float temp_score=0;           // 用户临时滑动选择的分数
float sum=0;                  // 总分累加
float store_rating;           // 商铺最终评分

// 外部变量，来自其他文件
extern lv_obj_t *F1_screen;
extern lv_obj_t *rating_screen;
extern lv_obj_t *back_screen;
extern lv_obj_t *detail_screen;
extern struct comment store_comment[20];
extern struct store_inform store_info;

// 初始化评分屏幕（评分页面）
void scores(lv_obj_t *display)
{		
    // 创建空星星图片，可点击滑动评分
    emptystars = lv_img_create(rating_screen);           
    lv_obj_add_flag(emptystars, LV_OBJ_FLAG_CLICKABLE);  
    lv_img_set_src(emptystars, EMPTY_STAR_PATH);
    lv_obj_set_pos(emptystars, STAR_P_X-STAR_W/2, STAR_P_Y-STAR_H/2);
    // 实心星星叠在空星星上面，用来显示滑动的进度
    lv_obj_t *stars = lv_img_create(emptystars);
    lv_obj_clear_flag(stars, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(stars, "0:/stars.bin");
    lv_obj_set_pos(stars, 0, 0);
    // 绑定滑动事件，滑动时改变实心星星宽度
    lv_obj_add_event_cb(emptystars, contain_cb, LV_EVENT_PRESSING, stars);
    lv_obj_move_foreground(stars);
    // 评分文字和商铺评分文字
    rating_text = lv_label_create(rating_screen);
    store_rating_text = lv_label_create(rating_screen);
    lv_obj_set_pos(rating_text, STAR_P_X-60, STAR_P_Y+30);
    lv_obj_set_pos(store_rating_text, STAR_P_X-60, STAR_P_Y+30);
    // 一开始隐藏评分区域（只有点击写评论才显示）
    lv_obj_add_flag(rating_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(emptystars, LV_OBJ_FLAG_HIDDEN);
    // 商铺详情页的星星（显示评分，可点击进入写评论）
    store_emptystars = lv_img_create(rating_screen);
    lv_img_set_src(store_emptystars, EMPTY_STAR_PATH);
    lv_obj_set_pos(store_emptystars, STAR_P_X-STAR_W/2, STAR_P_Y-STAR_H/2);
    lv_obj_add_flag(store_emptystars, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(store_emptystars, write_cb, LV_EVENT_CLICKED, NULL);
    store_stars = lv_img_create(store_emptystars);
    lv_obj_clear_flag(store_stars, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(store_stars, "0:/stars.bin");
    lv_obj_set_pos(store_stars, 0, 0);
    // 刷新商铺评分显示
    refresh_store_rating();
}

// 滑动评分时的回调函数
void contain_cb(lv_event_t *e)
{
    lv_obj_t *contain = lv_event_get_user_data(e);   // 实心星星控件
    lv_obj_t *empty_star = lv_event_get_target(e);   // 空星星控件

    lv_indev_t *indev = lv_indev_get_act();
    lv_point_t scr_point;
    lv_indev_get_point(indev, &scr_point);

    lv_area_t area;
    lv_obj_get_coords(empty_star, &area);
    int16_t x = scr_point.x - area.x1;   // 手指相对于空星星左边的偏移

    if (x < 0) x = 0;
    if (x > 290) x = 290;
		
    uint16_t width;
    // 根据x坐标决定分数和星星宽度（0.5一档）
    if (x <= 25)    {width = 25; temp_score = 0.5;}
    else if (x <= 60)  {width = 60; temp_score = 1.0;}
    else if (x <= 85)  {width = 85; temp_score = 1.5;}
    else if (x <= 120) {width = 120; temp_score = 2.0;}
    else if (x <= 145) {width = 145; temp_score = 2.5;}
    else if (x <= 180) {width = 180; temp_score = 3.0;}
    else if (x <= 205) {width = 205; temp_score = 3.5;}
    else if (x <= 240) {width = 240; temp_score = 4.0;}
    else if (x <= 265) {width = 265; temp_score = 4.5;}
    else {width = 290; temp_score = 5.0;}
    
    char buf[32];
    sprintf(buf, "your rating : %.1f", temp_score);
    lv_label_set_text(rating_text, buf);
    lv_obj_set_width(contain, width);   // 改变实心星星宽度，显示部分星星
}

// 评论页面（显示评论列表，加写评论按钮）
void comments(lv_obj_t *display)
{		
    // 创建输入框和键盘，一开始隐藏
    text = lv_textarea_create(rating_screen);
    keyboard = lv_keyboard_create(display);
    // 统计当前商铺的评论条数和总分
    for(int i=0; store_comment[i].name[0] != '\0'; i++)
    {	
        commentnumber++;
        sum += store_comment[i].score;
        store_rating = sum / commentnumber;
    }
    
    lv_keyboard_set_textarea(keyboard, text);
    lv_obj_add_event_cb(keyboard, keyboard_cb, LV_EVENT_ALL, keyboard);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(text, LV_OBJ_FLAG_HIDDEN);
    
    // 评论列表的外框
    lv_obj_t *contain = lv_obj_create(display);
    comment_list_obj = lv_obj_create(contain);
    lv_obj_set_size(comment_list_obj, 550, 270);
    refresh_comment_list();   // 刷新显示评论
    
    lv_obj_set_size(contain, COM_C_W, COM_C_L);
    lv_obj_set_pos(contain, COM_C_P_X-COM_C_W/2, COM_C_P_Y-COM_C_L/2);
    // 标题
    lv_obj_t *title = lv_label_create(contain);
    lv_obj_set_pos(title, 0, 0);
    lv_label_set_text(title, "Comments");
    // 写评论按钮
    lv_obj_t *write = lv_btn_create(contain);
    lv_obj_set_size(write, 80, 40);
    lv_obj_set_pos(write, 465, 0);
    lv_obj_t *writetext = lv_label_create(write);
    lv_label_set_text(writetext, "write");
    lv_obj_set_pos(writetext, 0, 0);
    lv_obj_set_pos(comment_list_obj, 0, 60);
    lv_obj_add_event_cb(write, write_cb, LV_EVENT_CLICKED, display);
    
    lv_obj_move_foreground(keyboard);
    
    // 返回按钮
    lv_obj_t *back = lv_btn_create(rating_screen);
    lv_obj_set_size(back, 80, 40);
    lv_obj_set_pos(back, 900, 0);
    lv_obj_t *back_text = lv_label_create(back);
    lv_label_set_text(back_text, "back");
    lv_obj_add_event_cb(back, back_cb, LV_EVENT_CLICKED, NULL);
}	

// 刷新评论列表（重新创建一遍）
void refresh_comment_list()
{
    lv_obj_clean(comment_list_obj);
    if (commentnumber == 0) {
        lv_obj_t *empty_label = lv_label_create(comment_list_obj);
        lv_label_set_text(empty_label, "No comments yet.");
        return;
    }

    for (int i = 0; i < commentnumber; i++) {
        lv_obj_t *temp = lv_obj_create(comment_list_obj);
        lv_obj_set_size(temp, 500, 60);
        lv_obj_set_pos(temp, 0, i * 60);
        lv_obj_set_flex_flow(temp, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(temp, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(temp, 5, 0);

        lv_obj_t *tname = lv_label_create(temp);
        lv_label_set_text(tname, store_comment[i].name);
        lv_obj_set_width(tname, 60);  

        lv_obj_t *tscore = lv_label_create(temp);
        char score_buf[8];
        sprintf(score_buf, "%.1f", store_comment[i].score);
        lv_label_set_text(tscore, score_buf);
        lv_obj_set_width(tscore, 40);
				
        lv_obj_t *tcom = lv_label_create(temp);
        lv_label_set_text(tcom, store_comment[i].comment);
        lv_obj_set_flex_grow(tcom, 1);   
        lv_label_set_long_mode(tcom, LV_LABEL_LONG_WRAP); 
    }
}

// 点击"write"按钮或者商铺星星时，进入写评论模式
void write_cb(lv_event_t *e)
{	
    temp_score = 0;
    lv_obj_clear_flag(rating_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(emptystars, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(store_emptystars, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(store_rating_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_t *display = lv_event_get_user_data(e);
    lv_obj_t *empty_star = lv_event_get_target(e);
    lv_label_set_text(rating_text, "Rate the sliding stars");
    // 把现有评论往后移动一位，腾出第一个位置给新评论
    for(int i = commentnumber; i > 0; i--)
    {
        store_comment[i] = store_comment[i-1];
    }
    lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(text, LV_OBJ_FLAG_HIDDEN);
}

// 键盘回调（按回车或者完成时触发）
void keyboard_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * kb = lv_event_get_target(e);

    if(code == LV_EVENT_READY)
    {
        // 保存评论文字
        strncpy(store_comment[0].comment, lv_textarea_get_text(text), sizeof(store_comment[0].comment)-1);
        store_comment[0].comment[sizeof(store_comment[0].comment)-1] = '\0';
        strcpy(store_comment[0].name, "user:");
        if(commentnumber < COM_MAX) commentnumber++;
        
        const char *input = lv_textarea_get_text(text);
        strncpy(store_comment[0].comment, input, sizeof(store_comment[0].comment) - 1);
        
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(text, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(rating_text, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(emptystars, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(store_emptystars, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(store_rating_text, LV_OBJ_FLAG_HIDDEN);
        
        store_comment[0].score = temp_score;
        // 同步到store_info的comments里（更新全局）
        for(int i=0; i<20; i++)
        {
            store_info.comments[i] = store_comment[i];
        }
        lv_textarea_set_text(text, "");
        refresh_comment_list();
        refresh_store_rating();
    }
}

// 返回按钮回调
void back_cb(lv_event_t *e)
{		
    back_clicked();   // 调用全局返回函数
}

// 刷新商铺评分（更新星星宽度和文字）
void refresh_store_rating()
{
    int count = 0;
    float total = 0.0f;
    for (int i = 0; i < COM_MAX && store_comment[i].name[0] != '\0'; i++) {
        total += store_comment[i].score;
        count++;
    }
    if (count > 0) 
    {
        store_rating = total / count;
        lv_coord_t width = (lv_coord_t)(290 * store_rating / 5.0f);
        lv_obj_set_width(store_stars, width);
        if (detail_stars != NULL) 
        {
            lv_obj_set_width(detail_stars, width);
        }
    } else 
    {
        lv_obj_set_width(store_stars, 0);
    }
    lv_obj_move_foreground(store_stars);
    char buf[32];
    sprintf(buf, "store rating : %.1f", store_rating);
    lv_label_set_text(store_rating_text, buf);

    if (detail_rating != NULL) {
        lv_label_set_text(detail_rating, buf);
    }
}

// 更新商铺显示（切换商铺时调用）
void update_store_display()
{
    commentnumber = 0;
    sum = 0.0f;
    for(int i = 0; i < COM_MAX && store_comment[i].name[0] != '\0'; i++) {
        commentnumber++;
        sum += store_comment[i].score;
    }
    if (commentnumber > 0)
        store_rating = sum / commentnumber;
    else
        store_rating = 0;

    refresh_comment_list();
    refresh_store_rating();
}

// 创建商铺详情页（每个商铺点进去看到的界面）
void detail()
{
    lv_obj_clear_flag(detail_screen, LV_OBJ_FLAG_SCROLLABLE);
    // 商铺图片
    lv_obj_t *store_img = lv_img_create(detail_screen);
    lv_img_set_src(store_img, store_info.img_path);
    lv_obj_set_pos(store_img, 40, 135);
    // 商铺名称
    lv_obj_t *store_name = lv_label_create(detail_screen);
    lv_label_set_text(store_name, store_info.name);
    lv_obj_set_pos(store_name, 520, 80);
    // 位置坐标
    lv_obj_t *store_coord = lv_label_create(detail_screen);
    char buff[64];
    if(store_info.center_x > 0){
        sprintf(buff, "Location: %d, %d", store_info.center_x, store_info.center_y);
    } else {
        sprintf(buff, "Location: Number: %d", store_info.center_y);
    }
    lv_label_set_text(store_coord, buff);
    lv_obj_set_pos(store_coord, 200, 100); 
    // 楼层
    lv_obj_t *store_floor = lv_label_create(detail_screen);
    lv_label_set_text(store_floor, store_info.floor);
    lv_obj_set_pos(store_floor, 240, 75); 
    // 描述框
    lv_obj_t *desc_contain = lv_obj_create(detail_screen);
    lv_obj_set_pos(desc_contain, 520, 230);
    lv_obj_set_size(desc_contain, 300, 250);
    lv_obj_t *desc = lv_label_create(desc_contain);
    lv_obj_set_width(desc, 250);
    lv_label_set_text(desc, store_info.desc);
    lv_label_set_long_mode(desc, LV_LABEL_LONG_WRAP);
    // 星星评分（可点击进入写评论）
    detail_emptystars = lv_img_create(detail_screen);
    lv_img_set_src(detail_emptystars, EMPTY_STAR_PATH);
    detail_stars = lv_img_create(detail_emptystars);
    lv_img_set_src(detail_stars, "0:/stars.bin");
    lv_obj_set_pos(detail_emptystars, 520, 130);
    detail_rating = lv_label_create(detail_screen);
    lv_obj_set_pos(detail_rating, 520, 190);
    lv_obj_add_flag(detail_emptystars, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(detail_emptystars, detail_comments_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(detail_stars, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(detail_stars, detail_comments_cb, LV_EVENT_CLICKED, NULL);
    // 返回按钮
    lv_obj_t *back = lv_btn_create(detail_screen);
    lv_obj_set_pos(back, 900, 0);
    lv_obj_set_size(back, 80, 40);
    lv_obj_t *back_text = lv_label_create(back);
    lv_label_set_text(back_text, "back");
    lv_obj_add_event_cb(back, back_cb, LV_EVENT_CLICKED, NULL);
    // 详细评论按钮（跳转到评论页面）
    lv_obj_t *detail_comments = lv_btn_create(detail_screen);
    lv_obj_set_pos(detail_comments, 520, 520);
    lv_obj_set_size(detail_comments, 300, 40);
    lv_obj_t *detail_comments_text = lv_label_create(detail_comments);
    lv_label_set_text(detail_comments_text, "detail_comments");
    lv_obj_add_event_cb(detail_comments, detail_comments_cb, LV_EVENT_CLICKED, NULL);
    
    refresh_store_rating();
}

// 点击详情页的评论按钮或星星，跳转到评论页面
void detail_comments_cb(lv_event_t *e)
{
    back_pushing_switch(detail_screen);   // 把当前详情页压入返回栈
    refresh_store_rating();
    update_store_display();
    lv_scr_load(rating_screen);
}
