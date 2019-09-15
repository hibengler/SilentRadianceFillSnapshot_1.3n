
/*
drop user  'order_fulfillment'@'64.13.145.243' ;
create user 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';  
*/

use sr;
grant select on sr.sr_songs_to_consumables_v to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_affiliations to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_artists to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_cd_artists to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_cd_track_artists to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_cd_tracks to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_cds to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_consumable_bill_of_materials  to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_consumable_types to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_consumables to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_emails to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_folders to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_picture_info to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_radio_plays to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select,insert on sr.sr_hash_codes to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select,insert on sr.sr_hash_code_links to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_radio_plays to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select,insert on sr.sr_station_web_programs to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_song_info to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';   
grant select on sr.sr_affiliation_actions to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';   
grant select on sr.sr_label_to_actions to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';   
grant select on sr.sr_actions to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';   

grant select,insert on sr.sr_song_info_consumables to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select,insert on sr.sr_tags to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select,insert on sr.sr_users to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';

grant show view  on sr.sr_songs_to_consumables_v  to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_songs_to_consumables_v  to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant create temporary table  on sr.'*'  to 'order_fulfillment'@'64.13.145.243' identified by 'secretcode';
grant select on sr.sr_international_standard_recording_codes  to 'order_fulfillment'@'64.13.145.243'
identified by 'secretcode';

# --v probably should be order_fulfillment user
grant select,insert,update on sr_paypal_raw_transactions to 'order_fulfillment'@'64.13.145.243' 
identified by 'secretcode';



grant select,insert,update on sri_raw_paypal_orders to 'order_fulfillment'@'64.13.145.243' 
identified by 'secretcode';
grant select,insert,update on sri_raw_paypal_order_items to 'order_fulfillment'@'64.13.145.243' 
identified by 'secretcode';
grant select,insert,update on sri_raw_order_fulfillment_items to 'order_fulfillment'@'64.13.145.243' 
identified by 'secretcode';
grant select,insert,update on sri_temp_locations to 'order_fulfillment'@'64.13.145.243' 
identified by 'secretcode';

/* AND 243 */

flush privileges;

