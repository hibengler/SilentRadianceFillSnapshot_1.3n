#include "world_internationalization_standard.h"

/* optional
#define WORLD_A_SilentRadianceWithVersion_r 0.9f
#define WORLD_A_SilentRadianceWithVersion_g 0.4f
#define WORLD_A_SilentRadianceWithVersion_b 1.f

#define WORLD_A_SilentRadiance_app_r 1.f
#define WORLD_A_SilentRadiance_app_g 0.3f
#define WORLD_A_SilentRadiance_app_b 0.8f
*/

#define WORLD_SilentRadianceWithVersion_SCALE 0.19f
#define WORLD_SilentRadianceWithVersion_x -0.70f
#define WORLD_SilentRadianceWithVersion_y -0.30f

// optional
#define WORLD_A_SilentRadiance_app_SCALE 0.19f
#define WORLD_A_SilentRadiance_app_x -0.82f
#define WORLD_A_SilentRadiance_app_y -0.70f


// optional to change nova colors 

#define WORLD_default_nova_colorset_1 1.f,0.29411764705882352941f,0.23921568627450980392f,1.f
// normally 0.9f,0.9f,0.0f,1.f  , yellow
// but in this case, same as the Resplandor banner
#define WORLD_default_nova_colorset_2 0.2f,0.f,1.f,1.f
// normally 0.2f,0.f,1.f,1.f , blue slight purple


#define WORLD_display_visual_voice__Sing_scale 0.55
#define WORLD_display_visual_voice__Sing_x -0.87f
#define WORLD_display_visual_voice__Sing_y 0.15f
#define WORLD_display_visual_voice_Along_x -0.76f
#define WORLD_display_visual_voice_Along_y -0.70f

#define WORLD_display_words_Disco_line_width 0.100f
#define WORLD_display_words_Disco_scale 0.4f
#define WORLD_display_words_Disco_x (-0.87f)
#define WORLD_display_words_Disco_y (0.15f)

#define WORLD_display_words_Info_line_width 0.100f
#define WORLD_display_words_Info_scale 0.4f
#define WORLD_display_words_Info_x (WORLD_display_words_Disco_x+0.2 \
  * WORLD_display_words_Info_scale)
#define WORLD_display_words_Info_y (WORLD_display_words_Disco_y- \
  WORLD_display_words_Info_scale-0.3f)
  
  
#define WORLD_display_window_shop_Window_scale 0.4
#define WORLD_display_window_shop_Window_x (-0.75f)
#define WORLD_display_window_shop_Window_y (0.35f)
  
#define WORLD_display_window_shop_Show_scale 0.4  
#define WORLD_display_window_shop_Show_x \
  (WORLD_display_window_shop_Window_x-0.35f)
#define WORLD_display_window_shop_Show_y \
   (WORLD_display_window_shop_Window_y - 0.6f)


#define WORLD_display_window_shop__dlr_hop_x \
   (WORLD_display_window_shop_Window_x-0.35f) 
#define WORLD_display_window_shop__dlr_hop_y \
   (WORLD_display_window_shop_Window_y - 0.6f)

#define WORLD_display_buy_it_now_beat_width 0.160f
#define WORLD_display_buy_it_now_onset_width 0.130f
#define WORLD_display_buy_it_now_normal_width 0.128f


#define WORLD_display_buy_it_now_Buy_scale 0.42f
#define WORLD_display_buy_it_now_Buy_x -0.75f
#define WORLD_display_buy_it_now_Buy_y 0.13

#define WORLD_display_buy_it_now__dlr__dlr__dlr_scale 0.6
#define WORLD_display_buy_it_now__dlr__dlr__dlr_x -0.75
#define WORLD_display_buy_it_now__dlr__dlr__dlr_y -0.5

#define WORLD_display_buy_it_now_Now_scale WORLD_display_buy_it_now_Buy_scale
#define WORLD_display_buy_it_now_Now_x WORLD_display_buy_it_now_Buy_x+0.f
#define WORLD_display_buy_it_now_Now_y WORLD_display_buy_it_now_Buy_y-0.9f


#define WORLD_UFOs_LAND_HERE__minus__gt_scale 1.5f
#define WORLD_UFOs_LAND_HERE__minus__gt_x -69.f
#define WORLD_UFOs_LAND_HERE__minus__gt_y 25.f
#define WORLD_UFOs_LAND_HERE__minus__gt_z -169.f

#define WORLD_main_menu_virtual_scale 0.53
#define WORLD_main_menu_virtual_x -0.83f
#define WORLD_main_menu_virtual_y 0.15f

/* english:
#define WORLD_main_menu_reality_scale 0.53
#define WORLD_main_menu_reality_x x+0.2*scale
#define WORLD_main_menu_reality_y y-scale-0.3
*/

#define WORLD_main_menu_reality_scale 0.46
#define WORLD_main_menu_reality_x -0.85f
#define WORLD_main_menu_reality_y -0.55f


#define WORLD_display_top_menu_Dead_line_width 0.100f
#define WORLD_display_top_menu_Dead_scale 0.079f
#define WORLD_display_top_menu_Dead_x -1.74
#define WORLD_display_top_menu_Dead_y -0.40f

#define WORLD__432_Namaste " 432 Namaste"
#define WORLD_All_of_the_good_times_you "Todos los buenos tiempos que"
#define WORLD_And_brought_you_here "Y te traigo aqui"
#define WORLD_And_comma__well_comma__thats_about_it "Y bueno, eso es todo."
#define WORLD_and_Hibbard_Engler "Y Miguel Del Gato"
#define WORLD_CREATIVE_COMMONS_BASE_URL "https://cc.silentradiance.com/download.cgi/"
#define WORLD_Developed_by_Hipolito_Medrano "Desarrollado por Hipolito Medrano"
#define WORLD_display_buy_it_now_Buy "Comprar"
#define WORLD_display_buy_it_now_buynow "buynow"
#define WORLD_display_buy_it_now__dlr__dlr__dlr_ "$$$"
#define WORLD_display_buy_it_now_Get "Obtener"
#define WORLD_display_buy_it_now_Now "Ahora"
#define WORLD_display_larry_harvey_hater_robots_e "e"
#define WORLD_display_larry_harvey_hater_robots_E "E"
#define WORLD_display_larry_harvey_hater_robots_n "n"
#define WORLD_display_larry_harvey_hater_robots_N "N"
#define WORLD_display_larry_harvey_hater_robots__plus_ "+"
#define WORLD_display_larry_harvey_hater_robots_s "s"
#define WORLD_display_larry_harvey_hater_robots_S "S"
#define WORLD_display_larry_harvey_hater_robots_w "w"
#define WORLD_display_larry_harvey_hater_robots_W "W"
#define WORLD_display_main_menu_Reality "Realidad"
#define WORLD_display_main_menu_Virtual "Virtual"
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
#define WORLD_display_staff_A "A"
#define WORLD_display_staff_Ab "Ab"
#define WORLD_display_staff_B "B"
#define WORLD_display_staff_Bb "Bb"
#define WORLD_display_staff_C "C"
#define WORLD_display_staff_C_hash_ "C#"
#define WORLD_display_staff_D "D"
#define WORLD_display_staff_E "E"
#define WORLD_display_staff_F "F"
#define WORLD_display_staff_F_hash_ "F#"
#define WORLD_display_staff_G "G"
#define WORLD_display_staff__minus_1 "-1"
#define WORLD_display_staff_staff "Clave"
#define WORLD_display_top_menu_666 "666"
#define WORLD_display_top_menu_Dead "Muerte"
#define WORLD_display_top_menu_Hib "Hib"
#define WORLD_display_top_menu_Psy "Psy"
#define WORLD_display_top_menu_Rge "Rge"
#define WORLD_display_triangle_none "none"
#define WORLD_display_triangle_test "test"
#define WORLD_display_visual_voice_Along "Junto"
#define WORLD_display_visual_voice__Sing "Cantar"
#define WORLD_display_window_shop__dlr_hop "De tienda"
#define WORLD_display_window_shop_Show "Show"
#define WORLD_display_window_shop_Window "Vierda"
#define WORLD_display_words_Disco "Info de"
#define WORLD_display_words_Info "la Disco"
#define WORLD_download7_dot_cgi "download7.cgi"
#define WORLD_DUBSTEP_BASE_URL "https://ds.silentradiance.com/download.cgi/"
#define WORLD_GRATEFUL_DEAD_BASE_URL "https://gd.silentradiance.com/download.cgi/"
#define WORLD_HEAVY_METAL_BASE_URL "https://hm.silentradiance.com/download.cgi/"
#define WORLD_In_Rememberance_of "En recuerdo de"
#define WORLD_intros__pct_3_dot_3d_dot_png "intros_%3.3d.png"
#define WORLD_keep_dot__Take_this_letter "guardas. Toma esta carta."
#define WORLD_LANGUAGE_CODE "es"
#define WORLD_Larry_comma__thanks_for_Burning_Man "Larry, gracias por Burning Man"
#define WORLD_Larry_Harvey "Larry Harvey"
#define WORLD_Larry_qt_s_Possee_the_Robot_Haters "Larry Posse, los robots que odian"
#define WORLD_Now_comma__lets_burn_something_excl_ "Ahora vamos a quemar algo!"
#define WORLD_PSYTRANCE_BASE_URL "https://pt.silentradiance.com/download.cgi/"
#define WORLD_PSYTRANCE_BASE_URL_NO_SLASH "https://pt.silentradiance.com/download.cgi"
#define WORLD_REGGAETON_BASE_URL "https://reggaeton.silentradiance.com/download.cgi/"
#define WORLD_Silent_Radiance "Resplandor"
#define WORLD_Silent_Radiance_With_Version_Splash_Title "Resplandor 1.3o"
// WORLD_A_SilentRadiance_app - is an optional second name that points to silentradiance.com
// useful when the app name is rebranded and we want someone to know the original
#define WORLD_A_SilentRadiance_app "(SilentRadiance.com)"
#define WORLD__slash_usr_slash_bin_slash_firefox__dqt__pct_s_dqt___amp__lf_ "/usr/bin/firefox \"%s\" &\n"
#define WORLD_SR000_BASE_URL "https://sr000.silentradiance.com/download.cgi/"
#define WORLD_SR002_BASE_URL "https://sr002.silentradiance.com/download.cgi/"
#define WORLD_store_a "a"
#define WORLD_STORE_BASE_URL "https://www.silentradiance.com"
#define WORLD_store_i "i"
#define WORLD_store_l "l"
#define WORLD_store_w "w"
#define WORLD_Thats_all_you_get "Eso es todo lo que obtienes."
#define WORLD_UFOs_LAND_HERE__minus__gt_ "OVNI QUE CONTIENE LA TIERRA DE CTHULHU AQUI ->"
#define WORLD_We_came_to_be_because_of_you "Existimos y prosperamos gracias a ustedes."
#define WORLD_win64_network_receiver_8_dot_8_dot_8_dot_8 "8.8.8.8"
#define WORLD_com_slash_killercool_slash_silentradiance_slash_WrapDb "com/killercool/resplandor/WrapDb"





// This is a warning for some phones where GPS interferes with the music broadcast
#define WORLD_GPS_WARNING "Debería configurar el Modo de ubicación como actividad GPS solamente.\n\n¿Por qué?\n\nLos modos de guardar en la batería y de alta precisión usan wifi.\nEsto hace que la música se corte cada pocos segundos.\n\nEstablecimiento / deshabilitación de la ubicación no soluciona esto porque muchos dispositivos controlan wifi incluso si la ubicación está desactivada.\n\nAlgunos teléfonos funcionarán bien, independientemente.Resplandor (Silent Radiance) no usa su ubicación."

#define WORLD_GPS_GOTO_THE_SETTINGS = "Ir a la configuración"

#define WORLD_GPS_ILL_TAKE_MY_CHANCES = "voy a tomar mis posibilidades"




// this is for the store
#define WORLD_wstore_paypal_language_code "ES"
#define WORLD_wstore_paypal_webscr_url_country_prefix ""
#define WORLD_wstore_Connection_Failed "La conexiLa conexión fall: "
#define WORLD_wstore_now_playing "Jugando ahora: "
#define WORLD_wstore_Previous_songs "Canciones anteriores:"
#define WORLD_wstore_Album "Album"
#define WORLD_wstore_Track "Pista"
#define WORLD_wstore_0_Results "0 resultados"
#define WORLD_wstore_Playing_ "Reproduciendo:"
#define WORLD_wstore_Time_second "segundo"
#define WORLD_wstore_Time_minute "minuto"
#define WORLD_wstore_Time_hour "hora"
#define WORLD_wstore_Time_day "día"
#define WORLD_wstore_Time_week "semana"
#define WORLD_wstore_Time_month "mes"
#define WORLD_wstore_Time_year "año"
#define WORLD_wstore_Time_decade "década"

#define WORLD_wstore_Time_seconds "segundos"
#define WORLD_wstore_Time_minutes "minutos"

#define WORLD_wstore_Time_hours "horas"
#define WORLD_wstore_Time_days "dias"
#define WORLD_wstore_Time_weeks "semanas"
#define WORLD_wstore_Time_months "meses"
#define WORLD_wstore_Time_years "años"
#define WORLD_wstore_Time_decades "décadas"
#define WORLD_wstore_Tense_In "En"
#define WORLD_wstore_Buy_Song_for_font_size '8';
#define WORLD_wstore_Buy_Song_for "Comprar canción <br>por"
#define WORLD_wstore_Make_payments_with_PayPal_ 'Realiza pagos con PayPal, es rápido, gratis y seguro!';
#define WORLD_wstore_Or_Buy_the_Album_for_font_size '7';
#define WORLD_wstore_Or_Buy_the_Album_for 'o comprar el album para';
#define WORLD_wstore_Signed_CD_for_font_size '8';
#define WORLD_wstore_Signed_CD_for "CD firmado<br>por"
#define WORLD_wstore_Paypal_location_lang_location "en_US"  
  
  
  
