

/*
drop table sri_raw_order_fulfilment_items;
drop table sri_raw_paypal_order_items;
drop table sri_raw_paypal_orders;
*/

create table sri_raw_paypal_orders (
txn_id varchar(40) not null,
business varchar(40),
charset varchar(40),
custom varchar(200),
discount decimal(10,2),
first_name varchar(200),
insurance_amount decimal(10,2),
ipn_track_id varchar(40),
item_name varchar(400),
item_number bigint,
last_name varchar(100),
mc_currency varchar(40),
mc_fee decimal(10,2),
mc_gross decimal(10,2),
notify_version varchar(20),
payer_email varchar(255),
payer_id varchar(20),
payer_status varchar(20),
payment_date_text varchar(100),
payment_date datetime,
payment_fee decimal(10,2),
payment_gross decimal(10,2),
payment_status varchar(30),
payment_type varchar(20),
protection_eligibility varchar(40),
quantity decimal(10,2),
receipt_id varchar(40),
receiver_email varchar(255),
receiver_id varchar(20),
residence_country varchar(5),
shipping decimal(10,2),
shipping_discount decimal(10,2),
shipping_method varchar(40),
transaction_subject varchar(100),
txn_type varchar(20),
verify_sign varchar(255)
,
station_name varchar(100),
campaign_code varchar(20),
web_request_type varchar(20),
initial_temp_location_id varchar(60),
language_code varchar(10)
,
other_text text,
full_text text,
warning_text text,
error_text text,
constraint sri_paypal_order_pk primary key(txn_id)
);


create table sri_raw_paypal_order_items (
txn_id varchar(40) not null, 
item_sequence int,
item_number bigint not null,
constraint sri_raw_paypal_order_items_pk primary key(txn_id,item_sequence),
constraint sri_raw_paypal_order_items_fk foreign key(txn_id)
  references sri_raw_paypal_orders(txn_id)
);

/* note -
station_name is derived from custom - field "s"
campaign_code is derived from custom - field "g"
web_request_type is also serived - field t - could be buynow, or could be regular
full_text - built from the info
other_text - built from anything missing in the raw table as built in
*/


create table sri_raw_order_fulfillment_items (
txn_id varchar(40) not null,
item_sequence int not null,
display_sequence int not null,
item_number bigint, /* could be different than order_items because bom */
fulfillment_type varchar(20) not null, /* flac, wav, mp3, jpg, mp4 */
song_info_station_name varchar(100),
song_info_file_name varchar(250),
album_cover_file_name varchar(250),
vault_file_name varchar(250),
vault_description varchar(2000),
created_date datetime,
constraint sri_raw_order_fulfillment_items_pk primary key(txn_id,display_sequence),
constraint sri_raw_order_fulfillment_items_fk1 foreign key(txn_id) references sri_raw_paypal_orders(txn_id),
constraint sri_raw_order_fulfillment_items_fk2 foreign key(txn_id,item_sequence) 
  references sri_raw_paypal_order_items(txn_id,item_sequence)
);





create table sri_temp_locations (
location_id varchar(60) not null,
ip_address varchar(45) not null,
txn_id varchar(40),
item_id bigint,
start_time datetime not null,
end_time datetime not null,
language_code varchar(10),
used int not null, /* number of times attempted to use - should be 0 or 1 */
errors int not null, /* number of errors - could be more than 1 I guess */
overlaps int   /* number of overlaps by ip_address, computed LAAATER */
,
constraint sri_temp_locations_pk primary key(location_id),
constraint sri_temp_locations_fk1 foreign key(txn_id)  references  sri_raw_paypal_orders(txn_id),
constraint sri_temp_locations_fk2 foreign key(ip_address)  references  sr_ip_addresses(ip_address)
);
