#include "world_internationalization_standard.h"


/* optional
#define WORLD_A_SilentRadianceWithVersion_r 0.9f
#define WORLD_A_SilentRadianceWithVersion_g 0.4f
#define WORLD_A_SilentRadianceWithVersion_b 1.f

#define WORLD_A_SilentRadiance_app_r 1.f
#define WORLD_A_SilentRadiance_app_g 0.3f
#define WORLD_A_SilentRadiance_app_b 0.8f
*/

/* one of these four: the application name
沉默的光芒
光芒
雷斯普伦杜
辉光
*/
#define WORLD_Silent_Radiance "辉光"
#define WORLD_Silent_Radiance_With_Version_Splash_Title "辉光 1.3"
// WORLD_A_SilentRadiance_app - is an optional second name that points to silentradiance.com
// useful when the app name is rebranded and we want someone to know the original
// not defined for english
#define WORLD_A_SilentRadiance_app "(SilentRadiance.com)"

#define WORLD__432_Namaste " 432 合十礼"




#define WORLD_In_Rememberance_of "I纪念"
#define WORLD_Larry_Harvey "拉里哈维"

#define WORLD_Larry_qt_s_Possee_the_Robot_Haters "拉里的不法之徒，机器人仇恨者"
#define WORLD_Developed_by_Hipolito_Medrano "由Hipolito Medrano开发"
#define WORLD_and_Hibbard_Engler "和希巴德恩格勒"

/* 
太空船与克苏鲁神地在这里
不明飞行物与克苏鲁神地在这里
*/
#define WORLD_UFOs_LAND_HERE__minus__gt_ "不明飞行物与克苏鲁神地在这里 ->"

#define WORLD_Larry_comma__thanks_for_Burning_Man "拉里，谢谢燃烧的人"
#define WORLD_And_comma__well_comma__thats_about_it "而且，好吧，那就是它"
#define WORLD_We_came_to_be_because_of_you "我们因你而生活和茁壮成长"
#define WORLD_And_brought_you_here "我们把你带到了这里"
#define WORLD_All_of_the_good_times_you "你保持的所有美好时光。"
#define WORLD_keep_dot__Take_this_letter "拿这封信"
#define WORLD_Thats_all_you_get "这就是你得到的全部"
#define WORLD_Now_comma__lets_burn_something_excl_ "现在，让我们烧烟花吧！"


// 卡拉OK    or 唱歌
#define WORLD_display_visual_voice_Along "OK"
#define WORLD_display_visual_voice__Sing "卡拉"

// 商店橱窗
#define WORLD_display_window_shop__dlr_hop "商店"
#define WORLD_display_window_shop_Show "节目"
#define WORLD_display_window_shop_Window "窗口"

#define WORLD_display_buy_it_now_Buy "购买"
#define WORLD_display_buy_it_now__dlr__dlr__dlr_ "¥¥¥"
#define WORLD_display_buy_it_now_Get "得到"
#define WORLD_display_buy_it_now_Now "现在"

#define WORLD_display_words_Disco "迪斯科"
#define WORLD_display_words_Info "信息"


#define WORLD_display_main_menu_Virtual "虚拟"
#define WORLD_display_main_menu_Reality "现实"

#define WORLD_display_top_menu_666 "666"
#define WORLD_display_top_menu_Dead "感恩的死"
#define WORLD_display_top_menu_Hib "Hib"
#define WORLD_display_top_menu_Psy "果阿"
#define WORLD_display_top_menu_Rge "雷鬼"

#define WORLD_display_staff_A "音符A"
#define WORLD_display_staff_Ab "音符A平"
#define WORLD_display_staff_B "音符B"
#define WORLD_display_staff_Bb "音符B平"
#define WORLD_display_staff_C "音符C"
#define WORLD_display_staff_C_hash_ "音符C锐利"
#define WORLD_display_staff_D "音符D"
#define WORLD_display_staff_E "音符E"
#define WORLD_display_staff_Eb "音符E平"
#define WORLD_display_staff_F "音符F"
#define WORLD_display_staff_F_hash_ "音符F锐利"
#define WORLD_display_staff_G "音符G"
#define WORLD_display_staff_staff "谱号"

#define WORLD_SilentRadianceWithVersion_SCALE 0.17f
#define WORLD_SilentRadianceWithVersion_x -0.90f
#define WORLD_SilentRadianceWithVersion_y -0.25f

#define WORLD_display_visual_voice__Sing_scale 0.55
#define WORLD_display_visual_voice__Sing_x -0.80f
#define WORLD_display_visual_voice__Sing_y 0.15f
#define WORLD_display_visual_voice_Along_x -0.80f
#define WORLD_display_visual_voice_Along_y -0.70f

#define WORLD_display_words_Disco_line_width 0.128f
#define WORLD_display_words_Disco_scale 0.7f
#define WORLD_display_words_Disco_x (-0.8f)
#define WORLD_display_words_Disco_y (0.15f)

#define WORLD_display_words_Info_line_width 0.128f
#define WORLD_display_words_Info_scale 0.7f
#define WORLD_display_words_Info_x (WORLD_display_words_Disco_x+0.2 \
  * WORLD_display_words_Info_scale)
#define WORLD_display_words_Info_y (WORLD_display_words_Disco_y- \
  WORLD_display_words_Info_scale-0.3f)
  
  
#define WORLD_display_window_shop_Window_scale 0.5
#define WORLD_display_window_shop_Window_x (-0.9f)
#define WORLD_display_window_shop_Window_y (-0.05f)
  
#define WORLD_display_window_shop_Show_scale 0.5  
#define WORLD_display_window_shop_Show_x (WORLD_display_window_shop_Window_x+0.35)
#define WORLD_display_window_shop_Show_y (WORLD_display_window_shop_Window_y - 0.7)


#define WORLD_display_window_shop__dlr_hop_x \
  (WORLD_display_window_shop_Window_x+0.35) 
#define WORLD_display_window_shop__dlr_hop_y \
  (WORLD_display_window_shop_Window_y - 0.7)

#define WORLD_display_buy_it_now_beat_width 0.160f


#define WORLD_display_buy_it_now_beat_width 0.160f
#define WORLD_display_buy_it_now_onset_width 0.130f
#define WORLD_display_buy_it_now_normal_width 0.128f

#define WORLD_display_buy_it_now_Buy_scale 0.6f
#define WORLD_display_buy_it_now_Buy_x -0.5f
#define WORLD_display_buy_it_now_Buy_y 0.13

#define WORLD_display_buy_it_now__dlr__dlr__dlr_scale 0.6
#define WORLD_display_buy_it_now__dlr__dlr__dlr_x -0.75
#define WORLD_display_buy_it_now__dlr__dlr__dlr_y -0.5

#define WORLD_display_buy_it_now_Now_scale WORLD_display_buy_it_now_Buy_scale
#define WORLD_display_buy_it_now_Now_x WORLD_display_buy_it_now_Buy_x+0.f
#define WORLD_display_buy_it_now_Now_y WORLD_display_buy_it_now_Buy_y-0.9f


#define WORLD_main_menu_virtual_scale 0.53
#define WORLD_main_menu_virtual_x -0.83f
#define WORLD_main_menu_virtual_y 0.15f


#define WORLD_main_menu_reality_scale 0.53
#define WORLD_main_menu_reality_x x+0.2*scale
#define WORLD_main_menu_reality_y y-scale-0.3

#define WORLD_UFOs_LAND_HERE__minus__gt_scale 1.3f
#define WORLD_UFOs_LAND_HERE__minus__gt_x -36.f
#define WORLD_UFOs_LAND_HERE__minus__gt_y 25.f
#define WORLD_UFOs_LAND_HERE__minus__gt_z -169.f

#define WORLD_display_top_menu_Dead_line_width 0.100f
#define WORLD_display_top_menu_Dead_scale 0.12f
#define WORLD_display_top_menu_Dead_x -1.20
#define WORLD_display_top_menu_Dead_y -0.30f

#define WORLD_CREATIVE_COMMONS_BASE_URL "https://cc.silentradiance.com/download.cgi/"
#define WORLD_display_buy_it_now_buynow "buynow"
#define WORLD_display_larry_harvey_hater_robots_e "e"
#define WORLD_display_larry_harvey_hater_robots_E "E"
#define WORLD_display_larry_harvey_hater_robots_n "n"
#define WORLD_display_larry_harvey_hater_robots_N "N"
#define WORLD_display_larry_harvey_hater_robots__plus_ "+"
#define WORLD_display_larry_harvey_hater_robots_s "s"
#define WORLD_display_larry_harvey_hater_robots_S "S"
#define WORLD_display_larry_harvey_hater_robots_w "w"
#define WORLD_display_larry_harvey_hater_robots_W "W"
#define WORLD_display_staff_ ""
#define WORLD_display_staff_0 "0"
#define WORLD_display_staff_10 "10"
#define WORLD_display_staff_1 "1"
#define WORLD_display_staff_11 "11"
#define WORLD_display_staff_12 "12"
#define WORLD_display_staff_13_plus_ "13+"
#define WORLD_display_staff_2 "2"
#define WORLD_display_staff_3 "3"
#define WORLD_display_staff_4 "4"
#define WORLD_display_staff_5 "5"
#define WORLD_display_staff_6 "6"
#define WORLD_display_staff_7 "7"
#define WORLD_display_staff_8 "8"
#define WORLD_display_staff_9 "9"
#define WORLD_display_staff__minus_1 "-1"
#define WORLD_display_triangle_none "none"
#define WORLD_display_triangle_test "test"

#define WORLD_download7_dot_cgi "download7.cgi"
#define WORLD_DUBSTEP_BASE_URL "https://ds.silentradiance.com/download.cgi/"
#define WORLD_GRATEFUL_DEAD_BASE_URL "https://gd.silentradiance.com/download.cgi/"
#define WORLD_HEAVY_METAL_BASE_URL "https://hm.silentradiance.com/download.cgi/"
#define WORLD_intros__pct_3_dot_3d_dot_png "intros_%3.3d.png"
#define WORLD_LANGUAGE_CODE "zh"
#define WORLD_PSYTRANCE_BASE_URL "https://pt.silentradiance.com/download.cgi/"
#define WORLD_PSYTRANCE_BASE_URL_NO_SLASH "https://pt.silentradiance.com/download.cgi"
#define WORLD_REGGAETON_BASE_URL "https://reggaeton.silentradiance.com/download.cgi/"

#define WORLD__slash_usr_slash_bin_slash_firefox__dqt__pct_s_dqt___amp__lf_ "/usr/bin/firefox \"%s\" &\n"
#define WORLD_SR000_BASE_URL "https://sr000.silentradiance.com/download.cgi/"
#define WORLD_SR002_BASE_URL "https://sr002.silentradiance.com/download.cgi/"
#define WORLD_store_a "a"
#define WORLD_STORE_BASE_URL "https://www.silentradiance.com"
#define WORLD_store_i "i"
#define WORLD_store_l "l"
#define WORLD_store_w "w"
#define WORLD_win64_network_receiver_8_dot_8_dot_8_dot_8 "8.8.8.8"


// optional
#define WORLD_A_SilentRadiance_app_SCALE 0.19f
#define WORLD_A_SilentRadiance_app_x -0.75f
#define WORLD_A_SilentRadiance_app_y -0.70f
