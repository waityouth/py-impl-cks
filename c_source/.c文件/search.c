#include "drivers.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lvgl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define STORE_COUNT 43      // 总共有43个商铺
#define RESULTS_COUNT 5     // 搜索列表一次最多显示5个结果

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

// 外部变量，来自其他文件
extern lv_obj_t *F0_screen;
extern lv_obj_t *F1_screen;
extern lv_obj_t *F2_screen;
extern lv_obj_t *F3_screen;
extern lv_obj_t *rating_screen;
extern lv_obj_t *back_screen;
extern lv_obj_t *detail_screen;
extern struct comment store_comment[20];
extern struct store_inform store_info;
extern struct store_inform *store_info_list[];   // 所有商铺的指针数组

// 搜索界面的控件
lv_obj_t *search_contain = NULL;        // 搜索窗口容器
lv_obj_t *is_store_list_contain = NULL; // 搜索结果列表容器
lv_obj_t *search_keyboard = NULL;       // 搜索用的键盘
lv_obj_t *btn_stores[RESULTS_COUNT]={NULL};   // 结果按钮数组
lv_obj_t *btn_text[RESULTS_COUNT]={NULL};     // 按钮上的文字标签

struct store_inform *is_list[STORE_COUNT];    // 用来临时存放筛选后的结果

// 函数声明
void search_keyboard_cb(lv_event_t *e);
void detail();
void update_store_display();
void btn_search_cb(lv_event_t *e);
void create_search();
void btn_back_cb(lv_event_t *e);
void text_cb(lv_event_t *e);
void btn_stores_cb(lv_event_t *e);
void is_list_contain_create();
void back_init();
void back_pushing_switch(lv_obj_t *screen);
void back_clicked();

// 三个数取最小值
int min3(int a, int b, int c) 
{
    int m = (a < b) ? a : b;
    return (m < c) ? m : c;
}

// 初始化 is_list 为所有商铺（默认显示全部）
void init_is_list()
{	
    int i=0;
    for( ; store_info_list[i]; i++)
    {
        is_list[i] = store_info_list[i];
    }
    is_list[i]=NULL;
}

// 创建一个带文本的按钮
lv_obj_t *search_btn_create(uint16_t pos_x, uint16_t pos_y, uint16_t size_x, uint16_t size_y, int color_usual, int color_pressed, lv_obj_t * display, char* text)
{
    lv_obj_t *btn = lv_btn_create(display);
    lv_obj_set_pos(btn, pos_x, pos_y);
    lv_obj_set_size(btn, size_x, size_y);
    lv_obj_set_style_bg_color(btn, lv_color_hex(color_usual), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(color_pressed), LV_STATE_PRESSED);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return btn;
}

// 在各层屏幕上创建搜索按钮（F0~F3）
void create_btn_search()
{
    lv_obj_t *btn_search0 = search_btn_create(950,475,50,50,0xA24BC7,0xA24BC7,F0_screen,"search");
    lv_obj_t *btn_search1 = search_btn_create(950,475,50,50,0xFF0000,0x00FF00,F1_screen,"search");
    lv_obj_t *btn_search2 = search_btn_create(950,475,50,50,0xFF0000,0x00FF00,F2_screen,"search");
    lv_obj_t *btn_search3 = search_btn_create(950,475,50,50,0xFF0000,0x00FF00,F3_screen,"search");
    // 绑定点击事件
    lv_obj_add_event_cb(btn_search0, btn_search_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_search1, btn_search_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_search2, btn_search_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_search3, btn_search_cb, LV_EVENT_CLICKED, NULL);
}

// 点击搜索按钮后，弹出搜索界面
void btn_search_cb(lv_event_t *e)
{
    // 如果已经有搜索窗口，先删掉再创建新的
    if(search_contain != NULL)
    {
        lv_obj_del(search_contain);
        search_contain = NULL;
        is_store_list_contain = NULL;
    }
    create_search();
}

// 创建搜索界面的内容
void create_search()
{		
    // 外层容器
    search_contain = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(search_contain, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(search_contain, 850, 500);
    lv_obj_set_pos(search_contain, (1024-850)/2, (600-500)/2);
    // 返回按钮
    lv_obj_t *btn_back = search_btn_create(730, 0, 80, 40, 0x0000FF, 0x0000FF, search_contain, "back");
    lv_obj_add_event_cb(btn_back, btn_back_cb, LV_EVENT_CLICKED, NULL);
    // 初始化临时列表（显示所有商铺）
    init_is_list();
    // 键盘和输入框
    search_keyboard = lv_keyboard_create(search_contain);
    lv_obj_t *text = lv_textarea_create(search_contain);
    lv_obj_add_flag(text, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(text, 650, 60);
    lv_keyboard_set_textarea(search_keyboard, text);
    // 结果列表容器（初始隐藏）
    is_store_list_contain = lv_obj_create(search_contain);
    lv_obj_set_size(is_store_list_contain, 800, 350);
    lv_obj_set_pos(is_store_list_contain, 0, 100);
    lv_obj_add_flag(is_store_list_contain, LV_OBJ_FLAG_HIDDEN);
    // 创建5个结果按钮，初始隐藏
    for(int i=0; i<RESULTS_COUNT; i++)
    {
        btn_stores[i] = lv_btn_create(is_store_list_contain);
        btn_text[i] = lv_label_create(btn_stores[i]);
        lv_obj_set_size(btn_stores[i], 750, 50);
        lv_obj_set_pos(btn_stores[i], 0, i * 60);
        lv_obj_add_flag(btn_stores[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_event_cb(btn_stores[i], btn_stores_cb, LV_EVENT_CLICKED, NULL);
    }
    // 事件绑定：点击输入框显示键盘，输入内容时筛选，键盘完成时隐藏键盘
    lv_obj_add_event_cb(text, text_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(text, search_keyboard_cb, LV_EVENT_VALUE_CHANGED, text);
    lv_obj_add_event_cb(search_keyboard, search_keyboard_cb, LV_EVENT_READY, text);
}

// 点击搜索窗口上的返回按钮，关闭搜索窗口
void btn_back_cb(lv_event_t *e)
{
    if (search_contain != NULL) {
        lv_obj_del(search_contain);
        search_contain = NULL;   
    }
}

// 模糊匹配函数：检查 store_name 是否包含 input 子串（忽略大小写）
bool if_strcpy(char *store_name, char *input)
{
    if (input == NULL || *input == '\0') return true;   // 空字符串匹配所有
    if (store_name == NULL) return false;
    int i = 0, j = 0;
    int store_len = strlen(store_name);
    int input_len = strlen(input);
    
    while (i < store_len && j < input_len)
    {
        // 转成小写比较
        char temp_store = (store_name[i] >= 'A' && store_name[i] <= 'Z') ? 
                          (store_name[i] + 32) : store_name[i];
        char temp_input = (input[j] >= 'A' && input[j] <= 'Z') ? 
                          (input[j] + 32) : input[j];
        
        if (temp_store == temp_input)
        {
            i++;  
            j++;
        }
        else
        {
            i++;  
        }
    }
    return j == input_len;   // 如果input的字符全部匹配到，返回true
}

// 根据商铺名字精确查找（用来从名字获取结构体指针）
struct store_inform* search_find_store_by_name(const char *name)
{
    for (int i = 0; store_info_list[i] != NULL ; i++) 
    {
        if (strcmp(store_info_list[i]->name, name) == 0) 
        {
            return store_info_list[i];
        }
    }
    return NULL;  		
}

// 临时输出：如果名字匹配输入，返回商铺指针，否则NULL
struct store_inform *temp_output(char *store_name, char *input)
{
    if(if_strcpy(store_name, input))
    {
        return search_find_store_by_name(store_name);
    }
    return NULL ;
}

// 根据输入字符串刷新 is_list（筛选出匹配的商铺）
void fresh_is_list(char *input)
{	
    struct store_inform *temp_list[STORE_COUNT]={NULL};
    int i=0, j=0, k=0;
    for(; store_info_list[i]; i++)
    {
        if(temp_output(store_info_list[i]->name, input) != NULL)
        {
            temp_list[j] = store_info_list[i];
            j++;
        }
    }
    for(; k<STORE_COUNT; k++)
    {
        is_list[k] = temp_list[k];
    }
}

// 更新搜索结果列表的显示（根据 is_list 更新按钮）
void is_list_contain_update()
{	
    if(is_store_list_contain != NULL)
    {
        lv_obj_clear_flag(is_store_list_contain, LV_OBJ_FLAG_HIDDEN);
    } else {
        return;
    }
    // 先把所有结果按钮隐藏
    for (int i = 0; i < RESULTS_COUNT; i++) {
        if (btn_stores[i] == NULL) continue;
        lv_obj_add_flag(btn_stores[i], LV_OBJ_FLAG_HIDDEN);
    }
    if(*is_list != NULL)
    {
        int visible = 0;
        for (int j = 0; j < STORE_COUNT && is_list[j] != NULL; j++) {
            if (visible >= RESULTS_COUNT) break;  
            int idx = visible;   
            lv_label_set_text(btn_text[idx], is_list[j]->name);
            lv_obj_set_user_data(btn_stores[idx], (void *)is_list[j]);
            lv_obj_clear_flag(btn_stores[idx], LV_OBJ_FLAG_HIDDEN);
            visible++;
        }
        if (visible > 0) {
            lv_obj_clear_flag(is_store_list_contain, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(is_store_list_contain, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

// 点击搜索结果按钮，进入商铺详情页
void btn_stores_cb(lv_event_t *e)
{		
    lv_obj_t *btn = lv_event_get_target(e);
    struct store_inform *store = (struct store_inform *)lv_obj_get_user_data(btn);
    if (store == NULL) return;
    store_info = *store;
    for (int i = 0; i < 20; i++) {
        store_comment[i] = store->comments[i];
    }
    lv_obj_clean(detail_screen);
    detail();                  
    update_store_display();    
    back_pushing_switch(detail_screen);
}

// 键盘或输入框的事件回调
void search_keyboard_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if(code == LV_EVENT_READY)
    {
        // 键盘完成，隐藏键盘
        lv_obj_add_flag(search_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
    else if(code == LV_EVENT_VALUE_CHANGED)
    {
        // 输入内容变化，重新筛选
        const char *input = lv_textarea_get_text(target);
        fresh_is_list((char*)input);
        is_list_contain_update();
        lv_obj_move_background(is_store_list_contain);
    }
}

// 点击输入框时，显示键盘
void text_cb(lv_event_t *e)
{
    lv_obj_clear_flag(search_keyboard, LV_OBJ_FLAG_HIDDEN);
}
