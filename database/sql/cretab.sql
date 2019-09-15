/*
drop table sr_sessions;
drop table sr_user_web_programs;
drop table sr_web_programs;
drop table sr_user_ip_addresses;
drop table sr_users;
drop table sr_name_ip_addresses;
drop table sr_ip_addresses;
drop table sr_names;
*/

create table sr_names (
dns_name varchar(200) not null,
description varchar(2000),
last_time_checked datetime not null,
constraint sr_names_pk primary key(dns_name)
);


create table sr_ip_addresses (
ip_address varchar(45) not null, /* for ipv6*/
reverse_dns varchar(200),
last_time_checked datetime not null,
constraint sr_ip_addresses_pk primary key(ip_address),
constraint sr_ip_addresses_fk1 foreign key(reverse_dns) references sr_names(dns_name)
);

create table sr_name_ip_addresses (
dns_name varchar(200) not null,
ip_address varchar(45) not null,
last_time_checked datetime not null,
constraint sr_name_ip_addresses_pk primary key (dns_name,ip_address),
constraint sr_name_ip_addresses_uk1 unique (ip_address,dns_name),
constraint sr_name_ip_addresses_fk1 foreign key (ip_address) references sr_ip_addresses(ip_address),
constraint sr_name_ip_addresses_fk2 foreign key (dns_name) references sr_names(dns_name)
);
  
  
create table sr_users (
user_code varchar(45) not null,
cred_level float,
notes varchar(2000),
constraint sr_users_pk primary key (user_code)
);

create table sr_user_ip_addresses (
user_code varchar(45) not null,
ip_address varchar(45) not null,
cred_level float,
notes varchar(2000),
constraint sr_user_ip_addresses_pk primary key (user_code,ip_address),
constraint sr_user_ip_addresses_uk unique(ip_address,user_code),
constraint sr_user_ip_addresses_fk1 foreign key (user_code) references sr_users(user_code),
constraint sr_user_ip_addresses_fk2 foreign key (ip_address) references sr_ip_addresses(ip_address)
);

create table sr_web_programs (
dns_name varchar(200) not null,
program_name varchar(200) not null,
description varchar(2000),
constraint sr_web_programs_pk primary key(dns_name,program_name),
constraint sr_web_programs_fk1 foreign key(dns_name) references sr_names(dns_name)
);

  

create table sr_user_web_programs (
user_code varchar(45) not null,
ip_address varchar(45) not null,
dns_name varchar(200) not null,
program_name varchar(200) not null,
constraint sr_ip_address_web_programs_pk primary key(user_code,ip_address,dns_name,program_name),
constraint sr_ip_address_web_programs_fk1 foreign key(dns_name,program_name) references sr_web_programs(dns_name,program_name),
constraint sr_ip_address_web_programs_fk2 foreign key(user_code,ip_address) references sr_user_ip_addresses(user_code,ip_address)
);


create table sr_sessions (
session_code varchar(30) not null,
start_unix_time bigint not null,
end_unix_time bigint not null,
start_position bigint not null,
end_position bigint not null,
user_code varchar(45) not null,
ip_address  varchar(45) not null,  
dns_name varchar(200) not null,  
program_name varchar(200) not null,  
constraint sr_sessions_pk primary key(session_code,start_unix_time),
constraint sr_sessions_uk unique key(start_unix_time,session_code),
constraint sr_sessions_fk1 foreign key (user_code,ip_address,dns_name,program_name) 
references
  sr_user_web_programs(user_code,ip_address,dns_name,program_name)
);


create index sr_sessions_i1 on sr_sessions (end_unix_time);














 create table sr_paypal_raw_transactions (transaction_code varchar(40) not null primary key,
 status varchar(20) not null,
 last datetime not null,
 stuff text,
 build_status varchar(20) not null,
 mail_status varchar(20) not null);

create index sr_paypal_raw_transactions_i1 on sr_paypal_raw_transactions(status,last);
create index sr_paypal_raw_transactions_i2 on sr_paypal_raw_transactions(payer_email);
