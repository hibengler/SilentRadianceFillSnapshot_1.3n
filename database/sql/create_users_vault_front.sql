
*/

use sr;
grant select on sr.sr_stations to 'vault_front'@'64.13.145.231' identified by 'somesecret';
grant select on sr.sr_hash_codes to 'vault_front'@'64.13.145.231' identified by 'somesecret';
grant select on sr.sr_hash_code_links to 'vault_front'@'64.13.145.231' identified by 'somesecret';
grant select on sr.sr_radio_plays to 'vault_front'@'64.13.145.231' identified by 'somesecret';
grant select on sr.sr_station_web_programs to 'vault_front'@'64.13.145.231' identified by 'somesecret';
grant select on sr.sr_song_info to 'vault_front'@'64.13.145.231' identified by 'somesecret';   
grant select on sr.sr_picture_info 'vault_front'@'64.13.145.231' identified by 'somesecret';   
grant select on sr.sr_song_info_consumables to 'vault_front'@'64.13.145.231' identified by 'somesecret';   
grant select on sr.sr_reflinks to 'vault_front'@'64.13.145.231' identified by 'somesecret';   

grant select,insert,update on sr_oauth2_tokens to 'vault_front'@'64.13.145.231' identified by 'somesecret';
grant select,insert,update on sr_oauth2_sessions to  'vault_front'@'64.13.145.231' identified by 'somesecret';

grant select,insert,update on sr_consumables to 'vault_front'@'64.13.145.231' identified by 'somesecret';
grant select,insert,update on sr_consumable_bill_of_materials to 'vault_front'@'64.13.145.231' identified by 'somesecret';
grant select,insert,update on sr_song_info_consumables to 'vault_front'@'64.13.145.231' identified by 'somesecret';

grant select,insert,update on sr_paypal_raw_transactions to 'vault_front'@'64.13.145.231' identified by 'somesecret';



/* sri */
grant select,insert,update,delete on sri_temp_locations to 'vault_front'@'64.13.145.231'identified by 'somesecret';
grant select,insert on sr_ip_addresses to 'vault_front'@'64.13.145.231'identified by 'somesecret';

grant select,insert,update on sri_temp_locations to 'vault_front'@'64.13.145.231'identified by 'somesecret';
grant select,insert,update on sri_raw_paypal_orders to 'vault_front'@'64.13.145.231'identified by 'somesecret';
grant select,insert,update on sri_raw_paypal_order_items to 'vault_front'@'64.13.145.231'identified by 'somesecret';
grant select,insert,update on sri_raw_order_fulfillment_items to 'vault_front'@'64.13.145.231'identified by 'somesecret';

flush privileges;

