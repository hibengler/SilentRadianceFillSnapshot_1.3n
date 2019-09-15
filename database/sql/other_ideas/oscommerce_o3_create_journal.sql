/*
select
concat(concat('call ddl_create_journal_trigger(''oscommerce'',''',table_name),''');')
from information_schema.tables
where table_schema='oscommerce'
and table_name not like '%_jn';


*/



call ddl_create_journal_trigger('oscommerce','action_recorder');
call ddl_create_journal_trigger('oscommerce','address_book');
call ddl_create_journal_trigger('oscommerce','address_format');
call ddl_create_journal_trigger('oscommerce','administrators');
call ddl_create_journal_trigger('oscommerce','banners');
call ddl_create_journal_trigger('oscommerce','banners_history');
call ddl_create_journal_trigger('oscommerce','categories');
call ddl_create_journal_trigger('oscommerce','categories_description');
call ddl_create_journal_trigger('oscommerce','configuration');
call ddl_create_journal_trigger('oscommerce','configuration_group');
call ddl_create_journal_trigger('oscommerce','counter');
call ddl_create_journal_trigger('oscommerce','counter_history');
call ddl_create_journal_trigger('oscommerce','countries');
call ddl_create_journal_trigger('oscommerce','currencies');
call ddl_create_journal_trigger('oscommerce','customers');
call ddl_create_journal_trigger('oscommerce','customers_basket');
call ddl_create_journal_trigger('oscommerce','customers_basket_attributes');
call ddl_create_journal_trigger('oscommerce','customers_info');
call ddl_create_journal_trigger('oscommerce','geo_zones');
call ddl_create_journal_trigger('oscommerce','languages');
call ddl_create_journal_trigger('oscommerce','manufacturers');
call ddl_create_journal_trigger('oscommerce','manufacturers_info');
call ddl_create_journal_trigger('oscommerce','newsletters');
call ddl_create_journal_trigger('oscommerce','orders');
call ddl_create_journal_trigger('oscommerce','orders_products');
call ddl_create_journal_trigger('oscommerce','orders_products_attributes');
call ddl_create_journal_trigger('oscommerce','orders_products_download');
call ddl_create_journal_trigger('oscommerce','orders_status');
call ddl_create_journal_trigger('oscommerce','orders_status_history');
call ddl_create_journal_trigger('oscommerce','orders_total');
call ddl_create_journal_trigger('oscommerce','products');
call ddl_create_journal_trigger('oscommerce','products_attributes');
call ddl_create_journal_trigger('oscommerce','products_attributes_download');
call ddl_create_journal_trigger('oscommerce','products_description');
call ddl_create_journal_trigger('oscommerce','products_images');
call ddl_create_journal_trigger('oscommerce','products_notifications');
call ddl_create_journal_trigger('oscommerce','products_options');
call ddl_create_journal_trigger('oscommerce','products_options_values');
call ddl_create_journal_trigger('oscommerce','products_options_values_to_products_options');
call ddl_create_journal_trigger('oscommerce','products_to_categories');
call ddl_create_journal_trigger('oscommerce','reviews');
call ddl_create_journal_trigger('oscommerce','reviews_description');
call ddl_create_journal_trigger('oscommerce','sec_directory_whitelist');
call ddl_create_journal_trigger('oscommerce','sessions');
call ddl_create_journal_trigger('oscommerce','specials');
call ddl_create_journal_trigger('oscommerce','tax_class');
call ddl_create_journal_trigger('oscommerce','tax_rates');
call ddl_create_journal_trigger('oscommerce','whos_online');
call ddl_create_journal_trigger('oscommerce','zones');
call ddl_create_journal_trigger('oscommerce','zones_to_geo_zones');



*/





delimiter |
create trigger `action_recorder_ji` after insert on `action_recorder` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `action_recorder_jn` where `id`=new.`id`
  and jn_datetime = xdate;

update `action_recorder_jn` set jn_endtime = xdate
where `id`=new.`id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `action_recorder_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`id`
 ,`module`
 ,`user_id`
 ,`user_name`
 ,`identifier`
 ,`success`
 ,`date_added`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`id`
 ,new.`module`
 ,new.`user_id`
 ,new.`user_name`
 ,new.`identifier`
 ,new.`success`
 ,new.`date_added`
);
end;|

create trigger `action_recorder_ju` after update on `action_recorder` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `action_recorder_jn` where `id`=old.`id`
  and jn_datetime = xdate;

update `action_recorder_jn` set jn_endtime = xdate
where `id`=old.`id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `action_recorder_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`id`
 ,`module`
 ,`user_id`
 ,`user_name`
 ,`identifier`
 ,`success`
 ,`date_added`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`id`
 ,new.`module`
 ,new.`user_id`
 ,new.`user_name`
 ,new.`identifier`
 ,new.`success`
 ,new.`date_added`
);
end;|

create trigger `action_recorder_jd` after delete on `action_recorder` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `action_recorder_jn` where `id`=old.`id`
  and jn_datetime = xdate;

update `action_recorder_jn` set jn_endtime = xdate
where `id`=old.`id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `action_recorder_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`id`
 ,`module`
 ,`user_id`
 ,`user_name`
 ,`identifier`
 ,`success`
 ,`date_added`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`id`
 ,old.`module`
 ,old.`user_id`
 ,old.`user_name`
 ,old.`identifier`
 ,old.`success`
 ,old.`date_added`
);
end;|
delimiter ;




delimiter |
create trigger `address_book_ji` after insert on `address_book` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `address_book_jn` where `address_book_id`=new.`address_book_id`
  and jn_datetime = xdate;

update `address_book_jn` set jn_endtime = xdate
where `address_book_id`=new.`address_book_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `address_book_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`address_book_id`
 ,`customers_id`
 ,`entry_gender`
 ,`entry_company`
 ,`entry_firstname`
 ,`entry_lastname`
 ,`entry_street_address`
 ,`entry_suburb`
 ,`entry_postcode`
 ,`entry_city`
 ,`entry_state`
 ,`entry_country_id`
 ,`entry_zone_id`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`address_book_id`
 ,new.`customers_id`
 ,new.`entry_gender`
 ,new.`entry_company`
 ,new.`entry_firstname`
 ,new.`entry_lastname`
 ,new.`entry_street_address`
 ,new.`entry_suburb`
 ,new.`entry_postcode`
 ,new.`entry_city`
 ,new.`entry_state`
 ,new.`entry_country_id`
 ,new.`entry_zone_id`
);
end;|

create trigger `address_book_ju` after update on `address_book` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `address_book_jn` where `address_book_id`=old.`address_book_id`
  and jn_datetime = xdate;

update `address_book_jn` set jn_endtime = xdate
where `address_book_id`=old.`address_book_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `address_book_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`address_book_id`
 ,`customers_id`
 ,`entry_gender`
 ,`entry_company`
 ,`entry_firstname`
 ,`entry_lastname`
 ,`entry_street_address`
 ,`entry_suburb`
 ,`entry_postcode`
 ,`entry_city`
 ,`entry_state`
 ,`entry_country_id`
 ,`entry_zone_id`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`address_book_id`
 ,new.`customers_id`
 ,new.`entry_gender`
 ,new.`entry_company`
 ,new.`entry_firstname`
 ,new.`entry_lastname`
 ,new.`entry_street_address`
 ,new.`entry_suburb`
 ,new.`entry_postcode`
 ,new.`entry_city`
 ,new.`entry_state`
 ,new.`entry_country_id`
 ,new.`entry_zone_id`
);
end;|

create trigger `address_book_jd` after delete on `address_book` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `address_book_jn` where `address_book_id`=old.`address_book_id`
  and jn_datetime = xdate;

update `address_book_jn` set jn_endtime = xdate
where `address_book_id`=old.`address_book_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `address_book_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`address_book_id`
 ,`customers_id`
 ,`entry_gender`
 ,`entry_company`
 ,`entry_firstname`
 ,`entry_lastname`
 ,`entry_street_address`
 ,`entry_suburb`
 ,`entry_postcode`
 ,`entry_city`
 ,`entry_state`
 ,`entry_country_id`
 ,`entry_zone_id`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`address_book_id`
 ,old.`customers_id`
 ,old.`entry_gender`
 ,old.`entry_company`
 ,old.`entry_firstname`
 ,old.`entry_lastname`
 ,old.`entry_street_address`
 ,old.`entry_suburb`
 ,old.`entry_postcode`
 ,old.`entry_city`
 ,old.`entry_state`
 ,old.`entry_country_id`
 ,old.`entry_zone_id`
);
end;|
delimiter ;






delimiter |
create trigger `address_format_ji` after insert on `address_format` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `address_format_jn` where `address_format_id`=new.`address_format_id`
  and jn_datetime = xdate;

update `address_format_jn` set jn_endtime = xdate
where `address_format_id`=new.`address_format_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `address_format_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`address_format_id`
 ,`address_format`
 ,`address_summary`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`address_format_id`
 ,new.`address_format`
 ,new.`address_summary`
);
end;|

create trigger `address_format_ju` after update on `address_format` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `address_format_jn` where `address_format_id`=old.`address_format_id`
  and jn_datetime = xdate;

update `address_format_jn` set jn_endtime = xdate
where `address_format_id`=old.`address_format_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `address_format_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`address_format_id`
 ,`address_format`
 ,`address_summary`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`address_format_id`
 ,new.`address_format`
 ,new.`address_summary`
);
end;|

create trigger `address_format_jd` after delete on `address_format` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `address_format_jn` where `address_format_id`=old.`address_format_id`
  and jn_datetime = xdate;

update `address_format_jn` set jn_endtime = xdate
where `address_format_id`=old.`address_format_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `address_format_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`address_format_id`
 ,`address_format`
 ,`address_summary`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`address_format_id`
 ,old.`address_format`
 ,old.`address_summary`
);
end;|
delimiter ;





delimiter |
create trigger `administrators_ji` after insert on `administrators` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `administrators_jn` where `id`=new.`id`
  and jn_datetime = xdate;

update `administrators_jn` set jn_endtime = xdate
where `id`=new.`id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `administrators_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`id`
 ,`user_name`
 ,`user_password`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`id`
 ,new.`user_name`
 ,new.`user_password`
);
end;|

create trigger `administrators_ju` after update on `administrators` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `administrators_jn` where `id`=old.`id`
  and jn_datetime = xdate;

update `administrators_jn` set jn_endtime = xdate
where `id`=old.`id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `administrators_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`id`
 ,`user_name`
 ,`user_password`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`id`
 ,new.`user_name`
 ,new.`user_password`
);
end;|

create trigger `administrators_jd` after delete on `administrators` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `administrators_jn` where `id`=old.`id`
  and jn_datetime = xdate;

update `administrators_jn` set jn_endtime = xdate
where `id`=old.`id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `administrators_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`id`
 ,`user_name`
 ,`user_password`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`id`
 ,old.`user_name`
 ,old.`user_password`
);
end;|
delimiter ;




create trigger `banners_ji` after insert on `banners` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `banners_jn` where `banners_id`=new.`banners_id`
  and jn_datetime = xdate;

update `banners_jn` set jn_endtime = xdate
where `banners_id`=new.`banners_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `banners_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`banners_id`
 ,`banners_title`
 ,`banners_url`
 ,`banners_image`
 ,`banners_group`
 ,`banners_html_text`
 ,`expires_impressions`
 ,`expires_date`
 ,`date_scheduled`
 ,`date_added`
 ,`date_status_change`
 ,`status`
) values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`banners_id`
 ,new.`banners_title`
 ,new.`banners_url`
 ,new.`banners_image`
 ,new.`banners_group`
 ,new.`banners_html_text`
 ,new.`expires_impressions`
 ,new.`expires_date`
 ,new.`date_scheduled`
 ,new.`date_added`
 ,new.`date_status_change`
 ,new.`status`
);
end;|

create trigger `banners_ju` after update on `banners` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `banners_jn` where `banners_id`=old.`banners_id`
  and jn_datetime = xdate;

update `banners_jn` set jn_endtime = xdate
where `banners_id`=old.`banners_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `banners_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`banners_id`
 ,`banners_title`
 ,`banners_url`
 ,`banners_image`
 ,`banners_group`
 ,`banners_html_text`
 ,`expires_impressions`
 ,`expires_date`
 ,`date_scheduled`
 ,`date_added`
 ,`date_status_change`
 ,`status`
) values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,new.`banners_id`
 ,new.`banners_title`
 ,new.`banners_url`
 ,new.`banners_image`
 ,new.`banners_group`
 ,new.`banners_html_text`
 ,new.`expires_impressions`
 ,new.`expires_date`
 ,new.`date_scheduled`
 ,new.`date_added`
 ,new.`date_status_change`
 ,new.`status`
);
end;|

create trigger `banners_jd` after delete on `banners` for each row begin
  declare xdate datetime;
  set xdate = now();
delete from `banners_jn` where `banners_id`=old.`banners_id`
  and jn_datetime = xdate;

update `banners_jn` set jn_endtime = xdate
where `banners_id`=old.`banners_id`
  and jn_datetime <= xdate
  and jn_endtime > xdate;

insert into `banners_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session
 ,`banners_id`
 ,`banners_title`
 ,`banners_url`
 ,`banners_image`
 ,`banners_group`
 ,`banners_html_text`
 ,`expires_impressions`
 ,`expires_date`
 ,`date_scheduled`
 ,`date_added`
 ,`date_status_change`
 ,`status`
) values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()
 ,old.`banners_id`
 ,old.`banners_title`
 ,old.`banners_url`
 ,old.`banners_image`
 ,old.`banners_group`
 ,old.`banners_html_text`
 ,old.`expires_impressions`
 ,old.`expires_date`
 ,old.`date_scheduled`
 ,old.`date_added`
 ,old.`date_status_change`
 ,old.`status`
);
end;|
delimiter ;

