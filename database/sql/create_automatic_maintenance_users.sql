
/*
drop user  'automatic_maintenance'@'64.13.145.233' ;
create user 'automatic_maintenance'@'64.13.145.233' identified by 'somesecret';  
*/

use sr;
grant select on sr.sr_actionssr.sr_songs_to_consumables_v to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_affiliations to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_artists to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_cd_artists to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_cd_track_artists to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_cd_tracks to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_cds to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_consumable_bill_of_materials  to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_consumable_types to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_consumables to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_emails to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_folders to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_picture_info to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select on sr.sr_radio_plays to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert on sr.sr_hash_codes to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert on sr.sr_hash_code_links to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_radio_plays to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert on sr.sr_station_web_programs to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert,update on sr.sr_song_info to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';   
grant select on sr.sr_affiliation_actions to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';   
grant select on sr.sr_label_to_actions to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';   
grant select on sr.sr_actions to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';   

grant select,insert on sr.sr_song_info_consumables to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert on sr.sr_tags to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select,insert on sr.sr_users to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';

grant show view  on sr.sr_songs_to_consumables_v  to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select on sr.sr_songs_to_consumables_v  to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant create temporary table  on sr.'*'  to 'automatic_maintenance'@'64.13.145.243' identified by 'somesecret';
grant select on sr.sr_international_standard_recording_codes  to 'automatic_maintenance'@'64.13.145.243'
identified by 'somesecret';

# --v probably should be order_fulfillment user
grant select,insert,update on sr_paypal_raw_transactions to 'automatic_maintenance'@'64.13.145.231' 
identified by 'somesecret';


/* AND 231 */

flush privileges;

