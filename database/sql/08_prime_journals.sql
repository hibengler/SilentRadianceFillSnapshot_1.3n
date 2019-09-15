/* This is sprcial because we started journalling after the tables were loaded. 
So we have to prime the journals.
*/

update sr_names set last_time_checked=now();

update sr_ip_addresses
set ip_address=concat(ip_address,'');

update sr_name_ip_addresses
set ip_address=concat(ip_address,'');

update sr_users
set user_code=concat(user_code,'');

update sr_user_ip_addresses
set user_code=concat(user_code,'');


update sr_web_programs
set dns_name=concat(dns_name,'');


update sr_sessions
set session_code=concat(session_code,'');

update sr_paypal_raw_transactions
set status=status;





/* ^^^ from cretab.sql */




/* cretab_transmitter.sql:

*/

update sr_stations set station_name=station_name;
update sr_station_web_programs set station_name=station_name;
update sr_hash_codes set sha256=sha256;
update sr_hash_code_links set sha256=sha256;
update sr_picture_info set station_name=station_name;
update isrc_country_codes set country_code=country_code;


update isrc_years set year_code=year_code;

update isrc_entities set entity_id=entity_id;
update isrc_registrant_codes set country_code=country_code;
update isrc_registrant_years set country_code=country_code;

update sr_international_standard_recording_codes set country_code=country_code;
update sr_international_articles set ean=ean;

 
update sr_consumable_types set consumable_type=consumable_type;

update sr_affiliations set affiliation_code=affiliation_code;

update sr_actions set action_id=action_id;
update sr_affiliation_actions set action_id=action_id;

update sr_consumables set consumable_id=consumable_id;

update sr_cds set station_name=station_name;
update sr_folders set station_name=station_name;
update sr_artists set artist_code=artist_code; 
update sr_emails set email_address=email_address;

update sr_cd_tracks set station_name=station_name;
update sr_cd_track_artists set station_name=station_name;
update sr_cd_artists set station_name=station_name;
update sr_song_info set ean=ean;
/* sr_radio_plays is insert only */
update sr_song_info_consumables set station_name=station_name;


update sr_tags set tag=tag;
update sr_reflinks set reflink=reflink;
update sr_consumable_bill_of_materials set bill_id=bill_id;
update sr_label_to_affiliations set label=label;



/* thats it for transmitter */



/* now for ../sqlint/02_cretab.sql */
update sri_raw_paypal_orders set txn_id=txn_id;
update sri_raw_paypal_order_items set txn_id=txn_id;
update sri_raw_order_fulfillment_items set txn_id=txn_id;
update sri_temp_locations set location_id=location_id;

sr_oauth2_sessions
sr_oauth2_tokens
sr_reflinks
^^ no - but drop sr_radio_plays and the triggers;
sr_user_web_programs


/*cretab_u - done before inserts */
