#include "lvgl.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>

// F2 层的目标点坐标
#define WC1_X 130
#define WC1_Y 190
#define WC2_X 820
#define WC2_Y 310
#define ESCAPE1_X 130
#define ESCAPE1_Y 420
#define ESCAPE2_X 820
#define ESCAPE2_Y 190
#define LIFT1_X 130
#define LIFT1_Y 310
#define LIFT2_X 820
#define LIFT2_Y 420

#define PATH_WIDTH 10
#define INF 100000

extern lv_obj_t *click_point;
lv_obj_t *path_line=NULL;
lv_obj_t *label=NULL;
lv_obj_t *dir_label=NULL;

typedef struct{
    int x,y;
}Node;

static Node base[9]={
    {155,130},{600,130},{800,130},
    {155,285},{600,285},{800,285},
    {155,440},{600,440},{800,440}
};


extern lv_obj_t *F2_screen; 
lv_obj_t *path_get_click_point_f2(void);

// 吸附到最近节点
static void snap(int *x,int *y)
{
    int vx[3]={155,600,800};
    int hy[3]={130,285,440};
    int dx=abs(*x-vx[0]),bestx=vx[0];
    for(int i=1;i<3;i++){
        int d=abs(*x-vx[i]);
        if(d<dx){dx=d;bestx=vx[i];}
    }
    int dy=abs(*y-hy[0]),besty=hy[0];
    for(int i=1;i<3;i++){
        int d=abs(*y-hy[i]);
        if(d<dy){dy=d;besty=hy[i];}
    }
    if(dx<dy) *x=bestx;
    else *y=besty;
}

// 投影到走廊内
static void project_to_corridor(int tx,int ty,int *px,int *py)
{
    *px = tx;
    *py = ty;
    if(tx < 155) *px = 155;
    else if(tx > 800) *px = 800;
    if(ty < 130) *py = 130;
    else if(ty > 440) *py = 440;
}

// 构建图
static void build_graph(Node node[],int g[11][11],int *n)
{
    *n=11;
    for(int i=0;i<11;i++)
        for(int j=0;j<11;j++)
            g[i][j]=INF;
    for(int i=0;i<9;i++) node[i]=base[i];
    int conn[9][9]={
        {0,1,0,1,0,0,0,0,0},
        {1,0,1,0,1,0,0,0,0},
        {0,1,0,0,0,1,0,0,0},
        {1,0,0,0,1,0,1,0,0},
        {0,1,0,1,0,0,0,1,0},
        {0,1,0,0,1,0,0,0,1},
        {0,0,1,0,0,0,0,1,0},
        {0,0,0,0,1,0,1,0,1},
        {0,0,0,0,0,1,0,1,0}
    };
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            if(conn[i][j]){
                int w=abs(node[i].x-node[j].x)+abs(node[i].y-node[j].y);
                g[i][j]=w;
            }
        }
    }
}

static void connect_point(Node node[],int g[11][11],int idx)
{
    for(int i=0;i<idx;i++){
        if(node[i].y==node[idx].y){
            int ok=1;
            if(node[i].y==285){
                if((node[i].x<600 && node[idx].x>600) ||
                   (node[i].x>600 && node[idx].x<600)){
                    ok=0;
                }
            }
            if(ok){
                int w=abs(node[i].x-node[idx].x);
                g[i][idx]=g[idx][i]=w;
            }
        }
        if(node[i].x==node[idx].x){
            int w=abs(node[i].y-node[idx].y);
            g[i][idx]=g[idx][i]=w;
        }
    }
}

static void dijkstra(int g[11][11],int n,int s,int prev[])
{
    int dist[11],vis[11]={0};
    for(int i=0;i<n;i++){
        dist[i]=INF;
        prev[i]=-1;
    }
    dist[s]=0;
    for(int i=0;i<n;i++){
        int u=-1,min=INF;
        for(int j=0;j<n;j++){
            if(!vis[j] && dist[j]<min){
                min=dist[j];
                u=j;
            }
        }
        if(u==-1) break;
        vis[u]=1;
        for(int v=0;v<n;v++){
            if(g[u][v]<INF && dist[v]>dist[u]+g[u][v]){
                dist[v]=dist[u]+g[u][v];
                prev[v]=u;
            }
        }
    }
}

static void gen_dir(lv_point_t *p,int n)
{
    if(!dir_label){
        dir_label=lv_label_create(F2_screen);
        lv_obj_align(dir_label,LV_ALIGN_TOP_MID,0,30);
    }
    char buf[256]="",tmp[64];
    if(n <= 1)
    {
        lv_label_set_text(dir_label,"");
        return;
    }
    int limit = (n>=3)? n-2 : n-1;

    int dx = p[1].x - p[0].x;
    int dy = p[1].y - p[0].y;
    int now_dx=dx,now_dy=dy;

    for(int i=1;i<limit;i++)
    {
        int ndx = p[i+1].x-p[i].x;
        int ndy = p[i+1].y-p[i].y;
        if((ndx>0&&now_dx>0)||(ndx<0&&now_dx<0)||(ndy>0&&now_dy>0)||(ndy<0&&now_dy<0))
        {
            now_dx += ndx;
            now_dy += ndy;
        }
        else
        {
            if(now_dx>0) sprintf(tmp,"Go east %d, ",now_dx);
            else if(now_dx<0) sprintf(tmp,"Go west %d, ",-now_dx);
            else if(now_dy>0) sprintf(tmp,"Go south %d, ",now_dy);
            else sprintf(tmp,"Go north %d, ",-now_dy);
            strcat(buf,tmp);
            now_dx=ndx;
            now_dy=ndy;
        }
    }
    if(now_dx>0) sprintf(tmp,"Go east %d, ",now_dx);
    else if(now_dx<0) sprintf(tmp,"Go west %d, ",-now_dx);
    else if(now_dy>0) sprintf(tmp,"Go south %d, ",now_dy);
    else sprintf(tmp,"Go north %d, ",-now_dy);
    strcat(buf,tmp);

    int l=strlen(buf);
    if(l>2) buf[l-2]=0;
    lv_label_set_text(dir_label,buf);
}

static void draw(int sx,int sy,int tx,int ty)
{
    if(path_line){
        lv_obj_del(path_line);
        path_line=NULL;
    }
    snap(&sx,&sy);
    Node node[11];
    int g[11][11],n;
    build_graph(node,g,&n);
    node[9]=(Node){sx,sy};
    int px,py;
    project_to_corridor(tx,ty,&px,&py);
    node[10]=(Node){px,py};
    connect_point(node,g,9);
    connect_point(node,g,10);
    int prev[11];
    dijkstra(g,n,9,prev);
    int path[20],cnt=0,cur=10;
    while(cur!=-1){
        path[cnt++]=cur;
        cur=prev[cur];
    }
    static lv_point_t pts[40];
    int k=0;
    for(int i=cnt-1;i>=0;i--){
        pts[k++] = (lv_point_t){node[path[i]].x,node[path[i]].y};
    }
    pts[k++] = (lv_point_t){tx,ty};
    path_line=lv_line_create(F2_screen);
    lv_line_set_points(path_line,pts,k);
    lv_obj_set_style_line_color(path_line,lv_color_make(255,0,0),0);
    lv_obj_set_style_line_width(path_line,PATH_WIDTH,0);
    gen_dir(pts,k);	
}

static void show(const char *t)
{
    if(!label){
        label=lv_label_create(F2_screen);
        lv_obj_align(label,LV_ALIGN_TOP_MID,0,5);
    }
    lv_label_set_text(label,t);
}

// 找最近卫生间
static void find_wc_f2(lv_event_t *e)
{
    lv_obj_t *pt=path_get_click_point_f2();
    if(!pt)return;
    int x=lv_obj_get_x(pt)+10;
    int y=lv_obj_get_y(pt)+10;
    int d1=abs(x-WC1_X)+abs(y-WC1_Y);
    int d2=abs(x-WC2_X)+abs(y-WC2_Y);
    if(d1<d2){
        show("The nearest one is WC1");
        draw(x,y,WC1_X,WC1_Y);
    }else{
        show("The nearest one is WC2");
        draw(x,y,WC2_X,WC2_Y);
    }
}

// 找最近出口
static void find_escape_f2(lv_event_t *e)
{
    lv_obj_t *pt=path_get_click_point_f2();
    if(!pt)return;
    int x=lv_obj_get_x(pt)+10;
    int y=lv_obj_get_y(pt)+10;
    int d1=abs(x-ESCAPE1_X)+abs(y-ESCAPE1_Y);
    int d2=abs(x-ESCAPE2_X)+abs(y-ESCAPE2_Y);
    if(d1<d2){
        show("The nearest one is escape1");
        draw(x,y,ESCAPE1_X,ESCAPE1_Y);
    }else{
        show("The nearest one is escape2");
        draw(x,y,ESCAPE2_X,ESCAPE2_Y);
    }
}

// 找最近电梯
static void find_lift_f2(lv_event_t *e)
{
    lv_obj_t *pt=path_get_click_point_f2();
    if(!pt)return;
    int x=lv_obj_get_x(pt)+10;
    int y=lv_obj_get_y(pt)+10;
    int d1=abs(x-LIFT1_X)+abs(y-LIFT1_Y);
    int d2=abs(x-LIFT2_X)+abs(y-LIFT2_Y);
    if(d1<d2){
        show("The nearest one is lift1");
        draw(x,y,LIFT1_X,LIFT1_Y);
    }else{
        show("The nearest one is lift2");
        draw(x,y,LIFT2_X,LIFT2_Y);
    }
}

// 绑定函数
void find_serve_f2_btn_bind(lv_obj_t *btn)
{
    lv_obj_add_event_cb(btn,find_wc_f2,LV_EVENT_CLICKED,NULL);
}
void find_escape_f2_btn_bind(lv_obj_t *btn)
{
    lv_obj_add_event_cb(btn,find_escape_f2,LV_EVENT_CLICKED,NULL);
}
void find_lift_f2_btn_bind(lv_obj_t *btn)
{
    lv_obj_add_event_cb(btn,find_lift_f2,LV_EVENT_CLICKED,NULL);
}
