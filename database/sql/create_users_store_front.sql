
*/

use sr;
grant select on sr.sr_stations to 'store_front'@'64.13.145.231' identified by 'othersecret';
grant select on sr.sr_hash_codes to 'store_front'@'64.13.145.231' identified by 'othersecret';
grant select on sr.sr_hash_code_links to 'store_front'@'64.13.145.231' identified by 'othersecret';
grant select on sr.sr_radio_plays to 'store_front'@'64.13.145.231' identified by 'othersecret';
grant select on sr.sr_station_web_programs to 'store_front'@'64.13.145.231' identified by 'othersecret';
grant select on sr.sr_song_info to 'store_front'@'64.13.145.231' identified by 'othersecret';   
grant select on sr.sr_picture_info 'store_front'@'64.13.145.231' identified by 'othersecret';   
grant select on sr.sr_song_info_consumables to 'store_front'@'64.13.145.231' identified by 'othersecret';   
grant select on sr.sr_reflinks to 'store_front'@'64.13.145.231' identified by 'othersecret';   

grant select,insert,update on sr_oauth2_tokens to 'store_front'@'64.13.145.231' identified by 'othersecret';
grant select,insert,update on sr_oauth2_sessions to  'store_front'@'64.13.145.231' identified by 'othersecret';

grant select,insert,update on sr_consumables to 'store_front'@'64.13.145.231' identified by 'othersecret';
grant select,insert,update on sr_consumable_bill_of_materials to 'store_front'@'64.13.145.231' identified by 'othersecret';
grant select,insert,update on sr_song_info_consumables to 'store_front'@'64.13.145.231' identified by 'othersecret';

grant select,insert,update on sr_paypal_raw_transactions to 'store_front'@'64.13.145.231' identified by 'othersecret';



/* sri */
grant select,insert,update,delete on sri_temp_locations to 'store_front'@'64.13.145.231'identified by 'othersecret';
grant select,insert on sr_ip_addresses to 'store_front'@'64.13.145.231'identified by 'othersecret';

flush privileges;

