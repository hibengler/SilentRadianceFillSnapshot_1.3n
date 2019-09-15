/*

select concat("call ddl_create_journal_table('sr','" ,TABLE_NAME,"')")
from information_schema.TABLES
where TABLE_SCHEMA='sr'
and TABLE_NAME not like '%_jn'
and TABLE_NAME not like '%_v'
and TABLE_NAME not in ('sr_radio_plays');

mysql -u root sr -s <a >b

call ddl_create_journal_table('sr','isrc_country_codes');
call ddl_create_journal_table('sr','isrc_entities');
call ddl_create_journal_table('sr','isrc_registrant_codes');
call ddl_create_journal_table('sr','isrc_registrant_years');
call ddl_create_journal_table('sr','isrc_years');
call ddl_create_journal_table('sr','sr_actions');
call ddl_create_journal_table('sr','sr_affiliation_actions');
call ddl_create_journal_table('sr','sr_affiliations');
call ddl_create_journal_table('sr','sr_artists');
call ddl_create_journal_table('sr','sr_cd_artists');
call ddl_create_journal_table('sr','sr_cd_track_artists');
call ddl_create_journal_table('sr','sr_cd_tracks');
call ddl_create_journal_table('sr','sr_cds');
call ddl_create_journal_table('sr','sr_consumable_bill_of_materials');
call ddl_create_journal_table('sr','sr_consumable_types');
call ddl_create_journal_table('sr','sr_consumables');
call ddl_create_journal_table('sr','sr_emails');
call ddl_create_journal_table('sr','sr_folders');
call ddl_create_journal_table('sr','sr_hash_code_links');
call ddl_create_journal_table('sr','sr_hash_codes');
call ddl_create_journal_table('sr','sr_international_articles');
call ddl_create_journal_table('sr','sr_international_standard_recording_codes');
call ddl_create_journal_table('sr','sr_ip_addresses');
call ddl_create_journal_table('sr','sr_label_to_affiliations');
call ddl_create_journal_table('sr','sr_name_ip_addresses');
call ddl_create_journal_table('sr','sr_names');
call ddl_create_journal_table('sr','sr_oauth2_sessions');
call ddl_create_journal_table('sr','sr_oauth2_tokens');
call ddl_create_journal_table('sr','sr_paypal_raw_transactions');
call ddl_create_journal_table('sr','sr_picture_info');
call ddl_create_journal_table('sr','sr_reflinks');
call ddl_create_journal_table('sr','sr_sessions');
call ddl_create_journal_table('sr','sr_song_info');
call ddl_create_journal_table('sr','sr_song_info_consumables');
call ddl_create_journal_table('sr','sr_songs_to_consumables_v');
call ddl_create_journal_table('sr','sr_station_web_programs');
call ddl_create_journal_table('sr','sr_stations');
call ddl_create_journal_table('sr','sr_tags');
call ddl_create_journal_table('sr','sr_user_ip_addresses');
call ddl_create_journal_table('sr','sr_user_web_programs');
call ddl_create_journal_table('sr','sr_users');
call ddl_create_journal_table('sr','sr_web_programs');
call ddl_create_journal_table('sr','sri_raw_order_fulfillment_items');
call ddl_create_journal_table('sr','sri_raw_paypal_order_items');
call ddl_create_journal_table('sr','sri_raw_paypal_orders');
call ddl_create_journal_table('sr','sri_temp_locations');

call ddl_create_journal_table('sr','sri_account_names');                     
call ddl_create_journal_table('sr','sri_accounts');                          
call ddl_create_journal_table('sr','sri_businesses');                        
call ddl_create_journal_table('sr','sri_currencies');                        
call ddl_create_journal_table('sr','sri_currency_conversion_rates');         
call ddl_create_journal_table('sr','sri_email_addresses');                   
call ddl_create_journal_table('sr','sri_entities');                          
call ddl_create_journal_table('sr','sri_entity_types');                      
call ddl_create_journal_table('sr','sri_paypal_accounts');                   
call ddl_create_journal_table('sr','sri_taxable_entities');                  
call ddl_create_journal_table('sr','sri_transactions');                      



Create table `isrc_country_codes_jn` (
jn_operation                    varchar(1)          NOT NULL,
jn_oracle_user                  varchar(30)         NOT NULL,
jn_datetime                     datetime  NOT NULL,
jn_endtime                     datetime not NULL,
jn_notes                        varchar(240)  , 
jn_appln                        varchar(30)   ,
jn_session                      int           
 ,`country_code` varchar(2)  not null
 ,`country_name` varchar(100) 
, constraint `isrc_country_codes_jn_pk` primary key (
`country_code`,jn_datetime)
, constraint `isrc_country_codes_jn_uk1` unique (
`country_code`,jn_endtime)
); 


builds them
*/
