#include "drivers.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"

// 函数声明
int read_file_to_array(const char* filename, uint8_t* buffer, uint32_t max_size);
lv_obj_t *F0_screen;
lv_obj_t *rating_screen;
lv_obj_t *F1_screen;
lv_obj_t *F2_screen;
lv_obj_t *F3_screen;
lv_obj_t *detail_screen;
lv_obj_t *back_screen;
void detail();
void scores(lv_obj_t *display);
void comments(lv_obj_t *display);
void button();
void switch_button();
void find_button();
void find2_button();
void sort_button();
void create_btn_search();

// 返回栈相关（记录之前打开的屏幕，实现返回功能）
lv_obj_t *back_screen_records[20];
int back_counts = -1;
void back_init();
void back_pushing_switch(lv_obj_t *screen);
void back_clicked();

int main()
{
    sys_init();
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
    
    // 创建各个屏幕（空对象）
    lv_obj_t *display0 = lv_obj_create(NULL);
    lv_obj_t *display1 = lv_obj_create(NULL);
    lv_obj_t *display2 = lv_obj_create(NULL);
    lv_obj_t *display3 = lv_obj_create(NULL);
    lv_obj_t *display4 = lv_obj_create(NULL);
    lv_obj_t *display5 = lv_obj_create(NULL);
    F0_screen = display0;
    F1_screen = display1;
    F2_screen = display2;
    F3_screen = display3;
    rating_screen = display4;
    detail_screen = display5;
    
    // F0 层背景图
    lv_obj_t *F0_background = lv_img_create(F0_screen);
    lv_img_set_src(F0_background, "0:/F0_screen.bin");
    
    // 初始化各个模块
    find_button();
    scores(display4);
    comments(display4);
    
    extern void path_navigate_init(void);
    extern void path_navigate_init_F2(void);
    path_navigate_init();      // 初始化 F1 层路径导航
    path_navigate_init_F2();   // 初始化 F2 层路径导航
    
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
        
    button();          // 创建地图上的商铺按钮
    find_button();     // 创建找路按钮
    find2_button();    // F2 层找路按钮
    switch_button();   // 楼层切换按钮
    sort_button();     // 排序按钮
    create_btn_search(); // 搜索按钮
    
    lv_scr_load(display0);   // 启动时加载 F0 屏幕
    
    while(1)
    {
        delay_us(2000);
        lv_timer_handler();
    }
}

// 初始化返回栈
void back_init()
{
    back_counts = -1;
    for (int i = 0; i < 20; i++) {
        back_screen_records[i] = NULL;
    }
}

// 压栈并切换屏幕
void back_pushing_switch(lv_obj_t *screen)
{		
    if(back_counts < 20-1)
    {
        lv_obj_t *temp_screen = lv_scr_act();
        back_screen_records[++back_counts] = temp_screen;
    }
    if(screen != NULL)
    {
        lv_scr_load(screen);
    } else {
        lv_scr_load(F1_screen);
    }
}

// 返回上一屏幕
void back_clicked()
{
    if((back_counts >= 0 && back_counts < 20-1) && back_screen_records[back_counts])
    {
        lv_scr_load(back_screen_records[back_counts]);
        back_screen_records[back_counts] = NULL;
        back_counts--;
    } else {
        lv_scr_load(F1_screen);
    }
}
