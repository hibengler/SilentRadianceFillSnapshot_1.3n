/*
select concat("call ddl_create_journal_trigger('sr','" ,TABLE_NAME,"');")
from information_schema.TABLES
where TABLE_SCHEMA='sr'
and TABLE_NAME not like '%_jn'
and TABLE_NAME not like '%_v'
and TABLE_NAME not in ('sr_radio_plays');

into a

mysql -u root sr -s <a >b
 sed 's/\\n/\n/g' <b >c
 
 This is c



call ddl_create_journal_trigger('sr','sri_account_names');                     
call ddl_create_journal_trigger('sr','sri_accounts');                          
call ddl_create_journal_trigger('sr','sri_businesses');                        
call ddl_create_journal_trigger('sr','sri_currencies');                        
call ddl_create_journal_trigger('sr','sri_currency_conversion_rates');         
call ddl_create_journal_trigger('sr','sri_email_addresses');                   
call ddl_create_journal_trigger('sr','sri_entities');                          
call ddl_create_journal_trigger('sr','sri_entity_types');                      
call ddl_create_journal_trigger('sr','sri_paypal_accounts');                   
call ddl_create_journal_trigger('sr','sri_raw_order_fulfillment_items');       
call ddl_create_journal_trigger('sr','sri_raw_paypal_order_items');            
call ddl_create_journal_trigger('sr','sri_raw_paypal_orders');                 
call ddl_create_journal_trigger('sr','sri_taxable_entities');                  
call ddl_create_journal_trigger('sr','sri_temp_locations');                    
call ddl_create_journal_trigger('sr','sri_transactions');      

*/


delimiter |
create trigger `isrc_country_codes_ji` after insert on `isrc_country_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_country_codes_jn` where `country_code`=new.`country_code`
  and jn_datetime = xdate;

update `isrc_country_codes_jn` set jn_endtime = xdate
where `country_code`=new.`country_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_country_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`country_name`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`country_code`
 ,new.`country_name`
);
end;|

create trigger `isrc_country_codes_ju` after update on `isrc_country_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_country_codes_jn` where `country_code`=old.`country_code`
  and jn_datetime = xdate;

update `isrc_country_codes_jn` set jn_endtime = xdate
where `country_code`=old.`country_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_country_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`country_name`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`country_code`
 ,new.`country_name`
);
end;|

create trigger `isrc_country_codes_jd` after delete on `isrc_country_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_country_codes_jn` where `country_code`=old.`country_code`
  and jn_datetime = xdate;

update `isrc_country_codes_jn` set jn_endtime = xdate
where `country_code`=old.`country_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_country_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`country_name`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`country_code`
 ,old.`country_name`
);
end;|
delimiter ;



delimiter |
create trigger `isrc_entities_ji` after insert on `isrc_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_entities_jn` where `entity_id`=new.`entity_id`
  and jn_datetime = xdate;

update `isrc_entities_jn` set jn_endtime = xdate
where `entity_id`=new.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`entity_name`
 ,`address`
 ,`other_details`
 ,`department`
 ,`person_name`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`entity_id`
 ,new.`entity_name`
 ,new.`address`
 ,new.`other_details`
 ,new.`department`
 ,new.`person_name`
);
end;|

create trigger `isrc_entities_ju` after update on `isrc_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_entities_jn` where `entity_id`=old.`entity_id`
  and jn_datetime = xdate;

update `isrc_entities_jn` set jn_endtime = xdate
where `entity_id`=old.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`entity_name`
 ,`address`
 ,`other_details`
 ,`department`
 ,`person_name`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`entity_id`
 ,new.`entity_name`
 ,new.`address`
 ,new.`other_details`
 ,new.`department`
 ,new.`person_name`
);
end;|

create trigger `isrc_entities_jd` after delete on `isrc_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_entities_jn` where `entity_id`=old.`entity_id`
  and jn_datetime = xdate;

update `isrc_entities_jn` set jn_endtime = xdate
where `entity_id`=old.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`entity_name`
 ,`address`
 ,`other_details`
 ,`department`
 ,`person_name`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`entity_id`
 ,old.`entity_name`
 ,old.`address`
 ,old.`other_details`
 ,old.`department`
 ,old.`person_name`
);
end;|
delimiter ;



delimiter |
create trigger `isrc_registrant_codes_ji` after insert on `isrc_registrant_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_registrant_codes_jn` where `country_code`=new.`country_code`
  and `registrant_code`=new.`registrant_code`
  and jn_datetime = xdate;

update `isrc_registrant_codes_jn` set jn_endtime = xdate
where `country_code`=new.`country_code`
  and `registrant_code`=new.`registrant_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_registrant_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`registrant_code`
 ,`registrant_id`
 ,`date_registrant_code_issused`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`country_code`
 ,new.`registrant_code`
 ,new.`registrant_id`
 ,new.`date_registrant_code_issused`
);
end;|

create trigger `isrc_registrant_codes_ju` after update on `isrc_registrant_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_registrant_codes_jn` where `country_code`=old.`country_code`
  and `registrant_code`=old.`registrant_code`
  and jn_datetime = xdate;

update `isrc_registrant_codes_jn` set jn_endtime = xdate
where `country_code`=old.`country_code`
  and `registrant_code`=old.`registrant_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_registrant_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`registrant_code`
 ,`registrant_id`
 ,`date_registrant_code_issused`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`country_code`
 ,new.`registrant_code`
 ,new.`registrant_id`
 ,new.`date_registrant_code_issused`
);
end;|

create trigger `isrc_registrant_codes_jd` after delete on `isrc_registrant_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_registrant_codes_jn` where `country_code`=old.`country_code`
  and `registrant_code`=old.`registrant_code`
  and jn_datetime = xdate;

update `isrc_registrant_codes_jn` set jn_endtime = xdate
where `country_code`=old.`country_code`
  and `registrant_code`=old.`registrant_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_registrant_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`registrant_code`
 ,`registrant_id`
 ,`date_registrant_code_issused`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`country_code`
 ,old.`registrant_code`
 ,old.`registrant_id`
 ,old.`date_registrant_code_issused`
);
end;|
delimiter ;



delimiter |
create trigger `isrc_registrant_years_ji` after insert on `isrc_registrant_years` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_registrant_years_jn` where `country_code`=new.`country_code`
  and `registrant_code`=new.`registrant_code`
  and `registrant_year`=new.`registrant_year`
  and jn_datetime = xdate;

update `isrc_registrant_years_jn` set jn_endtime = xdate
where `country_code`=new.`country_code`
  and `registrant_code`=new.`registrant_code`
  and `registrant_year`=new.`registrant_year`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_registrant_years_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`registrant_code`
 ,`registrant_year`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`country_code`
 ,new.`registrant_code`
 ,new.`registrant_year`
);
end;|

create trigger `isrc_registrant_years_ju` after update on `isrc_registrant_years` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_registrant_years_jn` where `country_code`=old.`country_code`
  and `registrant_code`=old.`registrant_code`
  and `registrant_year`=old.`registrant_year`
  and jn_datetime = xdate;

update `isrc_registrant_years_jn` set jn_endtime = xdate
where `country_code`=old.`country_code`
  and `registrant_code`=old.`registrant_code`
  and `registrant_year`=old.`registrant_year`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_registrant_years_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`registrant_code`
 ,`registrant_year`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`country_code`
 ,new.`registrant_code`
 ,new.`registrant_year`
);
end;|

create trigger `isrc_registrant_years_jd` after delete on `isrc_registrant_years` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_registrant_years_jn` where `country_code`=old.`country_code`
  and `registrant_code`=old.`registrant_code`
  and `registrant_year`=old.`registrant_year`
  and jn_datetime = xdate;

update `isrc_registrant_years_jn` set jn_endtime = xdate
where `country_code`=old.`country_code`
  and `registrant_code`=old.`registrant_code`
  and `registrant_year`=old.`registrant_year`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_registrant_years_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`country_code`
 ,`registrant_code`
 ,`registrant_year`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`country_code`
 ,old.`registrant_code`
 ,old.`registrant_year`
);
end;|
delimiter ;



delimiter |
create trigger `isrc_years_ji` after insert on `isrc_years` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_years_jn` where `year_code`=new.`year_code`
  and jn_datetime = xdate;

update `isrc_years_jn` set jn_endtime = xdate
where `year_code`=new.`year_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_years_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`year_code`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`year_code`
);
end;|

create trigger `isrc_years_ju` after update on `isrc_years` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_years_jn` where `year_code`=old.`year_code`
  and jn_datetime = xdate;

update `isrc_years_jn` set jn_endtime = xdate
where `year_code`=old.`year_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_years_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`year_code`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`year_code`
);
end;|

create trigger `isrc_years_jd` after delete on `isrc_years` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `isrc_years_jn` where `year_code`=old.`year_code`
  and jn_datetime = xdate;

update `isrc_years_jn` set jn_endtime = xdate
where `year_code`=old.`year_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `isrc_years_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`year_code`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`year_code`
);
end;|
delimiter ;



delimiter |
create trigger `sr_actions_ji` after insert on `sr_actions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_actions_jn` where `action_id`=new.`action_id`
  and jn_datetime = xdate;

update `sr_actions_jn` set jn_endtime = xdate
where `action_id`=new.`action_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_actions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`action_id`
 ,`action_name`
 ,`description`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`action_id`
 ,new.`action_name`
 ,new.`description`
);
end;|

create trigger `sr_actions_ju` after update on `sr_actions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_actions_jn` where `action_id`=old.`action_id`
  and jn_datetime = xdate;

update `sr_actions_jn` set jn_endtime = xdate
where `action_id`=old.`action_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_actions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`action_id`
 ,`action_name`
 ,`description`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`action_id`
 ,new.`action_name`
 ,new.`description`
);
end;|

create trigger `sr_actions_jd` after delete on `sr_actions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_actions_jn` where `action_id`=old.`action_id`
  and jn_datetime = xdate;

update `sr_actions_jn` set jn_endtime = xdate
where `action_id`=old.`action_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_actions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`action_id`
 ,`action_name`
 ,`description`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`action_id`
 ,old.`action_name`
 ,old.`description`
);
end;|
delimiter ;



delimiter |
create trigger `sr_affiliation_actions_ji` after insert on `sr_affiliation_actions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_affiliation_actions_jn` where `affiliation_code`=new.`affiliation_code`
  and `action_id`=new.`action_id`
  and jn_datetime = xdate;

update `sr_affiliation_actions_jn` set jn_endtime = xdate
where `affiliation_code`=new.`affiliation_code`
  and `action_id`=new.`action_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_affiliation_actions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`affiliation_code`
 ,`action_id`
 ,`default_price`
 ,`default_kickback`
 ,`default_kickback_percentage`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`affiliation_code`
 ,new.`action_id`
 ,new.`default_price`
 ,new.`default_kickback`
 ,new.`default_kickback_percentage`
);
end;|

create trigger `sr_affiliation_actions_ju` after update on `sr_affiliation_actions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_affiliation_actions_jn` where `affiliation_code`=old.`affiliation_code`
  and `action_id`=old.`action_id`
  and jn_datetime = xdate;

update `sr_affiliation_actions_jn` set jn_endtime = xdate
where `affiliation_code`=old.`affiliation_code`
  and `action_id`=old.`action_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_affiliation_actions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`affiliation_code`
 ,`action_id`
 ,`default_price`
 ,`default_kickback`
 ,`default_kickback_percentage`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`affiliation_code`
 ,new.`action_id`
 ,new.`default_price`
 ,new.`default_kickback`
 ,new.`default_kickback_percentage`
);
end;|

create trigger `sr_affiliation_actions_jd` after delete on `sr_affiliation_actions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_affiliation_actions_jn` where `affiliation_code`=old.`affiliation_code`
  and `action_id`=old.`action_id`
  and jn_datetime = xdate;

update `sr_affiliation_actions_jn` set jn_endtime = xdate
where `affiliation_code`=old.`affiliation_code`
  and `action_id`=old.`action_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_affiliation_actions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`affiliation_code`
 ,`action_id`
 ,`default_price`
 ,`default_kickback`
 ,`default_kickback_percentage`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`affiliation_code`
 ,old.`action_id`
 ,old.`default_price`
 ,old.`default_kickback`
 ,old.`default_kickback_percentage`
);
end;|
delimiter ;



delimiter |
create trigger `sr_affiliations_ji` after insert on `sr_affiliations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_affiliations_jn` where `affiliation_code`=new.`affiliation_code`
  and jn_datetime = xdate;

update `sr_affiliations_jn` set jn_endtime = xdate
where `affiliation_code`=new.`affiliation_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_affiliations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`affiliation_code`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`affiliation_code`
);
end;|

create trigger `sr_affiliations_ju` after update on `sr_affiliations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_affiliations_jn` where `affiliation_code`=old.`affiliation_code`
  and jn_datetime = xdate;

update `sr_affiliations_jn` set jn_endtime = xdate
where `affiliation_code`=old.`affiliation_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_affiliations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`affiliation_code`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`affiliation_code`
);
end;|

create trigger `sr_affiliations_jd` after delete on `sr_affiliations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_affiliations_jn` where `affiliation_code`=old.`affiliation_code`
  and jn_datetime = xdate;

update `sr_affiliations_jn` set jn_endtime = xdate
where `affiliation_code`=old.`affiliation_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_affiliations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`affiliation_code`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`affiliation_code`
);
end;|
delimiter ;



delimiter |
create trigger `sr_artists_ji` after insert on `sr_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_artists_jn` where `artist_id`=new.`artist_id`
  and jn_datetime = xdate;

update `sr_artists_jn` set jn_endtime = xdate
where `artist_id`=new.`artist_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`artist_id`
 ,`artist_code`
 ,`group_id`
 ,`group_sequence`
 ,`artist_name`
 ,`shoutout_available`
 ,`shoutout_price`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`artist_id`
 ,new.`artist_code`
 ,new.`group_id`
 ,new.`group_sequence`
 ,new.`artist_name`
 ,new.`shoutout_available`
 ,new.`shoutout_price`
);
end;|

create trigger `sr_artists_ju` after update on `sr_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_artists_jn` where `artist_id`=old.`artist_id`
  and jn_datetime = xdate;

update `sr_artists_jn` set jn_endtime = xdate
where `artist_id`=old.`artist_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`artist_id`
 ,`artist_code`
 ,`group_id`
 ,`group_sequence`
 ,`artist_name`
 ,`shoutout_available`
 ,`shoutout_price`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`artist_id`
 ,new.`artist_code`
 ,new.`group_id`
 ,new.`group_sequence`
 ,new.`artist_name`
 ,new.`shoutout_available`
 ,new.`shoutout_price`
);
end;|

create trigger `sr_artists_jd` after delete on `sr_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_artists_jn` where `artist_id`=old.`artist_id`
  and jn_datetime = xdate;

update `sr_artists_jn` set jn_endtime = xdate
where `artist_id`=old.`artist_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`artist_id`
 ,`artist_code`
 ,`group_id`
 ,`group_sequence`
 ,`artist_name`
 ,`shoutout_available`
 ,`shoutout_price`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`artist_id`
 ,old.`artist_code`
 ,old.`group_id`
 ,old.`group_sequence`
 ,old.`artist_name`
 ,old.`shoutout_available`
 ,old.`shoutout_price`
);
end;|
delimiter ;



delimiter |
create trigger `sr_cd_artists_ji` after insert on `sr_cd_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_artists_jn` where `station_name`=new.`station_name`
  and `cd_short_name`=new.`cd_short_name`
  and `sequence`=new.`sequence`
  and jn_datetime = xdate;

update `sr_cd_artists_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and `cd_short_name`=new.`cd_short_name`
  and `sequence`=new.`sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`sequence`
 ,`type`
 ,`artist_id`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`sequence`
 ,new.`type`
 ,new.`artist_id`
);
end;|

create trigger `sr_cd_artists_ju` after update on `sr_cd_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_artists_jn` where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `sequence`=old.`sequence`
  and jn_datetime = xdate;

update `sr_cd_artists_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `sequence`=old.`sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`sequence`
 ,`type`
 ,`artist_id`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`sequence`
 ,new.`type`
 ,new.`artist_id`
);
end;|

create trigger `sr_cd_artists_jd` after delete on `sr_cd_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_artists_jn` where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `sequence`=old.`sequence`
  and jn_datetime = xdate;

update `sr_cd_artists_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `sequence`=old.`sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`sequence`
 ,`type`
 ,`artist_id`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`cd_short_name`
 ,old.`sequence`
 ,old.`type`
 ,old.`artist_id`
);
end;|
delimiter ;



delimiter |
create trigger `sr_cd_track_artists_ji` after insert on `sr_cd_track_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_track_artists_jn` where `station_name`=new.`station_name`
  and `cd_short_name`=new.`cd_short_name`
  and `track`=new.`track`
  and `sequence`=new.`sequence`
  and jn_datetime = xdate;

update `sr_cd_track_artists_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and `cd_short_name`=new.`cd_short_name`
  and `track`=new.`track`
  and `sequence`=new.`sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_track_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`track`
 ,`sequence`
 ,`type`
 ,`artist_id`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`track`
 ,new.`sequence`
 ,new.`type`
 ,new.`artist_id`
);
end;|

create trigger `sr_cd_track_artists_ju` after update on `sr_cd_track_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_track_artists_jn` where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `track`=old.`track`
  and `sequence`=old.`sequence`
  and jn_datetime = xdate;

update `sr_cd_track_artists_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `track`=old.`track`
  and `sequence`=old.`sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_track_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`track`
 ,`sequence`
 ,`type`
 ,`artist_id`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`track`
 ,new.`sequence`
 ,new.`type`
 ,new.`artist_id`
);
end;|

create trigger `sr_cd_track_artists_jd` after delete on `sr_cd_track_artists` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_track_artists_jn` where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `track`=old.`track`
  and `sequence`=old.`sequence`
  and jn_datetime = xdate;

update `sr_cd_track_artists_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `track`=old.`track`
  and `sequence`=old.`sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_track_artists_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`track`
 ,`sequence`
 ,`type`
 ,`artist_id`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`cd_short_name`
 ,old.`track`
 ,old.`sequence`
 ,old.`type`
 ,old.`artist_id`
);
end;|
delimiter ;



delimiter |
create trigger `sr_cd_tracks_ji` after insert on `sr_cd_tracks` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_tracks_jn` where `station_name`=new.`station_name`
  and `cd_short_name`=new.`cd_short_name`
  and `track`=new.`track`
  and jn_datetime = xdate;

update `sr_cd_tracks_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and `cd_short_name`=new.`cd_short_name`
  and `track`=new.`track`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_tracks_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`track`
 ,`raw_track_text`
 ,`title`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`track`
 ,new.`raw_track_text`
 ,new.`title`
);
end;|

create trigger `sr_cd_tracks_ju` after update on `sr_cd_tracks` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_tracks_jn` where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `track`=old.`track`
  and jn_datetime = xdate;

update `sr_cd_tracks_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `track`=old.`track`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_tracks_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`track`
 ,`raw_track_text`
 ,`title`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`track`
 ,new.`raw_track_text`
 ,new.`title`
);
end;|

create trigger `sr_cd_tracks_jd` after delete on `sr_cd_tracks` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cd_tracks_jn` where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `track`=old.`track`
  and jn_datetime = xdate;

update `sr_cd_tracks_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and `track`=old.`track`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cd_tracks_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`track`
 ,`raw_track_text`
 ,`title`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`cd_short_name`
 ,old.`track`
 ,old.`raw_track_text`
 ,old.`title`
);
end;|
delimiter ;



delimiter |
create trigger `sr_cds_ji` after insert on `sr_cds` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cds_jn` where `station_name`=new.`station_name`
  and `cd_short_name`=new.`cd_short_name`
  and jn_datetime = xdate;

update `sr_cds_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and `cd_short_name`=new.`cd_short_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cds_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`cmd`
 ,`bn`
 ,`business`
 ,`item_name`
 ,`item_number`
 ,`amount`
 ,`currency_code`
 ,`shipping`
 ,`shipping2`
 ,`image_url`
 ,`imagehead_url`
 ,`undefined_quantity`
 ,`receiver_email`
 ,`mrb`
 ,`pal`
 ,`no_shipping`
 ,`no_note`
 ,`title`
 ,`artist`
 ,`label`
 ,`catalogue`
 ,`format`
 ,`barcode`
 ,`the_release`
 ,`demo_video_url`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`cmd`
 ,new.`bn`
 ,new.`business`
 ,new.`item_name`
 ,new.`item_number`
 ,new.`amount`
 ,new.`currency_code`
 ,new.`shipping`
 ,new.`shipping2`
 ,new.`image_url`
 ,new.`imagehead_url`
 ,new.`undefined_quantity`
 ,new.`receiver_email`
 ,new.`mrb`
 ,new.`pal`
 ,new.`no_shipping`
 ,new.`no_note`
 ,new.`title`
 ,new.`artist`
 ,new.`label`
 ,new.`catalogue`
 ,new.`format`
 ,new.`barcode`
 ,new.`the_release`
 ,new.`demo_video_url`
);
end;|

create trigger `sr_cds_ju` after update on `sr_cds` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cds_jn` where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and jn_datetime = xdate;

update `sr_cds_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cds_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`cmd`
 ,`bn`
 ,`business`
 ,`item_name`
 ,`item_number`
 ,`amount`
 ,`currency_code`
 ,`shipping`
 ,`shipping2`
 ,`image_url`
 ,`imagehead_url`
 ,`undefined_quantity`
 ,`receiver_email`
 ,`mrb`
 ,`pal`
 ,`no_shipping`
 ,`no_note`
 ,`title`
 ,`artist`
 ,`label`
 ,`catalogue`
 ,`format`
 ,`barcode`
 ,`the_release`
 ,`demo_video_url`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`cmd`
 ,new.`bn`
 ,new.`business`
 ,new.`item_name`
 ,new.`item_number`
 ,new.`amount`
 ,new.`currency_code`
 ,new.`shipping`
 ,new.`shipping2`
 ,new.`image_url`
 ,new.`imagehead_url`
 ,new.`undefined_quantity`
 ,new.`receiver_email`
 ,new.`mrb`
 ,new.`pal`
 ,new.`no_shipping`
 ,new.`no_note`
 ,new.`title`
 ,new.`artist`
 ,new.`label`
 ,new.`catalogue`
 ,new.`format`
 ,new.`barcode`
 ,new.`the_release`
 ,new.`demo_video_url`
);
end;|

create trigger `sr_cds_jd` after delete on `sr_cds` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_cds_jn` where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and jn_datetime = xdate;

update `sr_cds_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `cd_short_name`=old.`cd_short_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_cds_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`cmd`
 ,`bn`
 ,`business`
 ,`item_name`
 ,`item_number`
 ,`amount`
 ,`currency_code`
 ,`shipping`
 ,`shipping2`
 ,`image_url`
 ,`imagehead_url`
 ,`undefined_quantity`
 ,`receiver_email`
 ,`mrb`
 ,`pal`
 ,`no_shipping`
 ,`no_note`
 ,`title`
 ,`artist`
 ,`label`
 ,`catalogue`
 ,`format`
 ,`barcode`
 ,`the_release`
 ,`demo_video_url`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`cd_short_name`
 ,old.`cmd`
 ,old.`bn`
 ,old.`business`
 ,old.`item_name`
 ,old.`item_number`
 ,old.`amount`
 ,old.`currency_code`
 ,old.`shipping`
 ,old.`shipping2`
 ,old.`image_url`
 ,old.`imagehead_url`
 ,old.`undefined_quantity`
 ,old.`receiver_email`
 ,old.`mrb`
 ,old.`pal`
 ,old.`no_shipping`
 ,old.`no_note`
 ,old.`title`
 ,old.`artist`
 ,old.`label`
 ,old.`catalogue`
 ,old.`format`
 ,old.`barcode`
 ,old.`the_release`
 ,old.`demo_video_url`
);
end;|
delimiter ;



delimiter |
create trigger `sr_consumable_bill_of_materials_ji` after insert on `sr_consumable_bill_of_materials` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumable_bill_of_materials_jn` where `bill_id`=new.`bill_id`
  and jn_datetime = xdate;

update `sr_consumable_bill_of_materials_jn` set jn_endtime = xdate
where `bill_id`=new.`bill_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumable_bill_of_materials_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`bill_id`
 ,`used_action_sequence`
 ,`listed_action_sequence`
 ,`template_bill_id`
 ,`parent_consumable_id`
 ,`action_id`
 ,`child_consumable_id`
 ,`description`
 ,`technical_notes`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`bill_id`
 ,new.`used_action_sequence`
 ,new.`listed_action_sequence`
 ,new.`template_bill_id`
 ,new.`parent_consumable_id`
 ,new.`action_id`
 ,new.`child_consumable_id`
 ,new.`description`
 ,new.`technical_notes`
);
end;|

create trigger `sr_consumable_bill_of_materials_ju` after update on `sr_consumable_bill_of_materials` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumable_bill_of_materials_jn` where `bill_id`=old.`bill_id`
  and jn_datetime = xdate;

update `sr_consumable_bill_of_materials_jn` set jn_endtime = xdate
where `bill_id`=old.`bill_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumable_bill_of_materials_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`bill_id`
 ,`used_action_sequence`
 ,`listed_action_sequence`
 ,`template_bill_id`
 ,`parent_consumable_id`
 ,`action_id`
 ,`child_consumable_id`
 ,`description`
 ,`technical_notes`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`bill_id`
 ,new.`used_action_sequence`
 ,new.`listed_action_sequence`
 ,new.`template_bill_id`
 ,new.`parent_consumable_id`
 ,new.`action_id`
 ,new.`child_consumable_id`
 ,new.`description`
 ,new.`technical_notes`
);
end;|

create trigger `sr_consumable_bill_of_materials_jd` after delete on `sr_consumable_bill_of_materials` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumable_bill_of_materials_jn` where `bill_id`=old.`bill_id`
  and jn_datetime = xdate;

update `sr_consumable_bill_of_materials_jn` set jn_endtime = xdate
where `bill_id`=old.`bill_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumable_bill_of_materials_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`bill_id`
 ,`used_action_sequence`
 ,`listed_action_sequence`
 ,`template_bill_id`
 ,`parent_consumable_id`
 ,`action_id`
 ,`child_consumable_id`
 ,`description`
 ,`technical_notes`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`bill_id`
 ,old.`used_action_sequence`
 ,old.`listed_action_sequence`
 ,old.`template_bill_id`
 ,old.`parent_consumable_id`
 ,old.`action_id`
 ,old.`child_consumable_id`
 ,old.`description`
 ,old.`technical_notes`
);
end;|
delimiter ;



delimiter |
create trigger `sr_consumable_types_ji` after insert on `sr_consumable_types` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumable_types_jn` where `consumable_type`=new.`consumable_type`
  and jn_datetime = xdate;

update `sr_consumable_types_jn` set jn_endtime = xdate
where `consumable_type`=new.`consumable_type`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumable_types_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`consumable_type`
 ,`description`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`consumable_type`
 ,new.`description`
);
end;|

create trigger `sr_consumable_types_ju` after update on `sr_consumable_types` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumable_types_jn` where `consumable_type`=old.`consumable_type`
  and jn_datetime = xdate;

update `sr_consumable_types_jn` set jn_endtime = xdate
where `consumable_type`=old.`consumable_type`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumable_types_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`consumable_type`
 ,`description`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`consumable_type`
 ,new.`description`
);
end;|

create trigger `sr_consumable_types_jd` after delete on `sr_consumable_types` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumable_types_jn` where `consumable_type`=old.`consumable_type`
  and jn_datetime = xdate;

update `sr_consumable_types_jn` set jn_endtime = xdate
where `consumable_type`=old.`consumable_type`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumable_types_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`consumable_type`
 ,`description`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`consumable_type`
 ,old.`description`
);
end;|
delimiter ;



delimiter |
create trigger `sr_consumables_ji` after insert on `sr_consumables` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumables_jn` where `consumable_id`=new.`consumable_id`
  and jn_datetime = xdate;

update `sr_consumables_jn` set jn_endtime = xdate
where `consumable_id`=new.`consumable_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumables_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`consumable_id`
 ,`consumable_type`
 ,`consumable_name`
 ,`description`
 ,`template_consumable_id`
 ,`notes`
 ,`isrc_code`
 ,`ean`
 ,`bn`
 ,`business`
 ,`item_name`
 ,`item_number`
 ,`amount`
 ,`currency_code`
 ,`shipping`
 ,`shipping2`
 ,`demo_video_url`
 ,`image_url`
 ,`imagehead_url`
 ,`undefined_quantity`
 ,`receiver_email`
 ,`mrb`
 ,`pal`
 ,`no_shipping`
 ,`no_note`
 ,`title`
 ,`artist`
 ,`label`
 ,`catalogue`
 ,`format`
 ,`barcode`
 ,`the_release`
 ,`action_id`
 ,`affiliation_code`
 ,`our_youtube_video_url`
 ,`vendor_youtube_video_url`
 ,`live_youtube_video_url`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`consumable_id`
 ,new.`consumable_type`
 ,new.`consumable_name`
 ,new.`description`
 ,new.`template_consumable_id`
 ,new.`notes`
 ,new.`isrc_code`
 ,new.`ean`
 ,new.`bn`
 ,new.`business`
 ,new.`item_name`
 ,new.`item_number`
 ,new.`amount`
 ,new.`currency_code`
 ,new.`shipping`
 ,new.`shipping2`
 ,new.`demo_video_url`
 ,new.`image_url`
 ,new.`imagehead_url`
 ,new.`undefined_quantity`
 ,new.`receiver_email`
 ,new.`mrb`
 ,new.`pal`
 ,new.`no_shipping`
 ,new.`no_note`
 ,new.`title`
 ,new.`artist`
 ,new.`label`
 ,new.`catalogue`
 ,new.`format`
 ,new.`barcode`
 ,new.`the_release`
 ,new.`action_id`
 ,new.`affiliation_code`
 ,new.`our_youtube_video_url`
 ,new.`vendor_youtube_video_url`
 ,new.`live_youtube_video_url`
);
end;|

create trigger `sr_consumables_ju` after update on `sr_consumables` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumables_jn` where `consumable_id`=old.`consumable_id`
  and jn_datetime = xdate;

update `sr_consumables_jn` set jn_endtime = xdate
where `consumable_id`=old.`consumable_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumables_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`consumable_id`
 ,`consumable_type`
 ,`consumable_name`
 ,`description`
 ,`template_consumable_id`
 ,`notes`
 ,`isrc_code`
 ,`ean`
 ,`bn`
 ,`business`
 ,`item_name`
 ,`item_number`
 ,`amount`
 ,`currency_code`
 ,`shipping`
 ,`shipping2`
 ,`demo_video_url`
 ,`image_url`
 ,`imagehead_url`
 ,`undefined_quantity`
 ,`receiver_email`
 ,`mrb`
 ,`pal`
 ,`no_shipping`
 ,`no_note`
 ,`title`
 ,`artist`
 ,`label`
 ,`catalogue`
 ,`format`
 ,`barcode`
 ,`the_release`
 ,`action_id`
 ,`affiliation_code`
 ,`our_youtube_video_url`
 ,`vendor_youtube_video_url`
 ,`live_youtube_video_url`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`consumable_id`
 ,new.`consumable_type`
 ,new.`consumable_name`
 ,new.`description`
 ,new.`template_consumable_id`
 ,new.`notes`
 ,new.`isrc_code`
 ,new.`ean`
 ,new.`bn`
 ,new.`business`
 ,new.`item_name`
 ,new.`item_number`
 ,new.`amount`
 ,new.`currency_code`
 ,new.`shipping`
 ,new.`shipping2`
 ,new.`demo_video_url`
 ,new.`image_url`
 ,new.`imagehead_url`
 ,new.`undefined_quantity`
 ,new.`receiver_email`
 ,new.`mrb`
 ,new.`pal`
 ,new.`no_shipping`
 ,new.`no_note`
 ,new.`title`
 ,new.`artist`
 ,new.`label`
 ,new.`catalogue`
 ,new.`format`
 ,new.`barcode`
 ,new.`the_release`
 ,new.`action_id`
 ,new.`affiliation_code`
 ,new.`our_youtube_video_url`
 ,new.`vendor_youtube_video_url`
 ,new.`live_youtube_video_url`
);
end;|

create trigger `sr_consumables_jd` after delete on `sr_consumables` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_consumables_jn` where `consumable_id`=old.`consumable_id`
  and jn_datetime = xdate;

update `sr_consumables_jn` set jn_endtime = xdate
where `consumable_id`=old.`consumable_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_consumables_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`consumable_id`
 ,`consumable_type`
 ,`consumable_name`
 ,`description`
 ,`template_consumable_id`
 ,`notes`
 ,`isrc_code`
 ,`ean`
 ,`bn`
 ,`business`
 ,`item_name`
 ,`item_number`
 ,`amount`
 ,`currency_code`
 ,`shipping`
 ,`shipping2`
 ,`demo_video_url`
 ,`image_url`
 ,`imagehead_url`
 ,`undefined_quantity`
 ,`receiver_email`
 ,`mrb`
 ,`pal`
 ,`no_shipping`
 ,`no_note`
 ,`title`
 ,`artist`
 ,`label`
 ,`catalogue`
 ,`format`
 ,`barcode`
 ,`the_release`
 ,`action_id`
 ,`affiliation_code`
 ,`our_youtube_video_url`
 ,`vendor_youtube_video_url`
 ,`live_youtube_video_url`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`consumable_id`
 ,old.`consumable_type`
 ,old.`consumable_name`
 ,old.`description`
 ,old.`template_consumable_id`
 ,old.`notes`
 ,old.`isrc_code`
 ,old.`ean`
 ,old.`bn`
 ,old.`business`
 ,old.`item_name`
 ,old.`item_number`
 ,old.`amount`
 ,old.`currency_code`
 ,old.`shipping`
 ,old.`shipping2`
 ,old.`demo_video_url`
 ,old.`image_url`
 ,old.`imagehead_url`
 ,old.`undefined_quantity`
 ,old.`receiver_email`
 ,old.`mrb`
 ,old.`pal`
 ,old.`no_shipping`
 ,old.`no_note`
 ,old.`title`
 ,old.`artist`
 ,old.`label`
 ,old.`catalogue`
 ,old.`format`
 ,old.`barcode`
 ,old.`the_release`
 ,old.`action_id`
 ,old.`affiliation_code`
 ,old.`our_youtube_video_url`
 ,old.`vendor_youtube_video_url`
 ,old.`live_youtube_video_url`
);
end;|
delimiter ;



delimiter |
create trigger `sr_emails_ji` after insert on `sr_emails` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_emails_jn` where `email_address`=new.`email_address`
  and jn_datetime = xdate;

update `sr_emails_jn` set jn_endtime = xdate
where `email_address`=new.`email_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_emails_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`email_address`
 ,`email_address_full`
 ,`artist_id`
 ,`artist_seq`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`email_address`
 ,new.`email_address_full`
 ,new.`artist_id`
 ,new.`artist_seq`
);
end;|

create trigger `sr_emails_ju` after update on `sr_emails` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_emails_jn` where `email_address`=old.`email_address`
  and jn_datetime = xdate;

update `sr_emails_jn` set jn_endtime = xdate
where `email_address`=old.`email_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_emails_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`email_address`
 ,`email_address_full`
 ,`artist_id`
 ,`artist_seq`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`email_address`
 ,new.`email_address_full`
 ,new.`artist_id`
 ,new.`artist_seq`
);
end;|

create trigger `sr_emails_jd` after delete on `sr_emails` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_emails_jn` where `email_address`=old.`email_address`
  and jn_datetime = xdate;

update `sr_emails_jn` set jn_endtime = xdate
where `email_address`=old.`email_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_emails_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`email_address`
 ,`email_address_full`
 ,`artist_id`
 ,`artist_seq`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`email_address`
 ,old.`email_address_full`
 ,old.`artist_id`
 ,old.`artist_seq`
);
end;|
delimiter ;



delimiter |
create trigger `sr_folders_ji` after insert on `sr_folders` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_folders_jn` where `station_name`=new.`station_name`
  and `folder`=new.`folder`
  and jn_datetime = xdate;

update `sr_folders_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and `folder`=new.`folder`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_folders_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`folder`
 ,`unescaped_folder`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`folder`
 ,new.`unescaped_folder`
);
end;|

create trigger `sr_folders_ju` after update on `sr_folders` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_folders_jn` where `station_name`=old.`station_name`
  and `folder`=old.`folder`
  and jn_datetime = xdate;

update `sr_folders_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `folder`=old.`folder`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_folders_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`folder`
 ,`unescaped_folder`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`cd_short_name`
 ,new.`folder`
 ,new.`unescaped_folder`
);
end;|

create trigger `sr_folders_jd` after delete on `sr_folders` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_folders_jn` where `station_name`=old.`station_name`
  and `folder`=old.`folder`
  and jn_datetime = xdate;

update `sr_folders_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `folder`=old.`folder`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_folders_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`cd_short_name`
 ,`folder`
 ,`unescaped_folder`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`cd_short_name`
 ,old.`folder`
 ,old.`unescaped_folder`
);
end;|
delimiter ;



delimiter |
create trigger `sr_hash_code_links_ji` after insert on `sr_hash_code_links` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_hash_code_links_jn` where `sha256`=new.`sha256`
  and `table_name`=new.`table_name`
  and `column_name`=new.`column_name`
  and jn_datetime = xdate;

update `sr_hash_code_links_jn` set jn_endtime = xdate
where `sha256`=new.`sha256`
  and `table_name`=new.`table_name`
  and `column_name`=new.`column_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_hash_code_links_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`sha256`
 ,`table_name`
 ,`column_name`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`sha256`
 ,new.`table_name`
 ,new.`column_name`
);
end;|

create trigger `sr_hash_code_links_ju` after update on `sr_hash_code_links` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_hash_code_links_jn` where `sha256`=old.`sha256`
  and `table_name`=old.`table_name`
  and `column_name`=old.`column_name`
  and jn_datetime = xdate;

update `sr_hash_code_links_jn` set jn_endtime = xdate
where `sha256`=old.`sha256`
  and `table_name`=old.`table_name`
  and `column_name`=old.`column_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_hash_code_links_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`sha256`
 ,`table_name`
 ,`column_name`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`sha256`
 ,new.`table_name`
 ,new.`column_name`
);
end;|

create trigger `sr_hash_code_links_jd` after delete on `sr_hash_code_links` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_hash_code_links_jn` where `sha256`=old.`sha256`
  and `table_name`=old.`table_name`
  and `column_name`=old.`column_name`
  and jn_datetime = xdate;

update `sr_hash_code_links_jn` set jn_endtime = xdate
where `sha256`=old.`sha256`
  and `table_name`=old.`table_name`
  and `column_name`=old.`column_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_hash_code_links_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`sha256`
 ,`table_name`
 ,`column_name`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`sha256`
 ,old.`table_name`
 ,old.`column_name`
);
end;|
delimiter ;



delimiter |
create trigger `sr_hash_codes_ji` after insert on `sr_hash_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_hash_codes_jn` where `sha256`=new.`sha256`
  and jn_datetime = xdate;

update `sr_hash_codes_jn` set jn_endtime = xdate
where `sha256`=new.`sha256`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_hash_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`sha256`
 ,`data`
 ,`description`
 ,`notes`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`sha256`
 ,new.`data`
 ,new.`description`
 ,new.`notes`
);
end;|

create trigger `sr_hash_codes_ju` after update on `sr_hash_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_hash_codes_jn` where `sha256`=old.`sha256`
  and jn_datetime = xdate;

update `sr_hash_codes_jn` set jn_endtime = xdate
where `sha256`=old.`sha256`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_hash_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`sha256`
 ,`data`
 ,`description`
 ,`notes`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`sha256`
 ,new.`data`
 ,new.`description`
 ,new.`notes`
);
end;|

create trigger `sr_hash_codes_jd` after delete on `sr_hash_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_hash_codes_jn` where `sha256`=old.`sha256`
  and jn_datetime = xdate;

update `sr_hash_codes_jn` set jn_endtime = xdate
where `sha256`=old.`sha256`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_hash_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`sha256`
 ,`data`
 ,`description`
 ,`notes`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`sha256`
 ,old.`data`
 ,old.`description`
 ,old.`notes`
);
end;|
delimiter ;



delimiter |
create trigger `sr_international_articles_ji` after insert on `sr_international_articles` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_international_articles_jn` where `ean`=new.`ean`
  and jn_datetime = xdate;

update `sr_international_articles_jn` set jn_endtime = xdate
where `ean`=new.`ean`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_international_articles_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`ean`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`ean`
);
end;|

create trigger `sr_international_articles_ju` after update on `sr_international_articles` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_international_articles_jn` where `ean`=old.`ean`
  and jn_datetime = xdate;

update `sr_international_articles_jn` set jn_endtime = xdate
where `ean`=old.`ean`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_international_articles_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`ean`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`ean`
);
end;|

create trigger `sr_international_articles_jd` after delete on `sr_international_articles` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_international_articles_jn` where `ean`=old.`ean`
  and jn_datetime = xdate;

update `sr_international_articles_jn` set jn_endtime = xdate
where `ean`=old.`ean`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_international_articles_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`ean`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`ean`
);
end;|
delimiter ;



delimiter |
create trigger `sr_international_standard_recording_codes_ji` after insert on `sr_international_standard_recording_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_international_standard_recording_codes_jn` where `isrc_code`=new.`isrc_code`
  and jn_datetime = xdate;

update `sr_international_standard_recording_codes_jn` set jn_endtime = xdate
where `isrc_code`=new.`isrc_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_international_standard_recording_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`isrc_code`
 ,`country_code`
 ,`registrant_code`
 ,`year_of_reference`
 ,`designation_code`
 ,`audio_or_video`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`isrc_code`
 ,new.`country_code`
 ,new.`registrant_code`
 ,new.`year_of_reference`
 ,new.`designation_code`
 ,new.`audio_or_video`
);
end;|

create trigger `sr_international_standard_recording_codes_ju` after update on `sr_international_standard_recording_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_international_standard_recording_codes_jn` where `isrc_code`=old.`isrc_code`
  and jn_datetime = xdate;

update `sr_international_standard_recording_codes_jn` set jn_endtime = xdate
where `isrc_code`=old.`isrc_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_international_standard_recording_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`isrc_code`
 ,`country_code`
 ,`registrant_code`
 ,`year_of_reference`
 ,`designation_code`
 ,`audio_or_video`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`isrc_code`
 ,new.`country_code`
 ,new.`registrant_code`
 ,new.`year_of_reference`
 ,new.`designation_code`
 ,new.`audio_or_video`
);
end;|

create trigger `sr_international_standard_recording_codes_jd` after delete on `sr_international_standard_recording_codes` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_international_standard_recording_codes_jn` where `isrc_code`=old.`isrc_code`
  and jn_datetime = xdate;

update `sr_international_standard_recording_codes_jn` set jn_endtime = xdate
where `isrc_code`=old.`isrc_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_international_standard_recording_codes_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`isrc_code`
 ,`country_code`
 ,`registrant_code`
 ,`year_of_reference`
 ,`designation_code`
 ,`audio_or_video`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`isrc_code`
 ,old.`country_code`
 ,old.`registrant_code`
 ,old.`year_of_reference`
 ,old.`designation_code`
 ,old.`audio_or_video`
);
end;|
delimiter ;



delimiter |
create trigger `sr_ip_addresses_ji` after insert on `sr_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_ip_addresses_jn` where `ip_address`=new.`ip_address`
  and jn_datetime = xdate;

update `sr_ip_addresses_jn` set jn_endtime = xdate
where `ip_address`=new.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`ip_address`
 ,`reverse_dns`
 ,`last_time_checked`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`ip_address`
 ,new.`reverse_dns`
 ,new.`last_time_checked`
);
end;|

create trigger `sr_ip_addresses_ju` after update on `sr_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_ip_addresses_jn` where `ip_address`=old.`ip_address`
  and jn_datetime = xdate;

update `sr_ip_addresses_jn` set jn_endtime = xdate
where `ip_address`=old.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`ip_address`
 ,`reverse_dns`
 ,`last_time_checked`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`ip_address`
 ,new.`reverse_dns`
 ,new.`last_time_checked`
);
end;|

create trigger `sr_ip_addresses_jd` after delete on `sr_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_ip_addresses_jn` where `ip_address`=old.`ip_address`
  and jn_datetime = xdate;

update `sr_ip_addresses_jn` set jn_endtime = xdate
where `ip_address`=old.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`ip_address`
 ,`reverse_dns`
 ,`last_time_checked`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`ip_address`
 ,old.`reverse_dns`
 ,old.`last_time_checked`
);
end;|
delimiter ;



delimiter |
create trigger `sr_label_to_affiliations_ji` after insert on `sr_label_to_affiliations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_label_to_affiliations_jn` where `label`=new.`label`
  and jn_datetime = xdate;

update `sr_label_to_affiliations_jn` set jn_endtime = xdate
where `label`=new.`label`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_label_to_affiliations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`label`
 ,`affiliation_code`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`label`
 ,new.`affiliation_code`
);
end;|

create trigger `sr_label_to_affiliations_ju` after update on `sr_label_to_affiliations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_label_to_affiliations_jn` where `label`=old.`label`
  and jn_datetime = xdate;

update `sr_label_to_affiliations_jn` set jn_endtime = xdate
where `label`=old.`label`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_label_to_affiliations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`label`
 ,`affiliation_code`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`label`
 ,new.`affiliation_code`
);
end;|

create trigger `sr_label_to_affiliations_jd` after delete on `sr_label_to_affiliations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_label_to_affiliations_jn` where `label`=old.`label`
  and jn_datetime = xdate;

update `sr_label_to_affiliations_jn` set jn_endtime = xdate
where `label`=old.`label`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_label_to_affiliations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`label`
 ,`affiliation_code`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`label`
 ,old.`affiliation_code`
);
end;|
delimiter ;



delimiter |
create trigger `sr_name_ip_addresses_ji` after insert on `sr_name_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_name_ip_addresses_jn` where `dns_name`=new.`dns_name`
  and `ip_address`=new.`ip_address`
  and jn_datetime = xdate;

update `sr_name_ip_addresses_jn` set jn_endtime = xdate
where `dns_name`=new.`dns_name`
  and `ip_address`=new.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_name_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`ip_address`
 ,`last_time_checked`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`dns_name`
 ,new.`ip_address`
 ,new.`last_time_checked`
);
end;|

create trigger `sr_name_ip_addresses_ju` after update on `sr_name_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_name_ip_addresses_jn` where `dns_name`=old.`dns_name`
  and `ip_address`=old.`ip_address`
  and jn_datetime = xdate;

update `sr_name_ip_addresses_jn` set jn_endtime = xdate
where `dns_name`=old.`dns_name`
  and `ip_address`=old.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_name_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`ip_address`
 ,`last_time_checked`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`dns_name`
 ,new.`ip_address`
 ,new.`last_time_checked`
);
end;|

create trigger `sr_name_ip_addresses_jd` after delete on `sr_name_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_name_ip_addresses_jn` where `dns_name`=old.`dns_name`
  and `ip_address`=old.`ip_address`
  and jn_datetime = xdate;

update `sr_name_ip_addresses_jn` set jn_endtime = xdate
where `dns_name`=old.`dns_name`
  and `ip_address`=old.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_name_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`ip_address`
 ,`last_time_checked`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`dns_name`
 ,old.`ip_address`
 ,old.`last_time_checked`
);
end;|
delimiter ;



delimiter |
create trigger `sr_names_ji` after insert on `sr_names` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_names_jn` where `dns_name`=new.`dns_name`
  and jn_datetime = xdate;

update `sr_names_jn` set jn_endtime = xdate
where `dns_name`=new.`dns_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_names_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`description`
 ,`last_time_checked`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`dns_name`
 ,new.`description`
 ,new.`last_time_checked`
);
end;|

create trigger `sr_names_ju` after update on `sr_names` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_names_jn` where `dns_name`=old.`dns_name`
  and jn_datetime = xdate;

update `sr_names_jn` set jn_endtime = xdate
where `dns_name`=old.`dns_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_names_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`description`
 ,`last_time_checked`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`dns_name`
 ,new.`description`
 ,new.`last_time_checked`
);
end;|

create trigger `sr_names_jd` after delete on `sr_names` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_names_jn` where `dns_name`=old.`dns_name`
  and jn_datetime = xdate;

update `sr_names_jn` set jn_endtime = xdate
where `dns_name`=old.`dns_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_names_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`description`
 ,`last_time_checked`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`dns_name`
 ,old.`description`
 ,old.`last_time_checked`
);
end;|
delimiter ;



delimiter |
create trigger `sr_oauth2_sessions_ji` after insert on `sr_oauth2_sessions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_oauth2_sessions_jn` where `oauth2_session_code`=new.`oauth2_session_code`
  and jn_datetime = xdate;

update `sr_oauth2_sessions_jn` set jn_endtime = xdate
where `oauth2_session_code`=new.`oauth2_session_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_oauth2_sessions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`oauth2_session_code`
 ,`user_code`
 ,`ip_address`
 ,`session_supercookie`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`oauth2_session_code`
 ,new.`user_code`
 ,new.`ip_address`
 ,new.`session_supercookie`
);
end;|

create trigger `sr_oauth2_sessions_ju` after update on `sr_oauth2_sessions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_oauth2_sessions_jn` where `oauth2_session_code`=old.`oauth2_session_code`
  and jn_datetime = xdate;

update `sr_oauth2_sessions_jn` set jn_endtime = xdate
where `oauth2_session_code`=old.`oauth2_session_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_oauth2_sessions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`oauth2_session_code`
 ,`user_code`
 ,`ip_address`
 ,`session_supercookie`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`oauth2_session_code`
 ,new.`user_code`
 ,new.`ip_address`
 ,new.`session_supercookie`
);
end;|

create trigger `sr_oauth2_sessions_jd` after delete on `sr_oauth2_sessions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_oauth2_sessions_jn` where `oauth2_session_code`=old.`oauth2_session_code`
  and jn_datetime = xdate;

update `sr_oauth2_sessions_jn` set jn_endtime = xdate
where `oauth2_session_code`=old.`oauth2_session_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_oauth2_sessions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`oauth2_session_code`
 ,`user_code`
 ,`ip_address`
 ,`session_supercookie`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`oauth2_session_code`
 ,old.`user_code`
 ,old.`ip_address`
 ,old.`session_supercookie`
);
end;|
delimiter ;



delimiter |
create trigger `sr_oauth2_tokens_ji` after insert on `sr_oauth2_tokens` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_oauth2_tokens_jn` where `oauth2_token`=new.`oauth2_token`
  and jn_datetime = xdate;

update `sr_oauth2_tokens_jn` set jn_endtime = xdate
where `oauth2_token`=new.`oauth2_token`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_oauth2_tokens_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`oauth2_token`
 ,`oauth2_session_code`
 ,`start_ut`
 ,`end_ut`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`oauth2_token`
 ,new.`oauth2_session_code`
 ,new.`start_ut`
 ,new.`end_ut`
);
end;|

create trigger `sr_oauth2_tokens_ju` after update on `sr_oauth2_tokens` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_oauth2_tokens_jn` where `oauth2_token`=old.`oauth2_token`
  and jn_datetime = xdate;

update `sr_oauth2_tokens_jn` set jn_endtime = xdate
where `oauth2_token`=old.`oauth2_token`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_oauth2_tokens_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`oauth2_token`
 ,`oauth2_session_code`
 ,`start_ut`
 ,`end_ut`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`oauth2_token`
 ,new.`oauth2_session_code`
 ,new.`start_ut`
 ,new.`end_ut`
);
end;|

create trigger `sr_oauth2_tokens_jd` after delete on `sr_oauth2_tokens` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_oauth2_tokens_jn` where `oauth2_token`=old.`oauth2_token`
  and jn_datetime = xdate;

update `sr_oauth2_tokens_jn` set jn_endtime = xdate
where `oauth2_token`=old.`oauth2_token`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_oauth2_tokens_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`oauth2_token`
 ,`oauth2_session_code`
 ,`start_ut`
 ,`end_ut`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`oauth2_token`
 ,old.`oauth2_session_code`
 ,old.`start_ut`
 ,old.`end_ut`
);
end;|
delimiter ;



delimiter |
create trigger `sr_paypal_raw_transactions_ji` after insert on `sr_paypal_raw_transactions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_paypal_raw_transactions_jn` where `transaction_code`=new.`transaction_code`
  and jn_datetime = xdate;

update `sr_paypal_raw_transactions_jn` set jn_endtime = xdate
where `transaction_code`=new.`transaction_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_paypal_raw_transactions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`transaction_code`
 ,`status`
 ,`last`
 ,`stuff`
 ,`build_status`
 ,`mail_status`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`transaction_code`
 ,new.`status`
 ,new.`last`
 ,new.`stuff`
 ,new.`build_status`
 ,new.`mail_status`
);
end;|

create trigger `sr_paypal_raw_transactions_ju` after update on `sr_paypal_raw_transactions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_paypal_raw_transactions_jn` where `transaction_code`=old.`transaction_code`
  and jn_datetime = xdate;

update `sr_paypal_raw_transactions_jn` set jn_endtime = xdate
where `transaction_code`=old.`transaction_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_paypal_raw_transactions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`transaction_code`
 ,`status`
 ,`last`
 ,`stuff`
 ,`build_status`
 ,`mail_status`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`transaction_code`
 ,new.`status`
 ,new.`last`
 ,new.`stuff`
 ,new.`build_status`
 ,new.`mail_status`
);
end;|

create trigger `sr_paypal_raw_transactions_jd` after delete on `sr_paypal_raw_transactions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_paypal_raw_transactions_jn` where `transaction_code`=old.`transaction_code`
  and jn_datetime = xdate;

update `sr_paypal_raw_transactions_jn` set jn_endtime = xdate
where `transaction_code`=old.`transaction_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_paypal_raw_transactions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`transaction_code`
 ,`status`
 ,`last`
 ,`stuff`
 ,`build_status`
 ,`mail_status`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`transaction_code`
 ,old.`status`
 ,old.`last`
 ,old.`stuff`
 ,old.`build_status`
 ,old.`mail_status`
);
end;|
delimiter ;



delimiter |
create trigger `sr_picture_info_ji` after insert on `sr_picture_info` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_picture_info_jn` where `station_name`=new.`station_name`
  and `picture`=new.`picture`
  and jn_datetime = xdate;

update `sr_picture_info_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and `picture`=new.`picture`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_picture_info_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`picture`
 ,`picture_ipc`
 ,`picture_width`
 ,`picture_height`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`picture`
 ,new.`picture_ipc`
 ,new.`picture_width`
 ,new.`picture_height`
);
end;|

create trigger `sr_picture_info_ju` after update on `sr_picture_info` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_picture_info_jn` where `station_name`=old.`station_name`
  and `picture`=old.`picture`
  and jn_datetime = xdate;

update `sr_picture_info_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `picture`=old.`picture`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_picture_info_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`picture`
 ,`picture_ipc`
 ,`picture_width`
 ,`picture_height`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`picture`
 ,new.`picture_ipc`
 ,new.`picture_width`
 ,new.`picture_height`
);
end;|

create trigger `sr_picture_info_jd` after delete on `sr_picture_info` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_picture_info_jn` where `station_name`=old.`station_name`
  and `picture`=old.`picture`
  and jn_datetime = xdate;

update `sr_picture_info_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `picture`=old.`picture`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_picture_info_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`picture`
 ,`picture_ipc`
 ,`picture_width`
 ,`picture_height`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`picture`
 ,old.`picture_ipc`
 ,old.`picture_width`
 ,old.`picture_height`
);
end;|
delimiter ;



delimiter |
create trigger `sr_radio_plays_ji` after insert on `sr_radio_plays` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_radio_plays_jn` where `start_unix_time`=new.`start_unix_time`
  and `filename`=new.`filename`
  and `station_name`=new.`station_name`
  and jn_datetime = xdate;

update `sr_radio_plays_jn` set jn_endtime = xdate
where `start_unix_time`=new.`start_unix_time`
  and `filename`=new.`filename`
  and `station_name`=new.`station_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_radio_plays_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`start_unix_time`
 ,`end_unix_time`
 ,`length`
 ,`command`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`filename`
 ,new.`start_unix_time`
 ,new.`end_unix_time`
 ,new.`length`
 ,new.`command`
);
end;|

create trigger `sr_radio_plays_ju` after update on `sr_radio_plays` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_radio_plays_jn` where `start_unix_time`=old.`start_unix_time`
  and `filename`=old.`filename`
  and `station_name`=old.`station_name`
  and jn_datetime = xdate;

update `sr_radio_plays_jn` set jn_endtime = xdate
where `start_unix_time`=old.`start_unix_time`
  and `filename`=old.`filename`
  and `station_name`=old.`station_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_radio_plays_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`start_unix_time`
 ,`end_unix_time`
 ,`length`
 ,`command`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`filename`
 ,new.`start_unix_time`
 ,new.`end_unix_time`
 ,new.`length`
 ,new.`command`
);
end;|

create trigger `sr_radio_plays_jd` after delete on `sr_radio_plays` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_radio_plays_jn` where `start_unix_time`=old.`start_unix_time`
  and `filename`=old.`filename`
  and `station_name`=old.`station_name`
  and jn_datetime = xdate;

update `sr_radio_plays_jn` set jn_endtime = xdate
where `start_unix_time`=old.`start_unix_time`
  and `filename`=old.`filename`
  and `station_name`=old.`station_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_radio_plays_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`start_unix_time`
 ,`end_unix_time`
 ,`length`
 ,`command`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`filename`
 ,old.`start_unix_time`
 ,old.`end_unix_time`
 ,old.`length`
 ,old.`command`
);
end;|
delimiter ;



delimiter |
create trigger `sr_reflinks_ji` after insert on `sr_reflinks` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_reflinks_jn` where `reflink`=new.`reflink`
  and jn_datetime = xdate;

update `sr_reflinks_jn` set jn_endtime = xdate
where `reflink`=new.`reflink`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_reflinks_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`reflink`
 ,`consumable_id`
 ,`affiliation_code`
 ,`tag`
 ,`actual_link`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`reflink`
 ,new.`consumable_id`
 ,new.`affiliation_code`
 ,new.`tag`
 ,new.`actual_link`
);
end;|

create trigger `sr_reflinks_ju` after update on `sr_reflinks` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_reflinks_jn` where `reflink`=old.`reflink`
  and jn_datetime = xdate;

update `sr_reflinks_jn` set jn_endtime = xdate
where `reflink`=old.`reflink`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_reflinks_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`reflink`
 ,`consumable_id`
 ,`affiliation_code`
 ,`tag`
 ,`actual_link`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`reflink`
 ,new.`consumable_id`
 ,new.`affiliation_code`
 ,new.`tag`
 ,new.`actual_link`
);
end;|

create trigger `sr_reflinks_jd` after delete on `sr_reflinks` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_reflinks_jn` where `reflink`=old.`reflink`
  and jn_datetime = xdate;

update `sr_reflinks_jn` set jn_endtime = xdate
where `reflink`=old.`reflink`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_reflinks_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`reflink`
 ,`consumable_id`
 ,`affiliation_code`
 ,`tag`
 ,`actual_link`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`reflink`
 ,old.`consumable_id`
 ,old.`affiliation_code`
 ,old.`tag`
 ,old.`actual_link`
);
end;|
delimiter ;



delimiter |
create trigger `sr_sessions_ji` after insert on `sr_sessions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_sessions_jn` where `session_code`=new.`session_code`
  and `start_unix_time`=new.`start_unix_time`
  and jn_datetime = xdate;

update `sr_sessions_jn` set jn_endtime = xdate
where `session_code`=new.`session_code`
  and `start_unix_time`=new.`start_unix_time`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_sessions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`session_code`
 ,`start_unix_time`
 ,`end_unix_time`
 ,`start_position`
 ,`end_position`
 ,`user_code`
 ,`ip_address`
 ,`dns_name`
 ,`program_name`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`session_code`
 ,new.`start_unix_time`
 ,new.`end_unix_time`
 ,new.`start_position`
 ,new.`end_position`
 ,new.`user_code`
 ,new.`ip_address`
 ,new.`dns_name`
 ,new.`program_name`
);
end;|

create trigger `sr_sessions_ju` after update on `sr_sessions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_sessions_jn` where `session_code`=old.`session_code`
  and `start_unix_time`=old.`start_unix_time`
  and jn_datetime = xdate;

update `sr_sessions_jn` set jn_endtime = xdate
where `session_code`=old.`session_code`
  and `start_unix_time`=old.`start_unix_time`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_sessions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`session_code`
 ,`start_unix_time`
 ,`end_unix_time`
 ,`start_position`
 ,`end_position`
 ,`user_code`
 ,`ip_address`
 ,`dns_name`
 ,`program_name`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`session_code`
 ,new.`start_unix_time`
 ,new.`end_unix_time`
 ,new.`start_position`
 ,new.`end_position`
 ,new.`user_code`
 ,new.`ip_address`
 ,new.`dns_name`
 ,new.`program_name`
);
end;|

create trigger `sr_sessions_jd` after delete on `sr_sessions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_sessions_jn` where `session_code`=old.`session_code`
  and `start_unix_time`=old.`start_unix_time`
  and jn_datetime = xdate;

update `sr_sessions_jn` set jn_endtime = xdate
where `session_code`=old.`session_code`
  and `start_unix_time`=old.`start_unix_time`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_sessions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`session_code`
 ,`start_unix_time`
 ,`end_unix_time`
 ,`start_position`
 ,`end_position`
 ,`user_code`
 ,`ip_address`
 ,`dns_name`
 ,`program_name`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`session_code`
 ,old.`start_unix_time`
 ,old.`end_unix_time`
 ,old.`start_position`
 ,old.`end_position`
 ,old.`user_code`
 ,old.`ip_address`
 ,old.`dns_name`
 ,old.`program_name`
);
end;|
delimiter ;



delimiter |
create trigger `sr_song_info_ji` after insert on `sr_song_info` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_song_info_jn` where `station_name`=new.`station_name`
  and `filename`=new.`filename`
  and jn_datetime = xdate;

update `sr_song_info_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and `filename`=new.`filename`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_song_info_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`unescaped_filename`
 ,`length`
 ,`ean`
 ,`featured_artist`
 ,`sound_recording_title`
 ,`isrc`
 ,`album_title`
 ,`marketing_label`
 ,`track`
 ,`title`
 ,`year`
 ,`genre`
 ,`extra1`
 ,`extra2`
 ,`picture`
 ,`picture_ipc`
 ,`picture_width`
 ,`picture_height`
 ,`sha256`
 ,`cd_short_name`
 ,`folder`
 ,`cd_track`
 ,`demo_video_url`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`filename`
 ,new.`unescaped_filename`
 ,new.`length`
 ,new.`ean`
 ,new.`featured_artist`
 ,new.`sound_recording_title`
 ,new.`isrc`
 ,new.`album_title`
 ,new.`marketing_label`
 ,new.`track`
 ,new.`title`
 ,new.`year`
 ,new.`genre`
 ,new.`extra1`
 ,new.`extra2`
 ,new.`picture`
 ,new.`picture_ipc`
 ,new.`picture_width`
 ,new.`picture_height`
 ,new.`sha256`
 ,new.`cd_short_name`
 ,new.`folder`
 ,new.`cd_track`
 ,new.`demo_video_url`
);
end;|

create trigger `sr_song_info_ju` after update on `sr_song_info` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_song_info_jn` where `station_name`=old.`station_name`
  and `filename`=old.`filename`
  and jn_datetime = xdate;

update `sr_song_info_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `filename`=old.`filename`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_song_info_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`unescaped_filename`
 ,`length`
 ,`ean`
 ,`featured_artist`
 ,`sound_recording_title`
 ,`isrc`
 ,`album_title`
 ,`marketing_label`
 ,`track`
 ,`title`
 ,`year`
 ,`genre`
 ,`extra1`
 ,`extra2`
 ,`picture`
 ,`picture_ipc`
 ,`picture_width`
 ,`picture_height`
 ,`sha256`
 ,`cd_short_name`
 ,`folder`
 ,`cd_track`
 ,`demo_video_url`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`filename`
 ,new.`unescaped_filename`
 ,new.`length`
 ,new.`ean`
 ,new.`featured_artist`
 ,new.`sound_recording_title`
 ,new.`isrc`
 ,new.`album_title`
 ,new.`marketing_label`
 ,new.`track`
 ,new.`title`
 ,new.`year`
 ,new.`genre`
 ,new.`extra1`
 ,new.`extra2`
 ,new.`picture`
 ,new.`picture_ipc`
 ,new.`picture_width`
 ,new.`picture_height`
 ,new.`sha256`
 ,new.`cd_short_name`
 ,new.`folder`
 ,new.`cd_track`
 ,new.`demo_video_url`
);
end;|

create trigger `sr_song_info_jd` after delete on `sr_song_info` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_song_info_jn` where `station_name`=old.`station_name`
  and `filename`=old.`filename`
  and jn_datetime = xdate;

update `sr_song_info_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `filename`=old.`filename`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_song_info_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`unescaped_filename`
 ,`length`
 ,`ean`
 ,`featured_artist`
 ,`sound_recording_title`
 ,`isrc`
 ,`album_title`
 ,`marketing_label`
 ,`track`
 ,`title`
 ,`year`
 ,`genre`
 ,`extra1`
 ,`extra2`
 ,`picture`
 ,`picture_ipc`
 ,`picture_width`
 ,`picture_height`
 ,`sha256`
 ,`cd_short_name`
 ,`folder`
 ,`cd_track`
 ,`demo_video_url`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`filename`
 ,old.`unescaped_filename`
 ,old.`length`
 ,old.`ean`
 ,old.`featured_artist`
 ,old.`sound_recording_title`
 ,old.`isrc`
 ,old.`album_title`
 ,old.`marketing_label`
 ,old.`track`
 ,old.`title`
 ,old.`year`
 ,old.`genre`
 ,old.`extra1`
 ,old.`extra2`
 ,old.`picture`
 ,old.`picture_ipc`
 ,old.`picture_width`
 ,old.`picture_height`
 ,old.`sha256`
 ,old.`cd_short_name`
 ,old.`folder`
 ,old.`cd_track`
 ,old.`demo_video_url`
);
end;|
delimiter ;



delimiter |
create trigger `sr_song_info_consumables_ji` after insert on `sr_song_info_consumables` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_song_info_consumables_jn` where `filename`=new.`filename`
  and `station_name`=new.`station_name`
  and `consumable_id`=new.`consumable_id`
  and jn_datetime = xdate;

update `sr_song_info_consumables_jn` set jn_endtime = xdate
where `filename`=new.`filename`
  and `station_name`=new.`station_name`
  and `consumable_id`=new.`consumable_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_song_info_consumables_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`consumable_id`
 ,`display_sequence`
 ,`for_sale_in_general`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`filename`
 ,new.`consumable_id`
 ,new.`display_sequence`
 ,new.`for_sale_in_general`
);
end;|

create trigger `sr_song_info_consumables_ju` after update on `sr_song_info_consumables` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_song_info_consumables_jn` where `filename`=old.`filename`
  and `station_name`=old.`station_name`
  and `consumable_id`=old.`consumable_id`
  and jn_datetime = xdate;

update `sr_song_info_consumables_jn` set jn_endtime = xdate
where `filename`=old.`filename`
  and `station_name`=old.`station_name`
  and `consumable_id`=old.`consumable_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_song_info_consumables_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`consumable_id`
 ,`display_sequence`
 ,`for_sale_in_general`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`filename`
 ,new.`consumable_id`
 ,new.`display_sequence`
 ,new.`for_sale_in_general`
);
end;|

create trigger `sr_song_info_consumables_jd` after delete on `sr_song_info_consumables` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_song_info_consumables_jn` where `filename`=old.`filename`
  and `station_name`=old.`station_name`
  and `consumable_id`=old.`consumable_id`
  and jn_datetime = xdate;

update `sr_song_info_consumables_jn` set jn_endtime = xdate
where `filename`=old.`filename`
  and `station_name`=old.`station_name`
  and `consumable_id`=old.`consumable_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_song_info_consumables_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`filename`
 ,`consumable_id`
 ,`display_sequence`
 ,`for_sale_in_general`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`filename`
 ,old.`consumable_id`
 ,old.`display_sequence`
 ,old.`for_sale_in_general`
);
end;|
delimiter ;



delimiter |
create trigger `sr_station_web_programs_ji` after insert on `sr_station_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_station_web_programs_jn` where `station_name`=new.`station_name`
  and `dns_name`=new.`dns_name`
  and `program_name`=new.`program_name`
  and jn_datetime = xdate;

update `sr_station_web_programs_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and `dns_name`=new.`dns_name`
  and `program_name`=new.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_station_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`dns_name`
 ,`program_name`
 ,`description`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`dns_name`
 ,new.`program_name`
 ,new.`description`
);
end;|

create trigger `sr_station_web_programs_ju` after update on `sr_station_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_station_web_programs_jn` where `station_name`=old.`station_name`
  and `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime = xdate;

update `sr_station_web_programs_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_station_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`dns_name`
 ,`program_name`
 ,`description`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`dns_name`
 ,new.`program_name`
 ,new.`description`
);
end;|

create trigger `sr_station_web_programs_jd` after delete on `sr_station_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_station_web_programs_jn` where `station_name`=old.`station_name`
  and `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime = xdate;

update `sr_station_web_programs_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_station_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`dns_name`
 ,`program_name`
 ,`description`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`dns_name`
 ,old.`program_name`
 ,old.`description`
);
end;|
delimiter ;



delimiter |
create trigger `sr_stations_ji` after insert on `sr_stations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_stations_jn` where `station_name`=new.`station_name`
  and jn_datetime = xdate;

update `sr_stations_jn` set jn_endtime = xdate
where `station_name`=new.`station_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_stations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`description`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`description`
);
end;|

create trigger `sr_stations_ju` after update on `sr_stations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_stations_jn` where `station_name`=old.`station_name`
  and jn_datetime = xdate;

update `sr_stations_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_stations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`description`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`station_name`
 ,new.`description`
);
end;|

create trigger `sr_stations_jd` after delete on `sr_stations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_stations_jn` where `station_name`=old.`station_name`
  and jn_datetime = xdate;

update `sr_stations_jn` set jn_endtime = xdate
where `station_name`=old.`station_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_stations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`station_name`
 ,`description`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`station_name`
 ,old.`description`
);
end;|
delimiter ;



delimiter |
create trigger `sr_tags_ji` after insert on `sr_tags` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_tags_jn` where `tag`=new.`tag`
  and jn_datetime = xdate;

update `sr_tags_jn` set jn_endtime = xdate
where `tag`=new.`tag`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_tags_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`tag`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`tag`
);
end;|

create trigger `sr_tags_ju` after update on `sr_tags` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_tags_jn` where `tag`=old.`tag`
  and jn_datetime = xdate;

update `sr_tags_jn` set jn_endtime = xdate
where `tag`=old.`tag`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_tags_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`tag`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`tag`
);
end;|

create trigger `sr_tags_jd` after delete on `sr_tags` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_tags_jn` where `tag`=old.`tag`
  and jn_datetime = xdate;

update `sr_tags_jn` set jn_endtime = xdate
where `tag`=old.`tag`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_tags_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`tag`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`tag`
);
end;|
delimiter ;



delimiter |
create trigger `sr_user_ip_addresses_ji` after insert on `sr_user_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_user_ip_addresses_jn` where `user_code`=new.`user_code`
  and `ip_address`=new.`ip_address`
  and jn_datetime = xdate;

update `sr_user_ip_addresses_jn` set jn_endtime = xdate
where `user_code`=new.`user_code`
  and `ip_address`=new.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_user_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`ip_address`
 ,`cred_level`
 ,`notes`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`user_code`
 ,new.`ip_address`
 ,new.`cred_level`
 ,new.`notes`
);
end;|

create trigger `sr_user_ip_addresses_ju` after update on `sr_user_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_user_ip_addresses_jn` where `user_code`=old.`user_code`
  and `ip_address`=old.`ip_address`
  and jn_datetime = xdate;

update `sr_user_ip_addresses_jn` set jn_endtime = xdate
where `user_code`=old.`user_code`
  and `ip_address`=old.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_user_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`ip_address`
 ,`cred_level`
 ,`notes`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`user_code`
 ,new.`ip_address`
 ,new.`cred_level`
 ,new.`notes`
);
end;|

create trigger `sr_user_ip_addresses_jd` after delete on `sr_user_ip_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_user_ip_addresses_jn` where `user_code`=old.`user_code`
  and `ip_address`=old.`ip_address`
  and jn_datetime = xdate;

update `sr_user_ip_addresses_jn` set jn_endtime = xdate
where `user_code`=old.`user_code`
  and `ip_address`=old.`ip_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_user_ip_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`ip_address`
 ,`cred_level`
 ,`notes`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`user_code`
 ,old.`ip_address`
 ,old.`cred_level`
 ,old.`notes`
);
end;|
delimiter ;



delimiter |
create trigger `sr_user_web_programs_ji` after insert on `sr_user_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_user_web_programs_jn` where `user_code`=new.`user_code`
  and `ip_address`=new.`ip_address`
  and `dns_name`=new.`dns_name`
  and `program_name`=new.`program_name`
  and jn_datetime = xdate;

update `sr_user_web_programs_jn` set jn_endtime = xdate
where `user_code`=new.`user_code`
  and `ip_address`=new.`ip_address`
  and `dns_name`=new.`dns_name`
  and `program_name`=new.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_user_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`ip_address`
 ,`dns_name`
 ,`program_name`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`user_code`
 ,new.`ip_address`
 ,new.`dns_name`
 ,new.`program_name`
);
end;|

create trigger `sr_user_web_programs_ju` after update on `sr_user_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_user_web_programs_jn` where `user_code`=old.`user_code`
  and `ip_address`=old.`ip_address`
  and `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime = xdate;

update `sr_user_web_programs_jn` set jn_endtime = xdate
where `user_code`=old.`user_code`
  and `ip_address`=old.`ip_address`
  and `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_user_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`ip_address`
 ,`dns_name`
 ,`program_name`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`user_code`
 ,new.`ip_address`
 ,new.`dns_name`
 ,new.`program_name`
);
end;|

create trigger `sr_user_web_programs_jd` after delete on `sr_user_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_user_web_programs_jn` where `user_code`=old.`user_code`
  and `ip_address`=old.`ip_address`
  and `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime = xdate;

update `sr_user_web_programs_jn` set jn_endtime = xdate
where `user_code`=old.`user_code`
  and `ip_address`=old.`ip_address`
  and `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_user_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`ip_address`
 ,`dns_name`
 ,`program_name`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`user_code`
 ,old.`ip_address`
 ,old.`dns_name`
 ,old.`program_name`
);
end;|
delimiter ;



delimiter |
create trigger `sr_users_ji` after insert on `sr_users` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_users_jn` where `user_code`=new.`user_code`
  and jn_datetime = xdate;

update `sr_users_jn` set jn_endtime = xdate
where `user_code`=new.`user_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_users_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`cred_level`
 ,`notes`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`user_code`
 ,new.`cred_level`
 ,new.`notes`
);
end;|

create trigger `sr_users_ju` after update on `sr_users` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_users_jn` where `user_code`=old.`user_code`
  and jn_datetime = xdate;

update `sr_users_jn` set jn_endtime = xdate
where `user_code`=old.`user_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_users_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`cred_level`
 ,`notes`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`user_code`
 ,new.`cred_level`
 ,new.`notes`
);
end;|

create trigger `sr_users_jd` after delete on `sr_users` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_users_jn` where `user_code`=old.`user_code`
  and jn_datetime = xdate;

update `sr_users_jn` set jn_endtime = xdate
where `user_code`=old.`user_code`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_users_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`user_code`
 ,`cred_level`
 ,`notes`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`user_code`
 ,old.`cred_level`
 ,old.`notes`
);
end;|
delimiter ;



delimiter |
create trigger `sr_web_programs_ji` after insert on `sr_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_web_programs_jn` where `dns_name`=new.`dns_name`
  and `program_name`=new.`program_name`
  and jn_datetime = xdate;

update `sr_web_programs_jn` set jn_endtime = xdate
where `dns_name`=new.`dns_name`
  and `program_name`=new.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`program_name`
 ,`description`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`dns_name`
 ,new.`program_name`
 ,new.`description`
);
end;|

create trigger `sr_web_programs_ju` after update on `sr_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_web_programs_jn` where `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime = xdate;

update `sr_web_programs_jn` set jn_endtime = xdate
where `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`program_name`
 ,`description`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`dns_name`
 ,new.`program_name`
 ,new.`description`
);
end;|

create trigger `sr_web_programs_jd` after delete on `sr_web_programs` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sr_web_programs_jn` where `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime = xdate;

update `sr_web_programs_jn` set jn_endtime = xdate
where `dns_name`=old.`dns_name`
  and `program_name`=old.`program_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sr_web_programs_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`dns_name`
 ,`program_name`
 ,`description`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`dns_name`
 ,old.`program_name`
 ,old.`description`
);
end;|
delimiter ;



delimiter |
create trigger `sri_raw_order_fulfillment_items_ji` after insert on `sri_raw_order_fulfillment_items` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_order_fulfillment_items_jn` where `txn_id`=new.`txn_id`
  and `display_sequence`=new.`display_sequence`
  and jn_datetime = xdate;

update `sri_raw_order_fulfillment_items_jn` set jn_endtime = xdate
where `txn_id`=new.`txn_id`
  and `display_sequence`=new.`display_sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_order_fulfillment_items_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`item_sequence`
 ,`display_sequence`
 ,`item_number`
 ,`fulfillment_type`
 ,`song_info_station_name`
 ,`song_info_file_name`
 ,`album_cover_file_name`
 ,`vault_file_name`
 ,`vault_description`
 ,`created_date`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`txn_id`
 ,new.`item_sequence`
 ,new.`display_sequence`
 ,new.`item_number`
 ,new.`fulfillment_type`
 ,new.`song_info_station_name`
 ,new.`song_info_file_name`
 ,new.`album_cover_file_name`
 ,new.`vault_file_name`
 ,new.`vault_description`
 ,new.`created_date`
);
end;|

create trigger `sri_raw_order_fulfillment_items_ju` after update on `sri_raw_order_fulfillment_items` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_order_fulfillment_items_jn` where `txn_id`=old.`txn_id`
  and `display_sequence`=old.`display_sequence`
  and jn_datetime = xdate;

update `sri_raw_order_fulfillment_items_jn` set jn_endtime = xdate
where `txn_id`=old.`txn_id`
  and `display_sequence`=old.`display_sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_order_fulfillment_items_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`item_sequence`
 ,`display_sequence`
 ,`item_number`
 ,`fulfillment_type`
 ,`song_info_station_name`
 ,`song_info_file_name`
 ,`album_cover_file_name`
 ,`vault_file_name`
 ,`vault_description`
 ,`created_date`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`txn_id`
 ,new.`item_sequence`
 ,new.`display_sequence`
 ,new.`item_number`
 ,new.`fulfillment_type`
 ,new.`song_info_station_name`
 ,new.`song_info_file_name`
 ,new.`album_cover_file_name`
 ,new.`vault_file_name`
 ,new.`vault_description`
 ,new.`created_date`
);
end;|

create trigger `sri_raw_order_fulfillment_items_jd` after delete on `sri_raw_order_fulfillment_items` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_order_fulfillment_items_jn` where `txn_id`=old.`txn_id`
  and `display_sequence`=old.`display_sequence`
  and jn_datetime = xdate;

update `sri_raw_order_fulfillment_items_jn` set jn_endtime = xdate
where `txn_id`=old.`txn_id`
  and `display_sequence`=old.`display_sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_order_fulfillment_items_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`item_sequence`
 ,`display_sequence`
 ,`item_number`
 ,`fulfillment_type`
 ,`song_info_station_name`
 ,`song_info_file_name`
 ,`album_cover_file_name`
 ,`vault_file_name`
 ,`vault_description`
 ,`created_date`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`txn_id`
 ,old.`item_sequence`
 ,old.`display_sequence`
 ,old.`item_number`
 ,old.`fulfillment_type`
 ,old.`song_info_station_name`
 ,old.`song_info_file_name`
 ,old.`album_cover_file_name`
 ,old.`vault_file_name`
 ,old.`vault_description`
 ,old.`created_date`
);
end;|
delimiter ;



delimiter |
create trigger `sri_raw_paypal_order_items_ji` after insert on `sri_raw_paypal_order_items` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_paypal_order_items_jn` where `txn_id`=new.`txn_id`
  and `item_sequence`=new.`item_sequence`
  and jn_datetime = xdate;

update `sri_raw_paypal_order_items_jn` set jn_endtime = xdate
where `txn_id`=new.`txn_id`
  and `item_sequence`=new.`item_sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_paypal_order_items_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`item_sequence`
 ,`item_number`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`txn_id`
 ,new.`item_sequence`
 ,new.`item_number`
);
end;|

create trigger `sri_raw_paypal_order_items_ju` after update on `sri_raw_paypal_order_items` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_paypal_order_items_jn` where `txn_id`=old.`txn_id`
  and `item_sequence`=old.`item_sequence`
  and jn_datetime = xdate;

update `sri_raw_paypal_order_items_jn` set jn_endtime = xdate
where `txn_id`=old.`txn_id`
  and `item_sequence`=old.`item_sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_paypal_order_items_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`item_sequence`
 ,`item_number`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`txn_id`
 ,new.`item_sequence`
 ,new.`item_number`
);
end;|

create trigger `sri_raw_paypal_order_items_jd` after delete on `sri_raw_paypal_order_items` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_paypal_order_items_jn` where `txn_id`=old.`txn_id`
  and `item_sequence`=old.`item_sequence`
  and jn_datetime = xdate;

update `sri_raw_paypal_order_items_jn` set jn_endtime = xdate
where `txn_id`=old.`txn_id`
  and `item_sequence`=old.`item_sequence`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_paypal_order_items_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`item_sequence`
 ,`item_number`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`txn_id`
 ,old.`item_sequence`
 ,old.`item_number`
);
end;|
delimiter ;



delimiter |
create trigger `sri_raw_paypal_orders_ji` after insert on `sri_raw_paypal_orders` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_paypal_orders_jn` where `txn_id`=new.`txn_id`
  and jn_datetime = xdate;

update `sri_raw_paypal_orders_jn` set jn_endtime = xdate
where `txn_id`=new.`txn_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_paypal_orders_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`business`
 ,`charset`
 ,`custom`
 ,`discount`
 ,`first_name`
 ,`insurance_amount`
 ,`ipn_track_id`
 ,`item_name`
 ,`item_number`
 ,`last_name`
 ,`mc_currency`
 ,`mc_fee`
 ,`mc_gross`
 ,`notify_version`
 ,`payer_email`
 ,`payer_id`
 ,`payer_status`
 ,`payment_date_text`
 ,`payment_date`
 ,`payment_fee`
 ,`payment_gross`
 ,`payment_status`
 ,`payment_type`
 ,`protection_eligibility`
 ,`quantity`
 ,`receipt_id`
 ,`receiver_email`
 ,`receiver_id`
 ,`residence_country`
 ,`shipping`
 ,`shipping_discount`
 ,`transaction_subject`
 ,`txn_type`
 ,`verify_sign`
 ,`station_name`
 ,`campaign_code`
 ,`web_request_type`
 ,`other_text`
 ,`full_text`
 ,`warning_text`
 ,`error_text`
 ,`shipping_method`
 ,`initial_temp_location_id`
 ,`language_code`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`txn_id`
 ,new.`business`
 ,new.`charset`
 ,new.`custom`
 ,new.`discount`
 ,new.`first_name`
 ,new.`insurance_amount`
 ,new.`ipn_track_id`
 ,new.`item_name`
 ,new.`item_number`
 ,new.`last_name`
 ,new.`mc_currency`
 ,new.`mc_fee`
 ,new.`mc_gross`
 ,new.`notify_version`
 ,new.`payer_email`
 ,new.`payer_id`
 ,new.`payer_status`
 ,new.`payment_date_text`
 ,new.`payment_date`
 ,new.`payment_fee`
 ,new.`payment_gross`
 ,new.`payment_status`
 ,new.`payment_type`
 ,new.`protection_eligibility`
 ,new.`quantity`
 ,new.`receipt_id`
 ,new.`receiver_email`
 ,new.`receiver_id`
 ,new.`residence_country`
 ,new.`shipping`
 ,new.`shipping_discount`
 ,new.`transaction_subject`
 ,new.`txn_type`
 ,new.`verify_sign`
 ,new.`station_name`
 ,new.`campaign_code`
 ,new.`web_request_type`
 ,new.`other_text`
 ,new.`full_text`
 ,new.`warning_text`
 ,new.`error_text`
 ,new.`shipping_method`
 ,new.`initial_temp_location_id`
 ,new.`language_code`
);
end;|

create trigger `sri_raw_paypal_orders_ju` after update on `sri_raw_paypal_orders` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_paypal_orders_jn` where `txn_id`=old.`txn_id`
  and jn_datetime = xdate;

update `sri_raw_paypal_orders_jn` set jn_endtime = xdate
where `txn_id`=old.`txn_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_paypal_orders_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`business`
 ,`charset`
 ,`custom`
 ,`discount`
 ,`first_name`
 ,`insurance_amount`
 ,`ipn_track_id`
 ,`item_name`
 ,`item_number`
 ,`last_name`
 ,`mc_currency`
 ,`mc_fee`
 ,`mc_gross`
 ,`notify_version`
 ,`payer_email`
 ,`payer_id`
 ,`payer_status`
 ,`payment_date_text`
 ,`payment_date`
 ,`payment_fee`
 ,`payment_gross`
 ,`payment_status`
 ,`payment_type`
 ,`protection_eligibility`
 ,`quantity`
 ,`receipt_id`
 ,`receiver_email`
 ,`receiver_id`
 ,`residence_country`
 ,`shipping`
 ,`shipping_discount`
 ,`transaction_subject`
 ,`txn_type`
 ,`verify_sign`
 ,`station_name`
 ,`campaign_code`
 ,`web_request_type`
 ,`other_text`
 ,`full_text`
 ,`warning_text`
 ,`error_text`
 ,`shipping_method`
 ,`initial_temp_location_id`
 ,`language_code`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`txn_id`
 ,new.`business`
 ,new.`charset`
 ,new.`custom`
 ,new.`discount`
 ,new.`first_name`
 ,new.`insurance_amount`
 ,new.`ipn_track_id`
 ,new.`item_name`
 ,new.`item_number`
 ,new.`last_name`
 ,new.`mc_currency`
 ,new.`mc_fee`
 ,new.`mc_gross`
 ,new.`notify_version`
 ,new.`payer_email`
 ,new.`payer_id`
 ,new.`payer_status`
 ,new.`payment_date_text`
 ,new.`payment_date`
 ,new.`payment_fee`
 ,new.`payment_gross`
 ,new.`payment_status`
 ,new.`payment_type`
 ,new.`protection_eligibility`
 ,new.`quantity`
 ,new.`receipt_id`
 ,new.`receiver_email`
 ,new.`receiver_id`
 ,new.`residence_country`
 ,new.`shipping`
 ,new.`shipping_discount`
 ,new.`transaction_subject`
 ,new.`txn_type`
 ,new.`verify_sign`
 ,new.`station_name`
 ,new.`campaign_code`
 ,new.`web_request_type`
 ,new.`other_text`
 ,new.`full_text`
 ,new.`warning_text`
 ,new.`error_text`
 ,new.`shipping_method`
 ,new.`initial_temp_location_id`
 ,new.`language_code`
);
end;|

create trigger `sri_raw_paypal_orders_jd` after delete on `sri_raw_paypal_orders` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_raw_paypal_orders_jn` where `txn_id`=old.`txn_id`
  and jn_datetime = xdate;

update `sri_raw_paypal_orders_jn` set jn_endtime = xdate
where `txn_id`=old.`txn_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_raw_paypal_orders_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`txn_id`
 ,`business`
 ,`charset`
 ,`custom`
 ,`discount`
 ,`first_name`
 ,`insurance_amount`
 ,`ipn_track_id`
 ,`item_name`
 ,`item_number`
 ,`last_name`
 ,`mc_currency`
 ,`mc_fee`
 ,`mc_gross`
 ,`notify_version`
 ,`payer_email`
 ,`payer_id`
 ,`payer_status`
 ,`payment_date_text`
 ,`payment_date`
 ,`payment_fee`
 ,`payment_gross`
 ,`payment_status`
 ,`payment_type`
 ,`protection_eligibility`
 ,`quantity`
 ,`receipt_id`
 ,`receiver_email`
 ,`receiver_id`
 ,`residence_country`
 ,`shipping`
 ,`shipping_discount`
 ,`transaction_subject`
 ,`txn_type`
 ,`verify_sign`
 ,`station_name`
 ,`campaign_code`
 ,`web_request_type`
 ,`other_text`
 ,`full_text`
 ,`warning_text`
 ,`error_text`
 ,`shipping_method`
 ,`initial_temp_location_id`
 ,`language_code`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`txn_id`
 ,old.`business`
 ,old.`charset`
 ,old.`custom`
 ,old.`discount`
 ,old.`first_name`
 ,old.`insurance_amount`
 ,old.`ipn_track_id`
 ,old.`item_name`
 ,old.`item_number`
 ,old.`last_name`
 ,old.`mc_currency`
 ,old.`mc_fee`
 ,old.`mc_gross`
 ,old.`notify_version`
 ,old.`payer_email`
 ,old.`payer_id`
 ,old.`payer_status`
 ,old.`payment_date_text`
 ,old.`payment_date`
 ,old.`payment_fee`
 ,old.`payment_gross`
 ,old.`payment_status`
 ,old.`payment_type`
 ,old.`protection_eligibility`
 ,old.`quantity`
 ,old.`receipt_id`
 ,old.`receiver_email`
 ,old.`receiver_id`
 ,old.`residence_country`
 ,old.`shipping`
 ,old.`shipping_discount`
 ,old.`transaction_subject`
 ,old.`txn_type`
 ,old.`verify_sign`
 ,old.`station_name`
 ,old.`campaign_code`
 ,old.`web_request_type`
 ,old.`other_text`
 ,old.`full_text`
 ,old.`warning_text`
 ,old.`error_text`
 ,old.`shipping_method`
 ,old.`initial_temp_location_id`
 ,old.`language_code`
);
end;|
delimiter ;



delimiter |
create trigger `sri_temp_locations_ji` after insert on `sri_temp_locations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_temp_locations_jn` where `location_id`=new.`location_id`
  and jn_datetime = xdate;

update `sri_temp_locations_jn` set jn_endtime = xdate
where `location_id`=new.`location_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_temp_locations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`location_id`
 ,`ip_address`
 ,`txn_id`
 ,`item_id`
 ,`start_time`
 ,`end_time`
 ,`language_code`
 ,`used`
 ,`errors`
 ,`overlaps`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`location_id`
 ,new.`ip_address`
 ,new.`txn_id`
 ,new.`item_id`
 ,new.`start_time`
 ,new.`end_time`
 ,new.`language_code`
 ,new.`used`
 ,new.`errors`
 ,new.`overlaps`
);
end;|

create trigger `sri_temp_locations_ju` after update on `sri_temp_locations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_temp_locations_jn` where `location_id`=old.`location_id`
  and jn_datetime = xdate;

update `sri_temp_locations_jn` set jn_endtime = xdate
where `location_id`=old.`location_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_temp_locations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`location_id`
 ,`ip_address`
 ,`txn_id`
 ,`item_id`
 ,`start_time`
 ,`end_time`
 ,`language_code`
 ,`used`
 ,`errors`
 ,`overlaps`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`location_id`
 ,new.`ip_address`
 ,new.`txn_id`
 ,new.`item_id`
 ,new.`start_time`
 ,new.`end_time`
 ,new.`language_code`
 ,new.`used`
 ,new.`errors`
 ,new.`overlaps`
);
end;|

create trigger `sri_temp_locations_jd` after delete on `sri_temp_locations` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_temp_locations_jn` where `location_id`=old.`location_id`
  and jn_datetime = xdate;

update `sri_temp_locations_jn` set jn_endtime = xdate
where `location_id`=old.`location_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_temp_locations_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`location_id`
 ,`ip_address`
 ,`txn_id`
 ,`item_id`
 ,`start_time`
 ,`end_time`
 ,`language_code`
 ,`used`
 ,`errors`
 ,`overlaps`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`location_id`
 ,old.`ip_address`
 ,old.`txn_id`
 ,old.`item_id`
 ,old.`start_time`
 ,old.`end_time`
 ,old.`language_code`
 ,old.`used`
 ,old.`errors`
 ,old.`overlaps`
);
end;|
delimiter ;

delimiter |
create trigger `sri_account_names_ji` after insert on `sri_account_names` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_account_names_jn` where `account_name`=new.`account_name`
  and jn_datetime = xdate;

update `sri_account_names_jn` set jn_endtime = xdate
where `account_name`=new.`account_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_account_names_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`account_name`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`account_name`
);
end;|

create trigger `sri_account_names_ju` after update on `sri_account_names` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_account_names_jn` where `account_name`=old.`account_name`
  and jn_datetime = xdate;

update `sri_account_names_jn` set jn_endtime = xdate
where `account_name`=old.`account_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_account_names_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`account_name`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`account_name`
);
end;|

create trigger `sri_account_names_jd` after delete on `sri_account_names` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_account_names_jn` where `account_name`=old.`account_name`
  and jn_datetime = xdate;

update `sri_account_names_jn` set jn_endtime = xdate
where `account_name`=old.`account_name`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_account_names_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`account_name`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`account_name`
);
end;|
delimiter ;


delimiter |
create trigger `sri_accounts_ji` after insert on `sri_accounts` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_accounts_jn` where `account_id`=new.`account_id`
  and jn_datetime = xdate;

update `sri_accounts_jn` set jn_endtime = xdate
where `account_id`=new.`account_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_accounts_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`account_id`
 ,`entity_id`
 ,`name`
 ,`currency`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`account_id`
 ,new.`entity_id`
 ,new.`name`
 ,new.`currency`
);
end;|

create trigger `sri_accounts_ju` after update on `sri_accounts` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_accounts_jn` where `account_id`=old.`account_id`
  and jn_datetime = xdate;

update `sri_accounts_jn` set jn_endtime = xdate
where `account_id`=old.`account_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_accounts_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`account_id`
 ,`entity_id`
 ,`name`
 ,`currency`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`account_id`
 ,new.`entity_id`
 ,new.`name`
 ,new.`currency`
);
end;|

create trigger `sri_accounts_jd` after delete on `sri_accounts` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_accounts_jn` where `account_id`=old.`account_id`
  and jn_datetime = xdate;

update `sri_accounts_jn` set jn_endtime = xdate
where `account_id`=old.`account_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;
insert into `sri_accounts_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`account_id`
 ,`entity_id`
 ,`name`
 ,`currency`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`account_id`
 ,old.`entity_id`
 ,old.`name`
 ,old.`currency`
);
end;|
delimiter ;

  
delimiter |
create trigger `sri_businesses_ji` after insert on `sri_businesses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_businesses_jn` where `business_id`=new.`business_id`
  and jn_datetime = xdate;

update `sri_businesses_jn` set jn_endtime = xdate
where `business_id`=new.`business_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate; 

insert into `sri_businesses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`business_id`
 ,`parent_business_id`
 ,`business_name`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`business_id`
 ,new.`parent_business_id`
 ,new.`business_name`
);
end;|

create trigger `sri_businesses_ju` after update on `sri_businesses` for each row begin
  declare xdate datetime; 
  set xdate = now();
delete from `sri_businesses_jn` where `business_id`=old.`business_id`
  and jn_datetime = xdate;

update `sri_businesses_jn` set jn_endtime = xdate
where `business_id`=old.`business_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_businesses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`business_id`
 ,`parent_business_id`
 ,`business_name`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`business_id`
 ,new.`parent_business_id`
 ,new.`business_name`
);
end;|

create trigger `sri_businesses_jd` after delete on `sri_businesses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_businesses_jn` where `business_id`=old.`business_id`
  and jn_datetime = xdate;

update `sri_businesses_jn` set jn_endtime = xdate
where `business_id`=old.`business_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_businesses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`business_id`
 ,`parent_business_id`
 ,`business_name`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`business_id`
 ,old.`parent_business_id`
 ,old.`business_name`
);
end;|
delimiter ;


delimiter |
create trigger `sri_currencies_ji` after insert on `sri_currencies` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_currencies_jn` where `currency`=new.`currency`
  and jn_datetime = xdate;

update `sri_currencies_jn` set jn_endtime = xdate
where `currency`=new.`currency`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_currencies_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`currency`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`currency`
);
end;|

create trigger `sri_currencies_ju` after update on `sri_currencies` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_currencies_jn` where `currency`=old.`currency`
  and jn_datetime = xdate;

update `sri_currencies_jn` set jn_endtime = xdate
where `currency`=old.`currency`
  and jn_datetime <= xdate
  and jn_endtime > xdate; 
insert into `sri_currencies_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`currency`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`currency`
);
end;|

create trigger `sri_currencies_jd` after delete on `sri_currencies` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_currencies_jn` where `currency`=old.`currency`
  and jn_datetime = xdate;

update `sri_currencies_jn` set jn_endtime = xdate
where `currency`=old.`currency`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_currencies_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`currency`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`currency`
);
end;|
delimiter ;
  

delimiter |
create trigger `sri_currency_conversion_rates_ji` after insert on `sri_currency_conversion_rates` for each row begin
  declare xdate datetime;
  set xdate = now();  
delete from `sri_currency_conversion_rates_jn` where `from_currency`=new.`from_currency`
  and `to_currency`=new.`to_currency`
  and jn_datetime = xdate;
 
update `sri_currency_conversion_rates_jn` set jn_endtime = xdate
where `from_currency`=new.`from_currency`
  and `to_currency`=new.`to_currency`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_currency_conversion_rates_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`from_currency`
 ,`to_currency`
 ,`rate`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`from_currency`
 ,new.`to_currency`
 ,new.`rate`
);
end;|
 
create trigger `sri_currency_conversion_rates_ju` after update on `sri_currency_conversion_rates` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_currency_conversion_rates_jn` where `from_currency`=old.`from_currency`
  and `to_currency`=old.`to_currency`
  and jn_datetime = xdate;
  
update `sri_currency_conversion_rates_jn` set jn_endtime = xdate
where `from_currency`=old.`from_currency`
  and `to_currency`=old.`to_currency`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_currency_conversion_rates_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`from_currency`
 ,`to_currency`
 ,`rate`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`from_currency`
 ,new.`to_currency`
 ,new.`rate`
);
end;|

create trigger `sri_currency_conversion_rates_jd` after delete on `sri_currency_conversion_rates` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_currency_conversion_rates_jn` where `from_currency`=old.`from_currency`
  and `to_currency`=old.`to_currency`
  and jn_datetime = xdate;

update `sri_currency_conversion_rates_jn` set jn_endtime = xdate
where `from_currency`=old.`from_currency`
  and `to_currency`=old.`to_currency`
  and jn_datetime <= xdate
  and jn_endtime > xdate;
  
insert into `sri_currency_conversion_rates_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`from_currency`
 ,`to_currency`
 ,`rate`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`from_currency`
 ,old.`to_currency`
 ,old.`rate`
);
end;|
delimiter ;

  
delimiter |
create trigger `sri_email_addresses_ji` after insert on `sri_email_addresses` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_email_addresses_jn` where `email_address`=new.`email_address`
  and jn_datetime = xdate;
  
update `sri_email_addresses_jn` set jn_endtime = xdate
where `email_address`=new.`email_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate; 

insert into `sri_email_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`email_address`
 ,`formatted_email_address`
 ,`paypal_id`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`email_address`
 ,new.`formatted_email_address`
 ,new.`paypal_id`
);
end;|
  
create trigger `sri_email_addresses_ju` after update on `sri_email_addresses` for each row begin
  declare xdate datetime; 
  set xdate = now();
delete from `sri_email_addresses_jn` where `email_address`=old.`email_address`
  and jn_datetime = xdate;
  
update `sri_email_addresses_jn` set jn_endtime = xdate
where `email_address`=old.`email_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_email_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`email_address`
 ,`formatted_email_address`
 ,`paypal_id`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`email_address`
 ,new.`formatted_email_address`
 ,new.`paypal_id`
);
end;|

create trigger `sri_email_addresses_jd` after delete on `sri_email_addresses` for each row begin
  declare xdate datetime; 
  set xdate = now();
delete from `sri_email_addresses_jn` where `email_address`=old.`email_address`
  and jn_datetime = xdate;
  
update `sri_email_addresses_jn` set jn_endtime = xdate
where `email_address`=old.`email_address`
  and jn_datetime <= xdate
  and jn_endtime > xdate;
 
insert into `sri_email_addresses_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`email_address`
 ,`formatted_email_address`
 ,`paypal_id`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`email_address`
 ,old.`formatted_email_address`
 ,old.`paypal_id`
);
end;|
delimiter ;
  

delimiter |
create trigger `sri_entities_ji` after insert on `sri_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_entities_jn` where `entity_id`=new.`entity_id`
  and jn_datetime = xdate;

update `sri_entities_jn` set jn_endtime = xdate
where `entity_id`=new.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;
 
insert into `sri_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`entity_type`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`entity_id`
 ,new.`entity_type`
);   
end;|

create trigger `sri_entities_ju` after update on `sri_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_entities_jn` where `entity_id`=old.`entity_id`
  and jn_datetime = xdate;

update `sri_entities_jn` set jn_endtime = xdate
where `entity_id`=old.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate; 
  
insert into `sri_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`entity_type`  
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`entity_id`
 ,new.`entity_type`
);
end;|

create trigger `sri_entities_jd` after delete on `sri_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_entities_jn` where `entity_id`=old.`entity_id`
  and jn_datetime = xdate;

update `sri_entities_jn` set jn_endtime = xdate
where `entity_id`=old.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;
  
insert into `sri_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`entity_type`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`entity_id`
 ,old.`entity_type`
);
end;|
delimiter ;
 

delimiter |
create trigger `sri_entity_types_ji` after insert on `sri_entity_types` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_entity_types_jn` where `entity_type`=new.`entity_type`
  and jn_datetime = xdate;

update `sri_entity_types_jn` set jn_endtime = xdate
where `entity_type`=new.`entity_type`
  and jn_datetime <= xdate
  and jn_endtime > xdate; 
  
insert into `sri_entity_types_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_type`
 ,`entity_type_table`
 ,`entity_type_view`
 ,`description`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`entity_type`
 ,new.`entity_type_table`  
 ,new.`entity_type_view`
 ,new.`description`
);
end;|
 
create trigger `sri_entity_types_ju` after update on `sri_entity_types` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_entity_types_jn` where `entity_type`=old.`entity_type`
  and jn_datetime = xdate;
 
update `sri_entity_types_jn` set jn_endtime = xdate
where `entity_type`=old.`entity_type`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_entity_types_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_type`
 ,`entity_type_table`
 ,`entity_type_view`
 ,`description`  
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`entity_type`
 ,new.`entity_type_table`
 ,new.`entity_type_view`
 ,new.`description`
);
end;|

create trigger `sri_entity_types_jd` after delete on `sri_entity_types` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_entity_types_jn` where `entity_type`=old.`entity_type`
  and jn_datetime = xdate;
  
update `sri_entity_types_jn` set jn_endtime = xdate
where `entity_type`=old.`entity_type`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_entity_types_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_type`
 ,`entity_type_table`
 ,`entity_type_view`
 ,`description`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`entity_type`
 ,old.`entity_type_table`
 ,old.`entity_type_view`
 ,old.`description`
);   
end;|
delimiter ;

  
delimiter |
create trigger `sri_paypal_accounts_ji` after insert on `sri_paypal_accounts` for each row begin
  declare xdate datetime; 
  set xdate = now();
delete from `sri_paypal_accounts_jn` where `paypal_id`=new.`paypal_id`
  and jn_datetime = xdate;
  
update `sri_paypal_accounts_jn` set jn_endtime = xdate
where `paypal_id`=new.`paypal_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_paypal_accounts_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`paypal_id`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`paypal_id`
);   
end;|

create trigger `sri_paypal_accounts_ju` after update on `sri_paypal_accounts` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_paypal_accounts_jn` where `paypal_id`=old.`paypal_id`
  and jn_datetime = xdate;

update `sri_paypal_accounts_jn` set jn_endtime = xdate
where `paypal_id`=old.`paypal_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_paypal_accounts_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`paypal_id`  
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`paypal_id`
);
end;|

create trigger `sri_paypal_accounts_jd` after delete on `sri_paypal_accounts` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_paypal_accounts_jn` where `paypal_id`=old.`paypal_id`
  and jn_datetime = xdate;
  
update `sri_paypal_accounts_jn` set jn_endtime = xdate
where `paypal_id`=old.`paypal_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_paypal_accounts_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`paypal_id`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`paypal_id`
);
end;|
delimiter ;


delimiter |
drop trigger `sri_taxable_entities_ji`|
drop trigger `sri_taxable_entities_ju`|
drop trigger `sri_taxable_entities_jd`|
create trigger `sri_taxable_entities_ji` after insert on `sri_taxable_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_taxable_entities_jn` where `entity_id`=new.`entity_id`
  and jn_datetime = xdate;

update `sri_taxable_entities_jn` set jn_endtime = xdate
where `entity_id`=new.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_taxable_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`us_taxpayer_id`
 ,`paypal_email_address`
 ,`affiliation_code`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`entity_id`
 ,new.`us_taxpayer_id`
 ,new.`paypal_email_address`
 ,new.`affiliation_code`
);
end;|

create trigger `sri_taxable_entities_ju` after update on `sri_taxable_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_taxable_entities_jn` where `entity_id`=old.`entity_id`
  and jn_datetime = xdate;

update `sri_taxable_entities_jn` set jn_endtime = xdate
where `entity_id`=old.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_taxable_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`us_taxpayer_id`
 ,`paypal_email_address`
 ,`affiliation_code`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`entity_id`
 ,new.`us_taxpayer_id`
 ,new.`paypal_email_address`
 ,new.`affiliation_code`
);
end;|

create trigger `sri_taxable_entities_jd` after delete on `sri_taxable_entities` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_taxable_entities_jn` where `entity_id`=old.`entity_id`
  and jn_datetime = xdate;

update `sri_taxable_entities_jn` set jn_endtime = xdate
where `entity_id`=old.`entity_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_taxable_entities_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`entity_id`
 ,`us_taxpayer_id`
 ,`paypal_email_address`
 ,`affiliation_code`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`entity_id`
 ,old.`us_taxpayer_id`
 ,old.`paypal_email_address`
 ,old.`affiliation_code`
);
end;|
delimiter ;
  
  
delimiter |
create trigger `sri_transactions_ji` after insert on `sri_transactions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_transactions_jn` where `transaction_id`=new.`transaction_id`
  and jn_datetime = xdate;

update `sri_transactions_jn` set jn_endtime = xdate
where `transaction_id`=new.`transaction_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_transactions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`transaction_id`
 ,`parent_transaction_id`
 ,`transaction_sequence`
 ,`entity_id`
 ,`from_currency`
 ,`to_currency`
 ,`amount_from`
 ,`amount_to`  
 ,`conversion_rate_used`
 ,`paypal_txn_id`
 ,`paypal_item_sequence`
 ,`account_name`
 ,`is_balanced`
 ,`ref_transaction_id`
 ,`notes`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`transaction_id`
 ,new.`parent_transaction_id`
 ,new.`transaction_sequence`
 ,new.`entity_id`
 ,new.`from_currency`
 ,new.`to_currency`
 ,new.`amount_from`
 ,new.`amount_to`
 ,new.`conversion_rate_used`
 ,new.`paypal_txn_id`
 ,new.`paypal_item_sequence`
 ,new.`account_name`
 ,new.`is_balanced`
 ,new.`ref_transaction_id`
 ,new.`notes`
);
end;|

create trigger `sri_transactions_ju` after update on `sri_transactions` for each row begin
  declare xdate datetime;  
  set xdate = now();
delete from `sri_transactions_jn` where `transaction_id`=old.`transaction_id`
  and jn_datetime = xdate;

update `sri_transactions_jn` set jn_endtime = xdate
where `transaction_id`=old.`transaction_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_transactions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`transaction_id`
 ,`parent_transaction_id`
 ,`transaction_sequence`
 ,`entity_id`
 ,`from_currency`
 ,`to_currency`
 ,`amount_from`
 ,`amount_to`
 ,`conversion_rate_used`
 ,`paypal_txn_id`
 ,`paypal_item_sequence`
 ,`account_name`
 ,`is_balanced`
 ,`ref_transaction_id`
 ,`notes`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`transaction_id`
 ,new.`parent_transaction_id`
 ,new.`transaction_sequence`
 ,new.`entity_id`
 ,new.`from_currency`
 ,new.`to_currency`
 ,new.`amount_from`
 ,new.`amount_to` 
 ,new.`conversion_rate_used`
 ,new.`paypal_txn_id`
 ,new.`paypal_item_sequence`
 ,new.`account_name`  
 ,new.`is_balanced`
 ,new.`ref_transaction_id`
 ,new.`notes`
);
end;|
  
create trigger `sri_transactions_jd` after delete on `sri_transactions` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_transactions_jn` where `transaction_id`=old.`transaction_id`
  and jn_datetime = xdate;

update `sri_transactions_jn` set jn_endtime = xdate
where `transaction_id`=old.`transaction_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;
 
insert into `sri_transactions_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`transaction_id`
 ,`parent_transaction_id`
 ,`transaction_sequence`
 ,`entity_id`
 ,`from_currency`
 ,`to_currency`
 ,`amount_from`
 ,`amount_to`
 ,`conversion_rate_used`
 ,`paypal_txn_id`
 ,`paypal_item_sequence`
 ,`account_name`
 ,`is_balanced`
 ,`ref_transaction_id`
 ,`notes`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`transaction_id`
 ,old.`parent_transaction_id`
 ,old.`transaction_sequence`
 ,old.`entity_id`
 ,old.`from_currency`
 ,old.`to_currency`
 ,old.`amount_from`
 ,old.`amount_to` 
 ,old.`conversion_rate_used`
 ,old.`paypal_txn_id`   
 ,old.`paypal_item_sequence`
 ,old.`account_name`
 ,old.`is_balanced`
 ,old.`ref_transaction_id`
 ,old.`notes`  
);
end;|
delimiter ;
 

						      
delimiter |
create trigger `sri_people_ji` after insert on `sri_people` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_people_jn` where `person_id`=new.`person_id`
  and jn_datetime = xdate;

update `sri_people_jn` set jn_endtime = xdate
where `person_id`=new.`person_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_people_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`person_id`
 ,`first_name`
 ,`last_name`
 ,`email_address`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`person_id`
 ,new.`first_name`
 ,new.`last_name`
 ,new.`email_address`
);
end;|

create trigger `sri_people_ju` after update on `sri_people` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_people_jn` where `person_id`=old.`person_id`
  and jn_datetime = xdate;

update `sri_people_jn` set jn_endtime = xdate
where `person_id`=old.`person_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_people_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`person_id`
 ,`first_name`
 ,`last_name`
 ,`email_address`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`person_id`
 ,new.`first_name`
 ,new.`last_name`
 ,new.`email_address`
);
end;|

create trigger `sri_people_jd` after delete on `sri_people` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `sri_people_jn` where `person_id`=old.`person_id`
  and jn_datetime = xdate;

update `sri_people_jn` set jn_endtime = xdate
where `person_id`=old.`person_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `sri_people_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`person_id`
 ,`first_name`
 ,`last_name`
 ,`email_address`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`person_id`
 ,old.`first_name`
 ,old.`last_name`
 ,old.`email_address`
);
end;|
delimiter ;

