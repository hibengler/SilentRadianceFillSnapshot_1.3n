#include "world_internationalization_standard.h"



/* optional
#define WORLD_A_SilentRadianceWithVersion_r 0.9f
#define WORLD_A_SilentRadianceWithVersion_g 0.4f
#define WORLD_A_SilentRadianceWithVersion_b 1.f

#define WORLD_A_SilentRadiance_app_r 1.f
#define WORLD_A_SilentRadiance_app_g 0.3f
#define WORLD_A_SilentRadiance_app_b 0.8f
*/

// The name of the application. Started as Silent Radiance, but have different names:
// Radiance   in english like glow, warming
// Resplandor in spanish -  also glow
// also known as "Distance Disco"
// 
#define WORLD_Silent_Radiance "Silent Radiance"
#define WORLD_Silent_Radiance_With_Version_Splash_Title "Silent Radiance 1.3o"

// WORLD_A_SilentRadiance_app - is an optional second name that points to silentradiance.com
// useful when the app name is rebranded and we want someone to know the original
// not defined for english
//#define WORLD_A_SilentRadiance_app "(SilentRadiance.com)"

// Some visualizers detect if the song is 440 hz or 432 hz, and will put up this text when it is 432
#define WORLD__432_Namaste " 432 Namaste"


// This is a movie done with cute robots that go bo burning man, and bring back Larry Harvey from the dead
#define WORLD_Larry_qt_s_Possee_the_Robot_Haters "Larry's Possee the Robot Haters"
#define WORLD_Developed_by_Hipolito_Medrano "Developed by Hipolito Medrano"
#define WORLD_and_Hibbard_Engler "and Hibbard Engler"

#define WORLD_In_Rememberance_of "In Rememberance of"
#define WORLD_Larry_Harvey "Larry Harvey"

#define WORLD_UFOs_LAND_HERE__minus__gt_ "UFOs LAND HERE ->"

#define WORLD_Larry_comma__thanks_for_Burning_Man "Larry, thanks for Burning Man"
#define WORLD_And_comma__well_comma__thats_about_it "And, well, that's about it"
#define WORLD_We_came_to_be_because_of_you "We came to be because of you"
#define WORLD_And_brought_you_here "And brought you here"
#define WORLD_All_of_the_good_times_you "All of the good times you"
#define WORLD_keep_dot__Take_this_letter "keep. Take this letter"
#define WORLD_Thats_all_you_get "Thats all you get"
#define WORLD_Now_comma__lets_burn_something_excl_ "Now, lets burn something!"



// Next come the buttons for visualizers


// Sing along - like karaoke
#define WORLD_display_visual_voice__Sing " Sing"
#define WORLD_display_visual_voice_Along "Along"


// Window shop, - look at what is playing and also what was playing, and buy the song, or other stuff
//  Sometimes Wondow Show instead of Window Shop, when the music is free.
#define WORLD_display_window_shop_Window "Window"
#define WORLD_display_window_shop__dlr_hop "$hop"
#define WORLD_display_window_shop_Show "Show"


// Buy now - has no review, and takes the person to paypal to buy the song Right now - because its so good.
// Sometimes is Get now, when the song is free.
// Every beat has a flash of $$$ - dollar signs (or rubles, or euros, etc.)
#define WORLD_display_buy_it_now_Buy "Buy"
#define WORLD_display_buy_it_now_Get "Get"
#define WORLD_display_buy_it_now_Now "Now"
#define WORLD_display_buy_it_now__dlr__dlr__dlr_ "$$$"


// Disco info gives the song information in the corner
#define WORLD_display_words_Disco "Disco"
#define WORLD_display_words_Info "Info"


// Virtual Reality is another button.
#define WORLD_display_main_menu_Virtual "Virtual"
#define WORLD_display_main_menu_Reality "Reality"


// This is the name of a visualizer that shows the music notes
#define WORLD_display_staff_staff "staff"



// OK - now are the stations

// 666 - is a secret heavy metal station
#define WORLD_display_top_menu_666 "666"

// Dead is the grateful dead station
#define WORLD_display_top_menu_Dead "Dead"


//  This is my secret station
#define WORLD_display_top_menu_Hib "Hib"

// This is psytrance, but It is called Goa in China and India.
#define WORLD_display_top_menu_Psy "Psy"

// This is DubStep
#define WORLD_display_top_menu_DS "DS"

// This is Reggaeton from Puerto Rico
#define WORLD_display_top_menu_Rge "Rge"


// Some visuals will show the current note being played. These are music notes starting with A
#define WORLD_display_staff_A "A"
#define WORLD_display_staff_Bb "Bb"
#define WORLD_display_staff_B "B"
#define WORLD_display_staff_C "C"
#define WORLD_display_staff_C_hash_ "C#"
#define WORLD_display_staff_D "D"
#define WORLD_display_staff_Eb "Eb"
#define WORLD_display_staff_E "E"
#define WORLD_display_staff_F "F"
#define WORLD_display_staff_F_hash_ "F#"
#define WORLD_display_staff_G "G"
#define WORLD_display_staff_Ab "Ab"

#define WORLD_display_staff__minus_1 "-1"




// This is a warning for some phones where GPS interferes with the music broadcast
#define WORLD_GPS_WARNING "You should set Location Mode to GPS activity Only.\n\nWhy?\n\nBattery save and High accuracy modes use wifi.\nThis causes music to cut out every few seconds.\n\nEnabling/disabling location doesn't fix this because many devices survey wifi even if location is off.\n\nSome phones will work fine regardless.\n\nSilent Radiance does not use your location."

#define WORLD_GPS_GOTO_THE_SETTINGS = "Go to the Settings"

#define WORLD_GPS_ILL_TAKE_MY_CHANCES = "I'll take my chances"



// The rest is more technical - usually to be ignored


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

#define WORLD_display_triangle_none "none"
#define WORLD_display_triangle_test "test"
#define WORLD_download7_dot_cgi "download7.cgi"
#define WORLD_DUBSTEP_BASE_URL "https://ds.silentradiance.com/download.cgi/"
#define WORLD_GRATEFUL_DEAD_BASE_URL "https://gd.silentradiance.com/download.cgi/"
#define WORLD_HEAVY_METAL_BASE_URL "https://hm.silentradiance.com/download.cgi/"
#define WORLD_intros__pct_3_dot_3d_dot_png "intros_%3.3d.png"
#define WORLD_LANGUAGE_CODE "en"
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
#define WORLD_com_slash_killercool_slash_silentradiance_slash_WrapDb "com/killercool/silentradiance/WrapDb"


// optional
//#define WORLD_A_SilentRadiance_app_SCALE 0.19f
//#define WORLD_A_SilentRadiance_app_x -0.75f
//#define WORLD_A_SilentRadiance_app_y -0.70f





// Webstore -> translated into php by, well, something
#define WORLD_wstore_paypal_language_code "EN"
#define WORLD_wstore_paypal_webscr_url_country_prefix ""
#define WORLD_wstore_Connection_Failed "Connection Failed:"
#define WORLD_wstore_now_playing "Now Playing: "
#define WORLD_wstore_Previous_songs "Previous Songs:"
#define WORLD_wstore_Album "Album"
#define WORLD_wstore_Track "Track"
#define WORLD_wstore_0_Results "0 Results"
#define WORLD_wstore_Playing_ "Playing:"
#define WORLD_wstore_Time_second "second"
#define WORLD_wstore_Time_minute "minute"
#define WORLD_wstore_Time_hour "hour"
#define WORLD_wstore_Time_day "day"
#define WORLD_wstore_Time_week "week"
#define WORLD_wstore_Time_month "month"
#define WORLD_wstore_Time_year "year"
#define WORLD_wstore_Time_decade "decade"

#define WORLD_wstore_Time_seconds "seconds"
#define WORLD_wstore_Time_minutes "minutes"

#define WORLD_wstore_Time_hours "hours"
#define WORLD_wstore_Time_days "days"
#define WORLD_wstore_Time_weeks "weeks"
#define WORLD_wstore_Time_months "months"
#define WORLD_wstore_Time_years "years"
#define WORLD_wstore_Time_decades "decades"
#define WORLD_wstore_Tense_In "In"
#define WORLD_wstore_Buy_Song_for_font_size "8"
#define WORLD_wstore_Buy_Song_for "Buy Song <br>For"
#define WORLD_wstore_Make_payments_with_PayPal_ "Make payments with PayPal "
#define WORLD_wstore_Or_Buy_the_Album_for_font_size "7"
#define WORLD_wstore_Or_Buy_the_Album_for "or Buy the Album<br>For "
#define WORLD_wstore_Signed_CD_for_font_size "8"
#define WORLD_wstore_Signed_CD_for "Signed CD<br>For "
#define WORLD_wstore_Paypal_location_lang_location "en_US"  
