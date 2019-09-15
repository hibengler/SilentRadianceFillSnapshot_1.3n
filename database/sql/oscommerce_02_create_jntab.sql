/* 
select
concat(concat('call ddl_create_journal_table(''oscommerce'',''',table_name),''');')
from information_schema.tables
where table_schema='oscommerce'
and table_name not like '_jn';


*/

call ddl_create_journal_table('oscommerce','action_recorder');
call ddl_create_journal_table('oscommerce','address_book');
call ddl_create_journal_table('oscommerce','address_format');
call ddl_create_journal_table('oscommerce','administrators');
call ddl_create_journal_table('oscommerce','banners');
call ddl_create_journal_table('oscommerce','banners_history');
call ddl_create_journal_table('oscommerce','categories');
call ddl_create_journal_table('oscommerce','categories_description');
call ddl_create_journal_table('oscommerce','configuration');
call ddl_create_journal_table('oscommerce','configuration_group');
//call ddl_create_journal_table('oscommerce','counter');/**/
//call ddl_create_journal_table('oscommerce','counter_history');/**/
call ddl_create_journal_table('oscommerce','countries');
call ddl_create_journal_table('oscommerce','currencies');
call ddl_create_journal_table('oscommerce','customers');
call ddl_create_journal_table('oscommerce','customers_basket');
call ddl_create_journal_table('oscommerce','customers_basket_attributes');
call ddl_create_journal_table('oscommerce','customers_info');
call ddl_create_journal_table('oscommerce','geo_zones');
call ddl_create_journal_table('oscommerce','languages');
call ddl_create_journal_table('oscommerce','manufacturers');
call ddl_create_journal_table('oscommerce','manufacturers_info');
call ddl_create_journal_table('oscommerce','newsletters');
call ddl_create_journal_table('oscommerce','orders');
call ddl_create_journal_table('oscommerce','orders_products');
call ddl_create_journal_table('oscommerce','orders_products_attributes');
call ddl_create_journal_table('oscommerce','orders_products_download');
call ddl_create_journal_table('oscommerce','orders_status');
//call ddl_create_journal_table('oscommerce','orders_status_history'); /**/
Create table `products_attributes_jn` (
jn_operation                    varchar(1)          NOT NULL,
jn_oracle_user                  varchar(30)         NOT NULL,
jn_datetime                     datetime  NOT NULL,
jn_endtime                     datetime not NULL,
jn_notes                        varchar(240)  , 
jn_appln                        varchar(30)   ,
jn_session                      int           
 ,`products_attributes_id` int(11)  not null
  ,`products_id` int(11)  not null
   ,`options_id` int(11)  not null
    ,`options_values_id` int(11)  not null
     ,`options_values_price` decimal(15,4)  not null
  ,`price_prefix` char(1)  not null
, constraint `products_attributes_jn_pk` primary key (
`products_attributes_id`,jn_datetime)
, constraint `products_attributes_jn_uk1` unique (
`products_attributes_id`,jn_endtime)
);
call ddl_create_journal_table('oscommerce','orders_total');
call ddl_create_journal_table('oscommerce','products');
call ddl_create_journal_table('oscommerce','products_attributes');
call ddl_create_journal_table('oscommerce','products_attributes_download');
call ddl_create_journal_table('oscommerce','products_description');
call ddl_create_journal_table('oscommerce','products_images');
call ddl_create_journal_table('oscommerce','products_notifications');
call ddl_create_journal_table('oscommerce','products_options');
call ddl_create_journal_table('oscommerce','products_options_values');
call ddl_create_journal_table('oscommerce','products_options_values_to_products_options');
call ddl_create_journal_table('oscommerce','products_to_categories');
call ddl_create_journal_table('oscommerce','reviews');
call ddl_create_journal_table('oscommerce','reviews_description');
call ddl_create_journal_table('oscommerce','sec_directory_whitelist');
call ddl_create_journal_table('oscommerce','sessions');
call ddl_create_journal_table('oscommerce','specials');
call ddl_create_journal_table('oscommerce','tax_class');
call ddl_create_journal_table('oscommerce','tax_rates');
call ddl_create_journal_table('oscommerce','whos_online');
call ddl_create_journal_table('oscommerce','zones');
call ddl_create_journal_table('oscommerce','zones_to_geo_zones');

