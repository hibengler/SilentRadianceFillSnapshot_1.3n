#include "world_internationalization_standard.h"


/* optional
#define WORLD_A_SilentRadianceWithVersion_r 0.9f
#define WORLD_A_SilentRadianceWithVersion_g 0.4f
#define WORLD_A_SilentRadianceWithVersion_b 1.f

#define WORLD_A_SilentRadiance_app_r 1.f
#define WORLD_A_SilentRadiance_app_g 0.3f
#define WORLD_A_SilentRadiance_app_b 0.8f
*/

// если вы просматриваете это, дайте мне знать, какие из них не так, и лучше исправить
// Английское название с миром вещи. Русское название в двойных кавычках.
// Слова с двумя косыми чертами (//) являются комментариями, которые помогут вам.
//
// if you are reviewing this, Let me know which ones are wrong, and a better correction
// The english name is with the WORLD stuff. The russian name is in the double quotes.
// The words with the two slashes (//) are comments to help guide you.


// Название приложения. Начинается как Silent Radiance, но имеют разные названия:
// Сияние на английском, как свечение, согревание
// Респландор на испанском - тоже светится
// также известный как «Дискотека»
//
// The name of the application. Started as Silent Radiance, but have different names:
// Radiance   in english like glow, warming
// Resplandor in spanish -  also glow
// also known as "Distance Disco"
// 
// Тихое сияние
// светимость
// полыхать
// Расстояние Диско
#define WORLD_Silent_Radiance "Диско Расстояние"
#define WORLD_Silent_Radiance_With_Version_Splash_Title "Диско Расстояние 1.3"
// WORLD_A_SilentRadiance_app - is an optional second name that points to silentradiance.com
// useful when the app name is rebranded and we want someone to know the original
// not defined for english
#define WORLD_A_SilentRadiance_app "(SilentRadiance.com)"

// Некоторые визуализаторы определяют, является ли песня 440 Гц или 432 Гц, и выводят этот текст, когда она составляет 432
//
// Some visualizers detect if the song is 440 hz or 432 hz, and will put up this text when it is 432
#define WORLD__432_Namaste " 432 намасте"

// Это фильм, снятый с милыми роботами, которые сжигают человека и возвращают Ларри Харви из мертвых
// Злой бог Ктулху приходит на космический корабль к северу от Пылающего Человека, оживает и разговаривает с Ларри в его новом теле робота.
//
// This is a movie done with cute robots that go bo burning man, and bring back Larry Harvey from the dead
// The evil god Cthulhu comes in a spaceship north of Burning Man, Revives, and talks to Larry in his new robot body.
#define WORLD_Larry_qt_s_Possee_the_Robot_Haters "Ларри Possee Робот ненавистников"
#define WORLD_Developed_by_Hipolito_Medrano "Разработано Иполито Медрано"
#define WORLD_and_Hibbard_Engler "и Хиббард Энглер"

#define WORLD_In_Rememberance_of "В память о мертвых"
#define WORLD_Larry_Harvey "Ларри Харви"

#define WORLD_UFOs_LAND_HERE__minus__gt_ "НЛО, КОТОРЫЕ СОДЕРЖАТСЯ CTHULHU ПОСАДКА ПОЛОСА ЗДЕСЬ ->"

#define WORLD_Larry_comma__thanks_for_Burning_Man "Ларри, спасибо за \"Пылающего Человека\"

#define WORLD_And_comma__well_comma__thats_about_it "и, ну вот и все"
#define WORLD_We_came_to_be_because_of_you "Мы стали существовать и процветать благодаря вам"
#define WORLD_And_brought_you_here "и мы оживили тебя здесь в будущем"
#define WORLD_All_of_the_good_times_you "Все хорошие воспоминания вы "
#define WORLD_keep_dot__Take_this_letter "храните. Возьми это письмо"
#define WORLD_Thats_all_you_get "Вот и все, что вы получаете"
#define WORLD_Now_comma__lets_burn_something_excl_ "Теперь давайте что-нибудь сжигаем!"


// Далее идут кнопки для визуализаторов
//
// Next come the buttons for visualizers

// Пой вместе - как караоке
//
// Sing along - like karaoke
#define WORLD_display_visual_voice__Sing " Синг"
#define WORLD_display_visual_voice_Along "Алонг"

// Витрина, - посмотрите, что играет, а также что играет, и купите песню, или другие вещи
// Иногда Wondow Show вместо Window Shop, когда музыка бесплатная.
//
// Window shop, - look at what is playing and also what was playing, and buy the song, or other stuff
//  Sometimes Wondow Show instead of Window Shop, when the music is free.
#define WORLD_display_window_shop_Window "Магазин"
#define WORLD_display_window_shop__dlr_hop "окон"
#define WORLD_display_window_shop_Show "окон"


// Купить сейчас - не имеет обзора и берет человека в PayPal, чтобы купить песню Прямо сейчас - когда песня такая хорошая
// Иногда Get Get, когда песня бесплатная.
// Каждый удар имеет вспышку $$$ - знаки доллара (или рубли, или евро и т. Д.)
//
// Buy now - has no review, and takes the person to paypal to buy the song Right now - when the song is so good.
// Sometimes is Get now, when the song is free.
// Every beat has a flash of $$$ - dollar signs (or rubles, or euros, etc.)
#define WORLD_display_buy_it_now_Buy "купить"
#define WORLD_display_buy_it_now_Get "Получить"
#define WORLD_display_buy_it_now_Now "сейчас"
#define WORLD_display_buy_it_now__dlr__dlr__dlr_ "₽₽₽"


// Информация о диско дает информацию о песне в углу
//
// Disco info gives the song information in the corner
#define WORLD_display_words_Disco "Диско"
#define WORLD_display_words_Info "Информация"

// Виртуальная реальность - это еще одна кнопка.
//
// Virtual Reality is another button.
#define WORLD_display_main_menu_Virtual "виртуальный"
#define WORLD_display_main_menu_Reality "реальность"

// Это имя визуализатора, который показывает ноты
//
// This is the name of a visualizer that shows the music notes
#define WORLD_display_staff_staff "staff"



// ОК - теперь станции
//
// OK - now are the stations




// 666 - секретная станция тяжелого метала
//
// 666 - is a secret heavy metal station
#define WORLD_display_top_menu_666 "666"

// Мертвая благодарная мертвая станция
//
// Dead is the grateful dead station
#define WORLD_display_top_menu_Dead "Dead"

// Это моя секретная станция
//
//  This is my secret station
#define WORLD_display_top_menu_Hib "Hib"

// Это психрант, но в Китае и Индии он называется Гоа
//
// This is psytrance, but It is called Goa in China and India.
#define WORLD_display_top_menu_Psy "Гоа"

// Это шаг дублирования
//
// This is DubStep
#define WORLD_display_top_menu_DS "DS"

// Это Реггетон из Пуэрто-Рико
//
// This is Reggaeton from Puerto Rico
#define WORLD_display_top_menu_Rge "Реггетон"


// Некоторые изображения показывают текущую музыкальную ноту, которую играют. Это ноты, начинающиеся с буквы A
//
// Some visuals will show the current note being played. These are music notes starting with A
#define WORLD_display_staff_A "ла"
#define WORLD_display_staff_Bb "си бемоль"
#define WORLD_display_staff_B "си"
#define WORLD_display_staff_C "до"
#define WORLD_display_staff_C_hash_ "до диез"
#define WORLD_display_staff_D "ре"
#define WORLD_display_staff_Eb "ми бемоль"
#define WORLD_display_staff_E "ми"
#define WORLD_display_staff_F "фа"
#define WORLD_display_staff_F_hash_ "фа диез"
#define WORLD_display_staff_G "сол"
#define WORLD_display_staff_Ab "ла бемоль"

#define WORLD_display_staff__minus_1 "-1"


// This is a warning for some phones where GPS interferes with the music broadcast
//#define WORLD_GPS_WARNING "You should set Location Mode to GPS activity Only.\n\nWhy?\n\nBattery save and High accuracy modes use wifi.\nThis causes music to cut out every few seconds.\n\nEnabling/disabling location doesn't fix this because many devices survey wifi even if location is off.\n\nSome phones will work fine regardless.\n\nSilent Radiance does not use your location."
#define WORLD_GPS_WARNING "Вам следует установить режим местоположения только для активности GPS.\n\nПочему?\n\nВ режимах сохранения и высокой точности батареи используется Wi-Fi.\nЭто приводит к отключению музыки каждые несколько секунд.\n\nВключение / отключение местоположения не устраняет Это связано с тем, что многие устройства проверяют Wi-Fi, даже если местоположение отключено.\n\nНекоторые телефоны будут работать нормально, независимо от того.Диско Расстояние не использует ваше местоположение."

//#define WORLD_GPS_GOTO_THE_SETTINGS = "Go to the Settings"
#define WORLD_GPS_GOTO_THE_SETTINGS = "Перейти в настройки"

//#define WORLD_GPS_ILL_TAKE_MY_CHANCES = "I'll take my chances"
#define WORLD_GPS_ILL_TAKE_MY_CHANCES = "Я рискну"




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

// Остальное более технично - обычно игнорируется
//
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


// optional
#define WORLD_A_SilentRadiance_app_SCALE 0.19f
#define WORLD_A_SilentRadiance_app_x -0.75f
#define WORLD_A_SilentRadiance_app_y -0.70f
