select consumable_id,
concat('https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=',consumable_id) buy_now_url,
filename,
ean,
artist,
label,
consumable_type,
consumable_name
from sr_songs_to_consumables_v where station_name='reggaeton.silentradiance.com'
and ean like 'pri%'
and the_error = ''
order by filename;



// https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=91995

MariaDB [sr]> select consumable_id,
    -> concat('https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=',consumable_id) buy_now_url,                       
    -> filename,
    -> ean,   
    -> artist,
    -> label,
    -> consumable_type,
    -> consumable_name
    -> from sr_songs_to_consumables_v where station_name='reggaeton.silentradiance.com'
    -> and ean like 'pri%'
    -> and the_error = '' 
    -> order by filename;
+---------------+-------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+-------------+---------------------+---------------+-----------------+------------------------------------------------------------------------------------------+
| consumable_id | buy_now_url                                                       | filename                                                                                                    | ean         | artist              | label         | consumable_type | consumable_name                                                                          |
+---------------+-------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+-------------+---------------------+---------------+-----------------+------------------------------------------------------------------------------------------+
|         91994 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=91994 | './Prime_Records/pricd001-Various-Los_Gladiators/01 - Drakote and Chezina - Xplozion.wav'                   | pricd001,01 | Drakote and Chezina | Prime Records | product         | Song: Drakote and Chezina - Xplozion - Los Gladiators 1 - Prime Records                  |
|         91995 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=91995 | './Prime_Records/pricd001-Various-Los_Gladiators/03 - El Drakote - Mal de Amores Explicit.wav'              | pricd001,03 | El Drakote          | Prime Records | product         | Song: El Drakote - Mal de Amores Explicit - Los Gladiators 3 - Prime Records             |
|         91996 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=91996 | './Prime_Records/pricd002-Falo-Ritmo_Professional_Explicit/01 - Falo - Pal Cruse.wav'                       | pricd002,01 | Falo                | Prime Records | product         | Song: Falo - Pal Cruse - Ritmo Professional Explicit 1 - Prime Records                   |
|         91997 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=91997 | './Prime_Records/pricd003-DJ_Negro_and_Vico_C-La_Recta_Final/01 - DJ Negro and Vico C - La Recta Final.wav' | pricd003,01 | DJ Negro and Vico C | Prime Records | product         | Song: DJ Negro and Vico C - La Recta Final - La Recta Final 1 - Prime Records            |
|         91998 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=91998 | './Prime_Records/pricd004-Various-Prime_Underground_3_Explicit/02 - Nana Ya Llego - La Nana.wav'            | pricd004,02 | Nana Ya Llego       | Prime Records | product         | Song: Nana Ya Llego - La Nana - Prime Underground 3 Explicit 2 - Prime Records           |
|         91999 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=91999 | './Prime_Records/pricd004-Various-Prime_Underground_3_Explicit/04 - Falo and Bam Bam - Extra.wav'           | pricd004,04 | Falo and Bam Bam    | Prime Records | product         | Song: Falo and Bam Bam - Extra - Prime Underground 3 Explicit 4 - Prime Records          |
|         92000 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92000 | './Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/02 - Vico C - Viernes 13.wav'                       | pricd005,02 | Vico C              | Prime Records | product         | Song: Vico C - Viernes 13 - Greatest Hits Explicit 2 - Prime Records                     |
|         92001 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92001 | './Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/03 - Vico C - Mundo Artificial.wav'                 | pricd005,03 | Vico C              | Prime Records | product         | Song: Vico C - Mundo Artificial - Greatest Hits Explicit 3 - Prime Records               |
|         92002 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92002 | './Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/05 - Vico C - La Inglesa.wav'                       | pricd005,05 | Vico C              | Prime Records | product         | Song: Vico C - La Inglesa - Greatest Hits Explicit 5 - Prime Records                     |
|         92003 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92003 | './Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/06 - Vico C - Xplosion.wav'                         | pricd005,06 | Vico C              | Prime Records | product         | Song: Vico C - Xplosion - Greatest Hits Explicit 6 - Prime Records                       |
|         92004 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92004 | './Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/07 - Vico C - Saborealo.wav'                        | pricd005,07 | Vico C              | Prime Records | product         | Song: Vico C - Saborealo - Greatest Hits Explicit 7 - Prime Records                      |
|         92005 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92005 | './Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/08 - Vico C - Maria.wav'                            | pricd005,08 | Vico C              | Prime Records | product         | Song: Vico C - Maria - Greatest Hits Explicit 8 - Prime Records                          |
|         92006 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92006 | './Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/09 - Vico C - Cut it Up Baron (Long Version).wav'   | pricd005,09 | Vico C              | Prime Records | product         | Song: Vico C - Cut it Up Baron (Long Version) - Greatest Hits Explicit 9 - Prime Records |
|         92007 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92007 | './Prime_Records/pricd006-Kid_Power_Posse-La_Medicina_Explicit/05 - Kid Power Posse - Medicina.wav'         | pricd006,05 | Kid Power Posse     | Prime Records | product         | Song: Kid Power Posse - Medicina - La Medicina Explicit 5 - Prime Records                |
|         92008 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92008 | './Prime_Records/pricd006-Kid_Power_Posse-La_Medicina_Explicit/06 - Kid Power Posse - El Gordo.wav'         | pricd006,06 | Kid Power Posse     | Prime Records | product         | Song: Kid Power Posse - El Gordo - La Medicina Explicit 6 - Prime Records                |
|         92009 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92009 | './Prime_Records/pricd007-El_General-Estas_Buena_Explicit/05 - El General - Te Ves Buena.wav'               | pricd007,05 | El General          | Prime Records | product         | Song: El General - Te Ves Buena - Estas Buena Explicit 5 - Prime Records                 |
|         92010 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92010 | './Prime_Records/pricd007-El_General-Estas_Buena_Explicit/06 - El General - Pu Tun Tun.wav'                 | pricd007,06 | El General          | Prime Records | product         | Song: El General - Pu Tun Tun - Estas Buena Explicit 6 - Prime Records                   |
|         92011 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92011 | './Prime_Records/pricd008-Lisa_M-No_Lo_Derrumbes/05 - Lisa M - Tu Pum Pum.wav'                              | pricd008,05 | Lisa M              | Prime Records | product         | Song: Lisa M - Tu Pum Pum - No Lo Derrumbes 5 - Prime Records                            |
|         92012 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92012 | './Prime_Records/pricd009-Falo-Colecta/07 - Falo - Colecta.wav'                                             | pricd009,07 | Falo                | Prime Records | product         | Song: Falo - Colecta - Colecta 7 - Prime Records                                         |
|         92013 | https://reggaeton.silentradiance.com/?t=buynow&g=yt6&l=es&c=92013 | './Prime_Records/pricd010-Various-Prime_Underground_1_Explicit/08 - Baby J - Me Dizzen.wav'                 | pricd010,08 | Baby J              | Prime Records | product         | Song: Baby J - Me Dizzen - Prime Underground 1 Explicit 8 - Prime Records                |
+---------------+-------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+-------------+---------------------+---------------+-----------------+------------------------------------------------------------------------------------------+
20 rows in set (0.74 sec)

