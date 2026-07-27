// button.c - 地图上的按钮、商铺详情跳转、分类排序等功能
#include "drivers.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include <math.h>

// 评论结构体
struct comment{
    char name[10];
    float score;
    char comment[20];
};

// 商铺信息结构体
struct store_inform
{
    char *name;          // 商铺名字
    char *desc;          // 描述
    char *img_path;      // 图片路径
    struct comment *comments; // 评论数组指针
    int center_x;        // 地图上的x坐标
    int center_y;        // 地图上的y坐标
    char *floor;         // 所在楼层
    char *category;      // 分类
};

// 全局变量：当前显示哪个商铺的评论
struct comment store_comment[20];

// ================== 以下是各个商铺的评论数据（硬编码）==================
struct comment Louis_comments[20]= {
    {"lxx:",4,"good"},
    {"wty:",4.5,"nice"},
    {"user3:",4.0,"Classic bag"},
    {"user4:",4.5,"Luxurious feel"},
    {"user5:",5.0,"Iconic brand"}
};

struct comment Cartier_comments[20] = {
    {"user1:", 4.5, "Classic design"},
    {"user2:", 4.0, "Good service"},
    {"user3:",4.0,"Elegant jewelry"},
    {"user4:",4.5,"Perfect gift"},
    {"user5:",5.0,"Timeless love"}
};

struct comment Hermes_comments[20] = {
    {"user1:", 5.0, "Perfect quality"},
    {"user2:", 4.5, "Worth the wait"},
    {"user3:",5.0,"Exquisite scarf"},
    {"user4:",4.5,"Craftsmanship"},
    {"user5:",4.0,"Expensive but nice"}
};

struct comment Dior_comments[20] = {
    {"user1:", 4.0, "Elegant style"},
    {"user2:", 4.5, "Friendly staff"},
    {"user3:",4.5,"Lady Dior bag"},
    {"user4:",4.0,"Beautiful makeup"},
    {"user5:",5.0,"Sophisticated"}
};

struct comment Prada_comments[20] = {
    {"user1:", 3.5, "Modern look"},
    {"user2:", 4.0, "A bit pricey"},
    {"user3:",4.0,"Nylon classic"},
    {"user4:",3.5,"Simple elegance"},
    {"user5:",4.5,"Great quality"}
};

struct comment Chanel_comments[20] = {
    {"user1:", 4.5, "Timeless"},
    {"user2:", 5.0, "My favorite"},
    {"user3:",5.0,"No.5 forever"},
    {"user4:",4.5,"Chic tweed"},
    {"user5:",4.0,"Luxury must-have"}
};

struct comment YSL_comments[20] = {
    {"user1:", 4.0, "Stylish"},
    {"user2:", 3.5, "Not many choices"},
    {"user3:",4.5,"Kate bag love"},
    {"user4:",4.0,"Rock chic"},
    {"user5:",3.5,"Limited edition"}
};

struct comment Sk2_comments[20] = {
    {"user1:", 4.5, "Skin feels great"},
    {"user2:", 4.0, "Effective"},
    {"user3:",5.0,"Holy grail"},
    {"user4:",4.5,"Glowing skin"},
    {"user5:",4.0,"Worth the price"}
};

struct comment HR_comments[20] = {   // Helena Rubinstein
    {"user1:", 4.5, "Luxury skincare"},
    {"user2:", 5.0, "Magic cream"},
    {"user3:",5.0,"Prodigy serum"},
    {"user4:",4.5,"Anti-aging"},
    {"user5:",4.0,"Smooth texture"}
};

struct comment Rolex_comments[20] = {
    {"user1:", 5.0, "Iconic watch"},
    {"user2:", 4.5, "Professional"},
    {"user3:",5.0,"Submariner dream"},
    {"user4:",4.5,"Investment piece"},
    {"user5:",4.0,"Classic design"}
};

struct comment IWC_comments[20] = {
    {"user1:", 4.0, "Precise engineering"},
    {"user2:", 4.5, "Elegant"},
    {"user3:",4.5,"Pilot's choice"},
    {"user4:",4.0,"Clean dial"},
    {"user5:",5.0,"Superb craftsmanship"}
};

struct comment Puyi_comments[20] = {
    {"user1:", 4.0, "Good eye exam"},
    {"user2:", 3.5, "Limited frames"},
    {"user3:",4.5,"Professional"},
    {"user4:",4.0,"Nice glasses"},
    {"user5:",3.5,"A bit slow"}
};

struct comment Omega_comments[20] = {
    {"user1:", 4.5, "Bond's choice"},
    {"user2:", 4.0, "Nice collection"},
    {"user3:",4.5,"Speedmaster fan"},
    {"user4:",5.0,"Precision time"},
    {"user5:",4.0,"Good service"}
};

struct comment Costa1_comments[20] = {
    {"user1:", 3.5, "Ok coffee"},
    {"user2:", 4.0, "Relaxing spot"},
    {"user3:",4.0,"Cappuccino good"},
    {"user4:",3.5,"Average pastry"},
    {"user5:",4.5,"Cozy corner"}
};

struct comment PremiumSelected_comments[20] = {
    {"user1:", 4.5, "Exclusive items"},
    {"user2:", 5.0, "Top brands"},
    {"user3:",4.5,"Unique finds"},
    {"user4:",5.0,"Curated luxury"},
    {"user5:",4.0,"Expensive"}
};

struct comment Gucci_comments[20] = {
    {"user1:", 4.0, "Bold designs"},
    {"user2:", 4.5, "Great collection"},
    {"user3:",4.0,"GG belt classic"},
    {"user4:",4.5,"Vibrant colors"},
    {"user5:",3.5,"Sometimes too loud"}
};

struct comment MiuMiu_comments[20] = {
    {"user1:", 4.0, "Cute style"},
    {"user2:", 3.5, "Small selection"},
    {"user3:",4.5,"Playful designs"},
    {"user4:",4.0,"Feminine"},
    {"user5:",3.5,"Limited sizes"}
};

struct comment Tods_comments[20] = {
    {"user1:", 4.5, "Comfy shoes"},
    {"user2:", 4.0, "Classic"},
    {"user3:",5.0,"Driving shoes"},
    {"user4:",4.5,"Italian quality"},
    {"user5:",4.0,"Nice leather"}
};

struct comment PradaF2_comments[20] = {
    {"user1:", 4.0, "Nice layout"},
    {"user2:", 4.5, "Helpful SA"},
    {"user3:",4.0,"Good display"},
    {"user4:",4.5,"Wide range"},
    {"user5:",4.0,"Polite staff"}
};

struct comment SKPSelect1_comments[20] = {
    {"user1:", 4.0, "Curated picks"},
    {"user2:", 3.5, "Mixed brands"},
    {"user3:",4.5,"Designer selection"},
    {"user4:",4.0,"Trendy"},
    {"user5:",3.5,"Overpriced"}
};

struct comment Ecco1_comments[20] = {
    {"user1:", 4.5, "Very comfortable"},
    {"user2:", 4.0, "Good quality"},
    {"user3:",4.5,"Walking shoes"},
    {"user4:",4.0,"Durable"},
    {"user5:",5.0,"Love the fit"}
};

struct comment CarbonStar_comments[20] = {
    {"user1:", 4.0, "Unique concept"},
    {"user2:", 4.5, "Sustainable"},
    {"user3:",4.5,"Eco-friendly"},
    {"user4:",4.0,"Modern design"},
    {"user5:",5.0,"Innovative"}
};

struct comment Ecco2_comments[20] = {
    {"user1:", 4.0, "Same as other"},
    {"user2:", 4.5, "Lovely"},
    {"user3:",4.0,"Comfortable soles"},
    {"user4:",4.5,"Nice casual"},
    {"user5:",4.0,"Reasonable price"}
};

struct comment CostaF2_comments[20] = {
    {"user1:", 4.0, "Better latte"},
    {"user2:", 3.5, "Crowded"},
    {"user3:",4.0,"Mocha nice"},
    {"user4:",3.5,"Noisy"},
    {"user5:",4.5,"Great coffee art"}
};

struct comment SKPSelect2_comments[20] = {
    {"user1:", 4.0, "Good selection"},
    {"user2:", 4.5, "Worth visiting"},
    {"user3:",4.0,"Many brands"},
    {"user4:",4.5,"Friendly"},
    {"user5:",3.5,"A bit messy"}
};

struct comment SKPSelectMain_comments[20] = {
    {"user1:", 4.5, "Flagship experience"},
    {"user2:", 5.0, "Amazing"},
    {"user3:",5.0,"Top service"},
    {"user4:",4.5,"Luxury vibe"},
    {"user5:",4.0,"Crowded weekend"}
};

struct comment Coach_comments[20] = {
    {"user1:", 4.0, "Value for money"},
    {"user2:", 3.5, "Casual"},
    {"user3:",4.5,"Signature bags"},
    {"user4:",4.0,"Good gifts"},
    {"user5:",3.5,"Outlet better"}
};

struct comment Boss_comments[20] = {
    {"user1:", 4.0, "Professional wear"},
    {"user2:", 4.5, "Sharp suits"},
    {"user3:",4.0,"Perfect fit"},
    {"user4:",4.5,"Modern business"},
    {"user5:",4.0,"Quality fabric"}
};

struct comment SKPSelectBottom1_comments[20] = {
    {"user1:", 4.0, "More casual"},
    {"user2:", 4.5, "Great deals"},
    {"user3:",4.0,"Streetwear"},
    {"user4:",4.5,"Good prices"},
    {"user5:",3.5,"Limited stock"}
};

struct comment SKPSelectBottom2_comments[20] = {
    {"user1:", 4.0, "Nice area"},
    {"user2:", 3.5, "Similar items"},
    {"user3:",4.0,"Cool vibes"},
    {"user4:",3.5,"Repeated brands"},
    {"user5:",4.5,"Hidden gems"}
};

struct comment Samsung_comments[20] = {
    {"user1:", 4.0, "Great electronics"},
    {"user2:", 4.5, "Latest models"},
    {"user3:",4.5,"Galaxy fan"},
    {"user4:",4.0,"Good display"},
    {"user5:",5.0,"Innovative tech"}
};

struct comment Ecco_comments[20] = {
    {"user1:", 4.5, "Very comfortable"},
    {"user2:", 4.0, "Good quality"},
    {"user3:",5.0,"Best shoes"},
    {"user4:",4.5,"Soft leather"},
    {"user5:",4.0,"Great everyday"}
};

struct comment Philips_comments[20] = {
    {"user1:", 4.0, "Innovative products"},
    {"user2:", 3.5, "Good value"},
    {"user3:",4.5,"Reliable brand"},
    {"user4:",4.0,"Good shaver"},
    {"user5:",3.5,"Basic design"}
};

struct comment BeijingChufang_comments[20] = {
    {"user1:", 4.5, "Authentic duck"},
    {"user2:", 4.0, "Nice atmosphere"},
    {"user3:",5.0,"Crispy skin"},
    {"user4:",4.5,"Flavorful"},
    {"user5:",4.0,"Worth queuing"}
};

struct comment ChuLingLong_comments[20] = {
    {"user1:", 4.0, "Delicate dim sum"},
    {"user2:", 4.5, "Creative dishes"},
    {"user3:",4.5,"Soup dumplings"},
    {"user4:",4.0,"Artistic plating"},
    {"user5:",5.0,"Must try"}
};

struct comment ShangHeChuan_comments[20] = {
    {"user1:", 4.5, "Spicy and tasty"},
    {"user2:", 4.0, "Good service"},
    {"user3:",4.5,"Sichuan authentic"},
    {"user4:",4.0,"Numbing flavor"},
    {"user5:",5.0,"Boiled fish"}
};

struct comment YiZuoYiWang_comments[20] = {
    {"user1:", 4.0, "Modern fusion"},
    {"user2:", 3.5, "A bit noisy"},
    {"user3:",4.5,"Creative menu"},
    {"user4:",4.0,"Trendy spot"},
    {"user5:",3.5,"Portion small"}
};

struct comment YanYu_comments[20] = {
    {"user1:", 4.5, "Elegant decor"},
    {"user2:", 5.0, "Excellent seafood"},
    {"user3:",5.0,"Lobster fresh"},
    {"user4:",4.5,"Romantic"},
    {"user5:",4.0,"Pricey but good"}
};

struct comment WangPinNiuPai_comments[20] = {
    {"user1:", 4.0, "Juicy steak"},
    {"user2:", 4.5, "Worth the price"},
    {"user3:",5.0,"Melt in mouth"},
    {"user4:",4.5,"Set menu great"},
    {"user5:",4.0,"Nice ambiance"}
};

struct comment ZhenBaoHaiXian_comments[20] = {
    {"user1:", 4.5, "Fresh oysters"},
    {"user2:", 4.0, "Great variety"},
    {"user3:",5.0,"Lobster tank"},
    {"user4:",4.5,"Seafood platter"},
    {"user5:",4.0,"Good value"}
};

struct comment ChuXiangFuRong_comments[20] = {
    {"user1:", 4.0, "Hunan flavors"},
    {"user2:", 4.5, "Spicy and fragrant"},
    {"user3:",4.5,"Chairman Mao dish"},
    {"user4:",4.0,"Smoky aroma"},
    {"user5:",5.0,"Authentic spice"}
};

struct comment SkyGarden_comments[20] = {
    {"user1:", 5.0, "Amazing view"},
    {"user2:", 4.5, "Perfect for date"},
    {"user3:",5.0,"Night lights"},
    {"user4:",4.5,"Cocktails good"},
    {"user5:",4.0,"Romantic spot"}
};

// ================== 商铺信息结构体（F1层）==================

struct store_inform Louis_Vuitton_info = {
    "Louis Vuitton",
    "Louis Vuitton, a French luxury brand, founded in 1854, is renowned for its iconic Monogram canvas and luxury leather goods, being a flagship of the LVMH group.",
    "0:/Louis_Vuitton.bin",
    Louis_comments,
    220,
		305,   // 坐标
    "F1",
    "Luxury"
};

struct store_inform Cartier_info = {
    "Cartier",
    "Cartier, founded in Paris in 1847, is a French luxury jewelry and watch maison known as the 'Jeweller of Kings', celebrated for its Panther motif and classic designs.",
    "0:/Cartier.bin",
    Cartier_comments,
    245,
		97,
    "F1",
    "Watches"
};

struct store_inform Hermes_info = {
    "Hermes",
    "Hermes, established in 1837 in Paris, started as a harness workshop and evolved into a pinnacle of French luxury, famous for its handcrafted Kelly and Birkin bags.",
    "0:/Hermes.bin",
    Hermes_comments,
    455,
		97,
    "F1",
    "Luxury"
};

struct store_inform Dior_info = {
    "Dior",
    "Dior, a French luxury goods company founded in 1946, revolutionized fashion with the 'New Look' and epitomizes elegance in haute couture, perfumes, and accessories.",
    "0:/Dior.bin",
    Dior_comments,
    397,
		240,
    "F1",
    "Luxury"
};

struct store_inform Prada_info = {
    "Prada",
    "Prada, founded in 1913 in Milan, is an Italian luxury fashion house known for its understated style, innovative use of nylon, and intellectual approach to fashion.",
    "0:/Prada.bin",
    Prada_comments,
    632,
		240,
    "F1",
    "Luxury"
};

struct store_inform Chanel_info = {
    "Chanel",
    "Chanel, founded by Gabrielle 'Coco' Chanel in 1910, is a preeminent French luxury brand famous for the Chanel No.5 perfume, the little black dress, and the 2.55 handbag.",
    "0:/Chanel.bin",
    Chanel_comments,
    397,
		370,
    "F1",
    "Luxury"
};

struct store_inform YSL_info = {
    "YSL",
    "Yves Saint Laurent, founded in 1961, is a French luxury fashion house that introduced revolutionary designs like the 'Le Smoking' tuxedo suit for women.",
    "0:/YSL.bin",
    YSL_comments,
    632,
		370,
    "F1",
    "Luxury"
};

struct store_inform Sk2_info = {
    "Sk2",
    "SK-II, a Japanese premium skincare brand, is famous for its signature ingredient Pitera and is renowned for its effective anti-aging products like the Facial Treatment Essence.",
    "0:/Sk2.bin",
    Sk2_comments,
    515,
		225,
    "F1",
    "Beauty"
};

struct store_inform Helena_Rubinstein_info = {
    "Helena Rubinstein",
    "Helena Rubinstein, founded in 1902, is a global luxury skincare brand that merges cutting-edge science with beauty, offering high-performance, anti-aging solutions.",
    "0:/Helena_Rubinstein.bin",
    HR_comments,
    515,
		385,
    "F1",
    "Beauty"
};

struct store_inform Rolex_info = {
    "Rolex",
    "Rolex, founded in 1905, is a Swiss luxury watch manufacturer known for its precision, durability, and iconic models like the Submariner and Daytona, symbolizing success.",
    "0:/Rolex.bin",
    Rolex_comments,
    867,
		362,
    "F1",
    "Watches"
};

struct store_inform IWC_info = {
    "IWC",
    "IWC Schaffhausen, founded in 1868, is a Swiss luxury watch manufacturer celebrated for its engineering excellence and classic pilot's watches.",
    "0:/IWC.bin",
    IWC_comments,
    867,
		242,
    "F1",
    "Watches"
};

struct store_inform Puyi_Optical_info = {
    "Puyi Optical",
    "Puyi Optical, founded in 2001, is a high-end eyewear retailer that curates exclusive collections from global luxury brands, offering a personalized and premium shopping experience.",
    "0:/Puyi_Optical.bin",
    Puyi_comments,
    870,
		97,
    "F1",
    "Lifestyle"
};

struct store_inform Omega_info = {
    "Omega",
    "Omega, a Swiss luxury watchmaker founded in 1848, is known for its precision, its historical role as the official timekeeper of the Olympic Games, and the Speedmaster moonwatch.",
    "0:/Omega.bin",
    Omega_comments,
    665,
		97,
    "F1",
    "Watches"
};

struct store_inform Costa_1_info = {
    "Costa 1",
    "Costa Coffee, founded in London in 1971 by Italian brothers, is a global coffeehouse chain renowned for its signature Mocha Italia blend and handcrafted coffee.",
    "0:/Costa_1.bin",
    Costa1_comments,
    847,
		500,
    "F1",
    "Food"
};

struct store_inform Premium_Selected_info = {
    "Premium Selected",
    "Premium Selected is a multi-brand boutique offering a curated selection of avant-garde and established high-end fashion, accessories, and lifestyle products.",
    "0:/Premium_Selected.bin",
    PremiumSelected_comments,
    515,
		500,
    "F1",
    "Lifestyle"
};

// ================== 商铺信息（F2层）==================

struct store_inform Gucci_info = {
    "Gucci",
    "Gucci, founded in Florence in 1921, is an Italian luxury fashion house known for its eclectic, contemporary designs and iconic GG logo and green-red-green web.",
    "0:/Gucci.bin",
    Gucci_comments,
    250,
		80,
    "F2",
    "Luxury"
};

struct store_inform Miu_Miu_info = {
    "Miu Miu",
    "Miu Miu, founded in 1993 by Miuccia Prada, is an Italian high-fashion brand with a playful, rebellious spirit, known for its bold experimentation with materials and form.",
    "0:/Miu_Miu.bin",
    MiuMiu_comments,
    500,
		80,
    "F2",
    "Luxury"
};

struct store_inform Tods_info = {
    "Tod's",
    "Tod's, founded in 1970 by Diego Della Valle, is an Italian luxury brand renowned for its handcrafted shoes, particularly the iconic 'Gommino' driving loafers.",
    "0:/Tods.bin",
    Tods_comments,
    750,
		80,
    "F2",
    "Luxury"
};

struct store_inform Prada_F2_info = {
    "Prada F2",
    "Prada, founded in 1913 in Milan, is an Italian luxury fashion house known for its understated style, innovative use of nylon, and intellectual approach to fashion.",
    "0:/Prada_F2.bin",
    PradaF2_comments,
    210,
		210,
    "F2",
    "Luxury"
};

struct store_inform Carbon_Star_info = {
    "Carbon Star",
    "Carbon Star is a concept store focused on sustainable and eco-friendly fashion, showcasing innovative designs and brands committed to a better future.",
    "0:/Carbon_Star.bin",
    CarbonStar_comments,
    530,
		210,
    "F2",
    "Lifestyle"
};

struct store_inform Costa_F2_info = {
    "Costa F2",
    "Costa Coffee, founded in London in 1971 by Italian brothers, is a global coffeehouse chain renowned for its signature Mocha Italia blend and handcrafted coffee.",
    "0:/Costa_F2.bin",
    CostaF2_comments,
    700,
		510,
    "F2",
    "Food"
};

struct store_inform SKP_Select_Main_info = {
    "SKP Select Main",
    "SKP Select is a high-end multi-brand concept store by SKP, curating a unique mix of international and niche luxury fashion, accessories, and lifestyle goods.",
    "0:/SKP_Select_Main.bin",
    SKPSelectMain_comments,
    430, 
		360,
    "F2",
    "Lifestyle"
};

struct store_inform Coach_info = {
    "Coach",
    "Coach, founded in 1941 in New York, is an American fashion house known for its modern luxury accessories and handbags, combining heritage craftsmanship with a New York spirit.",
    "0:/Coach.bin",
    Coach_comments,
    650,
		285,
    "F2",
    "Luxury"
};

struct store_inform Boss_info = {
    "Boss",
    "Boss, founded in 1924, is a German luxury fashion house known for its sophisticated, contemporary tailoring and high-quality ready-to-wear clothing for men and women.",
    "0:/Boss.bin",
    Boss_comments,
    750,
		285,
    "F2",
    "Luxury"
};

struct store_inform Samsung_info = {
    "Samsung",
    "Samsung, founded in 1938 in Korea, is a global technology leader, renowned for its innovative smartphones, TVs, and home appliances.",
    "0:/Samsung.bin",
    Samsung_comments,
    360,
		210,
    "F2",
    "Electronics"
};

struct store_inform Ecco_info = {
    "Ecco",
    "ECCO, a Danish shoe manufacturer founded in 1963, is known for its comfortable, Scandinavian-design footwear and innovative leather technology.",
    "0:/Ecco.bin",
    Ecco_comments,
    210,
		360,
    "F2",
    "Lifestyle"
};

struct store_inform Philips_info = {
    "Philips",
    "Philips, founded in 1891 in the Netherlands, is a leading health technology company focused on improving people's lives through meaningful innovation in healthcare and personal care.",
    "0:/Philips.bin",
    Philips_comments,
    325,
		510,
    "F2",
    "Electronics"
};

// ================== 商铺信息（F3层）==================

struct store_inform BeijingChufang_info = {
    "beijingchufang",
    "Beijing Chufang Traditional Peking duck and northern Chinese cuisine in a refined setting.",
    "0:/BeijingChufang.bin",
    BeijingChufang_comments,
    -1,
		1,   // 坐标用负数+编号表示
    "F3",
    "Food"
};

struct store_inform ChuLingLong_info = {
    "chulinglong",
    "Chu Ling Long Exquisite Cantonese dim sum and innovative fusion dishes.",
    "0:/ChuLingLong.bin",
    ChuLingLong_comments,
    -1,
		2,
    "F3",
    "Food"
};

struct store_inform ShangHeChuan_info = {
    "shanghechuan",
    "Shang He Chuan  Authentic Sichuan cuisine, famous for its bold mala flavors.",
    "0:/ShangHeChuan.bin",
    ShangHeChuan_comments,
    -1,
		3,
    "F3",
    "Food"
};

struct store_inform YiZuoYiWang_info = {
    "yizuoyiwang",
    "Yi Zuo Yi Wang  Trendy restaurant offering a creative blend of East-West fusion.",
    "0:/YiZuoYiWang.bin",
    YiZuoYiWang_comments,
    -1,
		4,
    "F3",
    "Food"
};

struct store_inform YanYu_info = {
    "yanyu",
    "Yan Yu Premium seafood and hot pot with a romantic, contemporary vibe.",
    "0:/YanYu.bin",
    YanYu_comments,
    -1,
		5,
    "F3",
    "Food"
};

struct store_inform WangPinNiuPai_info = {
    "wangpinniupai",
    "Wang Pin Niu Pai High-end steakhouse serving dry-aged beef and fine wines.",
    "0:/WangPinNiuPai.bin",
    WangPinNiuPai_comments,
    -1,
		6,
    "F3",
    "Food"
};

struct store_inform ZhenBaoHaiXian_info = {
    "zhenbaohaixian",
    "Zhen Bao Hai Xian Luxurious seafood buffet with live cooking stations.",
    "0:/ZhenBaoHaiXian.bin",
    ZhenBaoHaiXian_comments,
    -1,
		7,
    "F3",
    "Food"
};

struct store_inform ChuXiangFuRong_info = {
    "chuxiangfurong",
    "Chu Xiang Fu Rong Classic Hunan cuisine, known for its spicy and savory dishes.",
    "0:/ChuXiangFuRong.bin",
    ChuXiangFuRong_comments,
    -1,
		8,
    "F3",
    "Food"
};

struct store_inform SkyGarden_info = {
    "Sky Garden",
    "Sky Garden A breathtaking rooftop garden caf with panoramic views and light refreshments.",
    "0:/SkyGarden.bin",
    SkyGarden_comments,
    -1,
		0,
    "F3",
    "Food"
};

// 所有商铺的指针列表（末尾NULL表示结束）
struct store_inform *store_info_list[] = {
    &Louis_Vuitton_info,
    &Cartier_info,
    &Hermes_info,
    &Dior_info,
    &Prada_info,
    &Chanel_info,
    &YSL_info,
    &Sk2_info,
    &Helena_Rubinstein_info,
    &Rolex_info,
    &IWC_info,
    &Puyi_Optical_info,
    &Omega_info,
    &Costa_1_info,
    &Premium_Selected_info,
    &Gucci_info,
    &Miu_Miu_info,
    &Tods_info,
    &Prada_F2_info,
    &Carbon_Star_info,
    &Costa_F2_info,
    &SKP_Select_Main_info,
    &Coach_info,
    &Boss_info,
    &Samsung_info,
    &Ecco_info,
    &Philips_info,
    &BeijingChufang_info,
    &ChuLingLong_info,
    &ShangHeChuan_info,
    &YiZuoYiWang_info,
    &YanYu_info,
    &WangPinNiuPai_info,
    &ZhenBaoHaiXian_info,
    &ChuXiangFuRong_info,
    &SkyGarden_info,
    NULL
};

// 外部屏幕变量（在其他文件定义）
extern lv_obj_t *F0_screen;
extern lv_obj_t *rating_screen;
extern lv_obj_t *F1_screen;
extern lv_obj_t *F2_screen;
extern lv_obj_t *F3_screen;
extern lv_obj_t *detail_screen;

// 当前选中的商铺信息（全局）
struct store_inform store_info;

// 函数声明
void F1_btn_event_cb(lv_event_t *e);
void F2_btn_event_cb(lv_event_t *e);
void store_list_click_cb(lv_event_t * e);
void category_btn_cb(lv_event_t * e);
void close_category_cb(lv_event_t * e);
void sort_btn_cb(lv_event_t * e);
void switchF0_cb();
void switchF1_cb();
void switchF2_cb();
void switchF3_cb();
void detail();
void back_init();
void back_pushing_switch(lv_obj_t *screen);
void back_clicked();
extern void update_store_display();

// 创建一个普通按钮（通用函数）
lv_obj_t *btn_create(uint16_t pos_x, uint16_t pos_y, uint16_t size_x, uint16_t size_y, int color_usual, int color_pressed, lv_obj_t * display, char* text)
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

// 圆弧按钮的点击检测（用来实现环形菜单）
static void arc_hit_test_cb(lv_event_t * e) {
    lv_hit_test_info_t * info = lv_event_get_param(e);
    lv_obj_t * obj = lv_event_get_target(e);

    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);
    lv_coord_t cx = coords.x1 + lv_area_get_width(&coords) / 2;
    lv_coord_t cy = coords.y1 + lv_area_get_height(&coords) / 2;

    lv_coord_t px = info->point->x;
    lv_coord_t py = info->point->y;

    int32_t dx = px - cx;
    int32_t dy = py - cy;
    int32_t r_sq = dx * dx + dy * dy;

    int32_t outer_r = lv_area_get_width(&coords) / 2;
    int32_t inner_r = outer_r - lv_obj_get_style_arc_width(obj, LV_PART_MAIN);

    if(r_sq < inner_r * inner_r || r_sq > outer_r * outer_r) {
        info->res = false;
        return;
    }

    float angle_f = atan2((float)dy, (float)dx) * 180.0f / 3.1415926f;
    if(angle_f < 0) angle_f += 360.0f;
    uint16_t angle = (uint16_t)angle_f;

    uint16_t start_angle = lv_arc_get_bg_angle_start(obj);
    uint16_t end_angle = lv_arc_get_bg_angle_end(obj);

    bool in_angle = false;
    if (start_angle <= end_angle) {
        in_angle = (angle >= start_angle && angle <= end_angle);
    } else {
        in_angle = (angle >= start_angle || angle <= end_angle);
    }

    if(!in_angle) {
        info->res = false;
    }
}

// 创建一个圆弧形状的按钮（用在 F3 层的环形菜单）
lv_obj_t * arc_btn_create(lv_obj_t * parent, int16_t start_angle, int16_t end_angle, 
                          lv_coord_t radius, lv_coord_t thickness, 
                          int color, const char * text) 
{
    lv_obj_t * arc = lv_arc_create(parent);
    lv_obj_set_size(arc, radius * 2, radius * 2);
    lv_obj_center(arc); 

    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_width(arc, 0, LV_PART_INDICATOR);

    lv_arc_set_bg_angles(arc, start_angle, end_angle);
    lv_arc_set_value(arc, 0);
    lv_obj_set_style_arc_width(arc, thickness, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_hex(color), LV_PART_MAIN);

    lv_obj_set_style_arc_rounded(arc, false, LV_PART_MAIN);
    lv_obj_add_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(arc, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_add_event_cb(arc, arc_hit_test_cb, LV_EVENT_HIT_TEST, NULL);

    lv_obj_t * label = lv_label_create(arc);
    lv_label_set_text(label, text);
    
    float mid_angle = (start_angle + end_angle) / 2.0f;
    float mid_rad = mid_angle * 3.14159f / 180.0f;
    lv_coord_t mid_r = radius - thickness / 2;
    lv_coord_t lx = (lv_coord_t)(mid_r * cos(mid_rad));
    lv_coord_t ly = (lv_coord_t)(mid_r * sin(mid_rad));
    
    lv_obj_align(label, LV_ALIGN_CENTER, lx, ly);

    return arc;
}

// 创建所有地图按钮（F1、F2、F3层）
void button()
{
    // ========== F1 层按钮 ==========
    lv_obj_t *btn_lift1 = btn_create(37,200,80,80,0x000000,0x8B0000,F1_screen,"Lift 1");
    lv_obj_t *btn_serve1 = btn_create(37,57,80,100,0x1A5276,0x0E3F5D,F1_screen,"Serve 1");
    lv_obj_t *btn_escape1 = btn_create(37,320,80,80,0x00CC00,0x00CC00,F1_screen,"Escape");
    lv_obj_t *btn_cartier = btn_create(180,65,130,64,0xFFB300,0xCC8B00,F1_screen,"Cartier");
    lv_obj_t *btn_lv = btn_create(155,200,130,210,0xD2691E,0xA0522D,F1_screen,"Louis Vuitton");
    lv_obj_t *btn_hermes = btn_create(390,65,130,64,0xE65100,0xB84000,F1_screen,"Hermes");
    lv_obj_t *btn_dior = btn_create(350,200,95,80,0xFF6A00,0xCC5500,F1_screen,"Dior");
    lv_obj_t *btn_prada = btn_create(585,200,95,80,0xFF8C00,0xCC6F00,F1_screen,"Prada");
    lv_obj_t *btn_chanel = btn_create(350,330,95,80,0xFFA726,0xCC851E,F1_screen,"Chanel");
    lv_obj_t *btn_ysl = btn_create(585,330,95,80,0x4B0082,0x370060,F1_screen,"YSL");
    lv_obj_t *btn_sk2 = btn_create(460,200,110,50,0xFF69B4,0xCC5490,F1_screen,"Sk2");
    lv_obj_t *btn_hr = btn_create(460,360,110,50,0x9370DB,0x7058A8,F1_screen,"Helena Rubinstein");
    lv_obj_t *btn_rolex = btn_create(800,320,135,85,0x0074E4,0x0059B2,F1_screen,"Rolex");
    lv_obj_t *btn_iwc = btn_create(800,200,135,85,0x003FFF,0x002699,F1_screen,"IWC");
    lv_obj_t *btn_puyi = btn_create(805,65,130,64,0x4682B4,0x355F80,F1_screen,"Puyi Optical");
    lv_obj_t *btn_omega = btn_create(600,65,130,64,0x4169E1,0x3254A8,F1_screen,"Omega");
    lv_obj_t *btn_Costa1 = btn_create(760,440,175,120,0x8B4513,0x6B3210,F1_screen,"Costa 1");
    lv_obj_t *btn_select = btn_create(420,440,190,120,0x2C3E50,0x1F2D3D,F1_screen,"Premiun Selected");
    lv_obj_t *btn_lift2 = btn_create(645,480,75,60,0x000000,0x690000,F1_screen,"Lift2");
    lv_obj_t *btn_wc1 = btn_create(155,450,130,90,0x00CC00,0x00CC00,F1_screen,"Entrance");
    lv_obj_t *btn_serve2 = btn_create(315,480,75,60,0x1A5276,0x0E3F5D,F1_screen,"Serve 2");

    // 绑定 F1 按钮的点击事件（传入商铺名字）
    lv_obj_add_event_cb(btn_cartier, F1_btn_event_cb, LV_EVENT_CLICKED, "Cartier");
    lv_obj_add_event_cb(btn_lv,      F1_btn_event_cb, LV_EVENT_CLICKED, "Louis Vuitton");
    lv_obj_add_event_cb(btn_hermes,  F1_btn_event_cb, LV_EVENT_CLICKED, "Hermes");
    lv_obj_add_event_cb(btn_dior,    F1_btn_event_cb, LV_EVENT_CLICKED, "Dior");
    lv_obj_add_event_cb(btn_prada,   F1_btn_event_cb, LV_EVENT_CLICKED, "Prada");
    lv_obj_add_event_cb(btn_chanel,  F1_btn_event_cb, LV_EVENT_CLICKED, "Chanel");
    lv_obj_add_event_cb(btn_ysl,     F1_btn_event_cb, LV_EVENT_CLICKED, "YSL");
    lv_obj_add_event_cb(btn_sk2,     F1_btn_event_cb, LV_EVENT_CLICKED, "Sk2");
    lv_obj_add_event_cb(btn_hr,      F1_btn_event_cb, LV_EVENT_CLICKED, "Helena Rubinstein");
    lv_obj_add_event_cb(btn_rolex,   F1_btn_event_cb, LV_EVENT_CLICKED, "Rolex");
    lv_obj_add_event_cb(btn_iwc,     F1_btn_event_cb, LV_EVENT_CLICKED, "IWC");
    lv_obj_add_event_cb(btn_puyi,    F1_btn_event_cb, LV_EVENT_CLICKED, "Puyi Optical");
    lv_obj_add_event_cb(btn_omega,   F1_btn_event_cb, LV_EVENT_CLICKED, "Omega");
    lv_obj_add_event_cb(btn_Costa1,  F1_btn_event_cb, LV_EVENT_CLICKED, "Costa 1");
    lv_obj_add_event_cb(btn_select,  F1_btn_event_cb, LV_EVENT_CLICKED, "Premium Selected");
    
    // ========== F2 层按钮 ==========
    lv_obj_t *btn_gucci = btn_create(150, 50, 200, 60, 0xFFB300, 0xFF8C00, F2_screen, "GUCCI");
    lv_obj_t *btn_miumiu = btn_create(400, 50, 200, 60, 0xFF69B4, 0xFF1493, F2_screen, "miu miu");
    lv_obj_t *btn_tods = btn_create(650, 50, 200, 60, 0xE65100, 0xC43F00, F2_screen, "TOD'S");
    lv_obj_t *btn_wc1_f2 = btn_create(50, 150, 80, 80, 0x4682B4, 0x2F4F4F, F2_screen, "WC1");
    lv_obj_t *btn_lift1_f2 = btn_create(50, 270, 80, 80, 0x000000, 0xCC0000, F2_screen, "Lift1");
    lv_obj_t *btn_escape1_f2 = btn_create(50, 390, 80, 60, 0x00CC00, 0xCC0000, F2_screen, "Escape1");
    lv_obj_t *btn_prada_f2 = btn_create(180, 150, 60, 120, 0xFF8C00, 0xFF4500, F2_screen, "PRADA");
    lv_obj_t *btn_SAMSUNG = btn_create(280, 150, 160, 120, 0x0074E4, 0x0059B2, F2_screen, "SAMSUNG");
    lv_obj_t *btn_ECCO = btn_create(180, 300, 60, 120, 0x4682B4, 0x355F80, F2_screen, "ECCO");
    lv_obj_t *btn_carbonstar = btn_create(480, 150, 100, 120, 0x90EE90, 0x6B8E23, F2_screen, "carbon star");
    lv_obj_t *btn_coach = btn_create(620, 150, 60, 270, 0xFFC299, 0xFFA500, F2_screen, "COACH");
    lv_obj_t *btn_skpselect_main = btn_create(280, 300, 300, 120, 0xCC0000, 0x00CC00, F2_screen, "SKP SELECT");
    lv_obj_t *btn_boss = btn_create(720, 150, 60, 270, 0xA9B1BC, 0x808890, F2_screen, "BOSS");
    lv_obj_t *btn_philips = btn_create(150, 460, 350, 100, 0x003FFF, 0x002699, F2_screen, "PHILIPS");
    lv_obj_t *btn_costa = btn_create(525, 460, 350, 100, 0xA0522D, 0x8B4513, F2_screen, "COSTA");
    lv_obj_t *btn_escape2_f2 = btn_create(820, 150, 80, 80, 0x00CC00, 0xCC0000, F2_screen, "Escape2");
    lv_obj_t *btn_wc2_f2 = btn_create(820, 270, 80, 80, 0x4682B4, 0x2F4F4F, F2_screen, "WC2");
    lv_obj_t *btn_lift2_f2 = btn_create(820, 390, 80, 60, 0x000000, 0xCC0000, F2_screen, "Lift2");

    // 绑定 F2 按钮事件
    lv_obj_add_event_cb(btn_gucci,         F2_btn_event_cb, LV_EVENT_CLICKED, "Gucci");
    lv_obj_add_event_cb(btn_miumiu,        F2_btn_event_cb, LV_EVENT_CLICKED, "Miu Miu");
    lv_obj_add_event_cb(btn_tods,          F2_btn_event_cb, LV_EVENT_CLICKED, "Tod's");
    lv_obj_add_event_cb(btn_prada_f2,      F2_btn_event_cb, LV_EVENT_CLICKED, "Prada F2");
    lv_obj_add_event_cb(btn_SAMSUNG,       F2_btn_event_cb, LV_EVENT_CLICKED, "Samsung");
    lv_obj_add_event_cb(btn_ECCO,          F2_btn_event_cb, LV_EVENT_CLICKED, "Ecco");        
    lv_obj_add_event_cb(btn_carbonstar,    F2_btn_event_cb, LV_EVENT_CLICKED, "Carbon Star");
    lv_obj_add_event_cb(btn_coach,         F2_btn_event_cb, LV_EVENT_CLICKED, "Coach");
    lv_obj_add_event_cb(btn_skpselect_main,F2_btn_event_cb, LV_EVENT_CLICKED, "SKP Select Main");
    lv_obj_add_event_cb(btn_boss,          F2_btn_event_cb, LV_EVENT_CLICKED, "Boss");
    lv_obj_add_event_cb(btn_philips,       F2_btn_event_cb, LV_EVENT_CLICKED, "Philips");
    lv_obj_add_event_cb(btn_costa,         F2_btn_event_cb, LV_EVENT_CLICKED, "Costa F2");
    
    // ========== F3 层圆弧按钮 ==========
    lv_coord_t r = 250;
    lv_coord_t thick = 80;
    
    lv_obj_t *btn_beijingchufang = arc_btn_create(F3_screen, 0,   40,  r, thick, 0xFF5722, "beijing\nchufang");
    lv_obj_t *btn_chulinglong = arc_btn_create(F3_screen, 45,  85,  r, thick, 0xFF9800, "chulinglong");
    lv_obj_t *btn_shanghechuan = arc_btn_create(F3_screen, 90,  130, r, thick, 0xFFC107, "shanghechuan");
    lv_obj_t *btn_yizuoyiwang = arc_btn_create(F3_screen, 135, 175, r, thick, 0x81D8D0, "yizuo\nyiwang");
    lv_obj_t *btn_yanyu = arc_btn_create(F3_screen, 180, 220, r, thick, 0x00BCD4, "yanyu");
    lv_obj_t *btn_wangpinniupai = arc_btn_create(F3_screen, 225, 265, r, thick, 0x2196F3, "wangpin\nniupai");
    lv_obj_t *btn_zhenbaohaixian = arc_btn_create(F3_screen, 270, 310, r, thick, 0x9C27B0, "zhenbao\nhaixian");
    lv_obj_t *btn_chuxiangfurong = arc_btn_create(F3_screen, 315, 355, r, thick, 0xE91E63, "chuxiang\nfurong");

    // 绑定 F3 事件
    lv_obj_add_event_cb(btn_beijingchufang, F2_btn_event_cb, LV_EVENT_CLICKED, "beijingchufang");
    lv_obj_add_event_cb(btn_chulinglong,    F2_btn_event_cb, LV_EVENT_CLICKED, "chulinglong");
    lv_obj_add_event_cb(btn_shanghechuan,   F2_btn_event_cb, LV_EVENT_CLICKED, "shanghechuan");
    lv_obj_add_event_cb(btn_yizuoyiwang,    F2_btn_event_cb, LV_EVENT_CLICKED, "yizuoyiwang");
    lv_obj_add_event_cb(btn_yanyu,          F2_btn_event_cb, LV_EVENT_CLICKED, "yanyu");
    lv_obj_add_event_cb(btn_wangpinniupai,  F2_btn_event_cb, LV_EVENT_CLICKED, "wangpinniupai");
    lv_obj_add_event_cb(btn_zhenbaohaixian, F2_btn_event_cb, LV_EVENT_CLICKED, "zhenbaohaixian");
    lv_obj_add_event_cb(btn_chuxiangfurong, F2_btn_event_cb, LV_EVENT_CLICKED, "chuxiangfurong");
    
    // 中间的 Sky Garden 圆形按钮
    lv_obj_t *btn_skygarden = btn_create(0, 0, 220, 220, 0x7CFC00, 0x7CFC00, F3_screen, "sky garden");
    lv_obj_set_style_radius(btn_skygarden, LV_RADIUS_CIRCLE, 0);
    lv_obj_center(btn_skygarden);
    lv_obj_add_event_cb(btn_skygarden, F2_btn_event_cb, LV_EVENT_CLICKED, "Sky Garden");
}

// 创建 F1 层的找路按钮（服务台、出口、电梯）
void find_button()
{
    lv_obj_t *btn_find_serve = btn_create(950,65,50,50,0xFF0000,0x00FF00,F1_screen,"find\nserve");
    lv_obj_t *btn_find_escape = btn_create(950,130,50,50,0xFF0000,0x00FF00,F1_screen,"find\nescape");
    lv_obj_t *btn_find_lift = btn_create(950,195,50,50,0xFF0000,0x00FF00,F1_screen,"find\nlift");
    
    // 这些绑定函数在 find.c 中定义
    extern void find_serve_btn_bind(lv_obj_t* btn);
    extern void find_escape_btn_bind(lv_obj_t* btn);
    extern void find_lift_btn_bind(lv_obj_t* btn);
    
    find_serve_btn_bind(btn_find_serve);
    find_escape_btn_bind(btn_find_escape);
    find_lift_btn_bind(btn_find_lift);
}

// 创建 F2 层的找路按钮（卫生间、出口、电梯）
void find2_button()
{
    lv_obj_t *btn_find_wc = btn_create(950,65,50,50,0xFF0000,0x00FF00,F2_screen,"find\nWC");
    lv_obj_t *btn_find_escape = btn_create(950,130,50,50,0xFF0000,0x00FF00,F2_screen,"find\nescape");
    lv_obj_t *btn_find_lift = btn_create(950,195,50,50,0xFF0000,0x00FF00,F2_screen,"find\nlift");
    
    extern void find_serve_f2_btn_bind(lv_obj_t* btn);
    extern void find_escape_f2_btn_bind(lv_obj_t* btn);
    extern void find_lift_f2_btn_bind(lv_obj_t* btn);
    
    find_serve_f2_btn_bind(btn_find_wc);
    find_escape_f2_btn_bind(btn_find_escape);
    find_lift_f2_btn_bind(btn_find_lift);
}

// 创建楼层切换按钮（在各个屏幕底部或边缘）
void switch_button()
{
    // 从 F1/F2/F3 切换到 F0 的按钮
    lv_obj_t *btn_1F0 = btn_create(10,550,50,50,0xFF0000,0x00FF00,F1_screen,"F0");
    lv_obj_t *btn_2F0 = btn_create(10,550,50,50,0xFF0000,0x00FF00,F2_screen,"F0");
    lv_obj_t *btn_3F0 = btn_create(10,550,50,50,0xFF0000,0xFF0000,F3_screen,"F0");
    
    // 从 F0 切换到各楼层的按钮
    lv_obj_t *btn_0F1 = btn_create(950,270,50,50,0xA24BC7,0xA24BC7,F0_screen,"F1");
    lv_obj_t *btn_0F2 = btn_create(950,335,50,50,0xA24BC7,0xA24BC7,F0_screen,"F2");
    lv_obj_t *btn_0F3 = btn_create(950,400,50,50,0xA24BC7,0xA24BC7,F0_screen,"F3");
    
    // 从 F1 切换到 F1/F2/F3
    lv_obj_t *btn_1F1 = btn_create(950,270,50,50,0x00FF00,0x00FF00,F1_screen,"F1");
    lv_obj_t *btn_1F2 = btn_create(950,335,50,50,0xFF0000,0x00FF00,F1_screen,"F2");
    lv_obj_t *btn_1F3 = btn_create(950,400,50,50,0xFF0000,0x00FF00,F1_screen,"F3");
    
    // 从 F2 切换到各楼层
    lv_obj_t *btn_2F1 = btn_create(950,270,50,50,0xFF0000,0x00FF00,F2_screen,"F1");
    lv_obj_t *btn_2F2 = btn_create(950,335,50,50,0x00FF00,0x00FF00,F2_screen,"F2");
    lv_obj_t *btn_2F3 = btn_create(950,400,50,50,0xFF0000,0x00FF00,F2_screen,"F3");
    
    // 从 F3 切换到各楼层
    lv_obj_t *btn_3F1 = btn_create(950,270,50,50,0xFF0000,0x00FF00,F3_screen,"F1");
    lv_obj_t *btn_3F2 = btn_create(950,335,50,50,0xFF0000,0x00FF00,F3_screen,"F2");
    lv_obj_t *btn_3F3 = btn_create(950,400,50,50,0x00FF00,0x00FF00,F3_screen,"F3");
    
    // 绑定事件
    lv_obj_add_event_cb(btn_1F0, switchF0_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_2F0, switchF0_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_3F0, switchF0_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_add_event_cb(btn_0F1, switchF1_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_0F2, switchF2_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_0F3, switchF3_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_add_event_cb(btn_1F1, switchF1_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_1F2, switchF2_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_1F3, switchF3_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_add_event_cb(btn_2F1, switchF1_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_2F2, switchF2_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_2F3, switchF3_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_add_event_cb(btn_3F1, switchF1_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_3F2, switchF2_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_3F3, switchF3_cb, LV_EVENT_CLICKED, NULL);
}

// 创建排序按钮（在各个屏幕的右上角）
void sort_button(){
    lv_obj_t *btn_sortF0 = btn_create(950, 550, 50, 50, 0xA24BC7, 0xA24BC7, F0_screen, "Sort");
    lv_obj_t *btn_sortF1 = btn_create(950, 550, 50, 50, 0xFF0000, 0xFF0000, F1_screen, "Sort");
    lv_obj_t *btn_sortF2 = btn_create(950, 550, 50, 50, 0xFF0000, 0xFF0000, F2_screen, "Sort");
    lv_obj_t *btn_sortF3 = btn_create(950, 550, 50, 50, 0xFF0000, 0xFF0000, F3_screen, "Sort");
    lv_obj_add_event_cb(btn_sortF0, sort_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_sortF1, sort_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_sortF2, sort_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_sortF3, sort_btn_cb, LV_EVENT_CLICKED, NULL);
}

// 根据名字查找商铺（精确匹配）
struct store_inform* find_store_by_name(const char *name)
{
    for (int i = 0; i < (sizeof(store_info_list) / sizeof(store_info_list[0])); i++) 
    {
        if (strcmp(store_info_list[i]->name, name) == 0) 
        {
            return store_info_list[i];
        }
    }
    return NULL;  		
}

// F1 层按钮点击回调（进入商铺详情）
void F1_btn_event_cb(lv_event_t *e)
{		
    const char *name = (const char *)lv_event_get_user_data(e);
    struct store_inform *info = find_store_by_name(name);
    if (info == NULL) return;
    store_info = *info;
    for (int i = 0; i < 20; i++) {
        store_comment[i] = info->comments[i];
    }          
    lv_obj_clean(detail_screen);
    detail();
    update_store_display();	
    back_pushing_switch(detail_screen);
}

// F2 层按钮点击回调
void F2_btn_event_cb(lv_event_t *e)
{		
    const char *name = (const char *)lv_event_get_user_data(e);
    struct store_inform *info = find_store_by_name(name);
    if (info == NULL) return;
    store_info = *info;
    for (int i = 0; i < 20; i++) {
        store_comment[i] = info->comments[i];
    }          
    lv_obj_clean(detail_screen);
    detail();		
    update_store_display();	
    back_pushing_switch(detail_screen);
}

// 楼层切换回调函数
void switchF1_cb() { back_pushing_switch(F1_screen); back_init(); }
void switchF2_cb() { back_pushing_switch(F2_screen); back_init(); }
void switchF3_cb() { back_pushing_switch(F3_screen); back_init(); }
void switchF0_cb() { back_pushing_switch(F0_screen); back_init(); }

// 分类排序窗口的全局变量
lv_obj_t * category_contain = NULL;   // 弹窗容器
lv_obj_t * store_list = NULL;         // 商铺列表控件

// 点击分类列表中的商铺，进入详情
void store_list_click_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    struct store_inform * info = (struct store_inform *)lv_obj_get_user_data(btn);
    if (info == NULL) return;
    store_info = *info;
    for (int i = 0; i < 20; i++) {
        store_comment[i] = info->comments[i];
    }
    lv_obj_clean(detail_screen);
    detail();
    update_store_display();
    if(category_contain != NULL) {
        lv_obj_del(category_contain);
        category_contain = NULL;
    }
    back_pushing_switch(detail_screen);
}

// 分类按钮回调：显示该分类下的所有商铺（并计算平均评分）
void category_btn_cb(lv_event_t * e) {
    const char * cat_name = (const char *)lv_event_get_user_data(e);
    lv_obj_clean(store_list); 
    for(int i = 0; store_info_list[i] != NULL; i++) {
        if(store_info_list[i]->category != NULL && strcmp(store_info_list[i]->category, cat_name) == 0) {
            char buf[20];
            float total = 0;
            int count = 0;
            // 计算平均分
            for(int j = 0; j < 20 && store_info_list[i]->comments[j].name[0] != '\0'; j++)
            {
                total += store_info_list[i]->comments[j].score;
                count++;
            }
            float average = (float)total / (float)count;
            sprintf(buf, "%.1f", average);
            char btn_text[64];
            sprintf(btn_text, "%s (rating: %s)", store_info_list[i]->name, buf);
            lv_obj_t * btn = lv_list_add_btn(store_list, NULL, btn_text);
            lv_obj_set_user_data(btn, store_info_list[i]);
            lv_obj_add_event_cb(btn, store_list_click_cb, LV_EVENT_CLICKED, NULL);
        }
    }
}

// 关闭分类排序窗口
void close_category_cb(lv_event_t * e) {
    if(category_contain != NULL) {
        lv_obj_del(category_contain);
        category_contain = NULL;
    }
}

// 排序按钮回调：弹出窗口，包含分类按钮（评分排序功能可在此扩展）
void sort_btn_cb(lv_event_t * e) {
    if(category_contain != NULL) return;
    
    category_contain = lv_obj_create(lv_scr_act());
    lv_obj_set_size(category_contain, 800, 450);
    lv_obj_center(category_contain);

    lv_obj_t * close_btn = btn_create(690, 0, 80, 40, 0x0000FF, 0x0000FF, category_contain, "Close");
    lv_obj_add_event_cb(close_btn, close_category_cb, LV_EVENT_CLICKED, NULL);

    // 分类按钮
    const char * categories[] = {"Luxury", "Watches", "Beauty", "Food", "Electronics", "Lifestyle"};
    for(int i = 0; i < 6; i++) {
        lv_obj_t * btn = btn_create(20, 20 + i * 65, 150, 50, 0xA24BC7, 0x822BA7, category_contain, (char*)categories[i]);
        lv_obj_add_event_cb(btn, category_btn_cb, LV_EVENT_CLICKED, (void*)categories[i]);
    }
    store_list = lv_list_create(category_contain);
    lv_obj_set_size(store_list, 500, 380);
    lv_obj_set_pos(store_list, 190, 20);
    
    category_btn_cb(e); // 默认显示第一个分类
}
