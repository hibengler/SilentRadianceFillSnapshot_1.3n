

create table hib_entity_types (
entity_type varchar(30) not null,
description varchar(2000) not null);

create table hib_entities (
entity_id bigint not null,
entity_name varchar(700)
);

create table hib_types_of_entities (
entity_id bigint not null
entity_type varchar(30) not null
);

create table hib_entity_attribute_datatypes (
datatype_code varchar(100)
);

create table hib_entity_attributes (
attribute_id bigint not null,
attribute_name varchar(400) not null
);


create table hib_entity_values (
entity_value_id bigint not null,
entity_id bigint not null,
attribute_id bigint not null,
sequence int not null,
id_value bigint,
int_value int,
decimal_value decimal(20,6),
char_valuex varchar(700),
char_value text,
blob_value blob
);


create table hib_entity_clarifications (
entity_clarification_id bigint not null,
old_entity_id bigint not null,
new_entity_id biging not null
);

create table hib_countries (
country_id bigint not null,



create table hib_businesses (
business_id bigint not null,
taxpayer_id varchar(20),
duns_number varchar(30),
parent_business_id bigint,
business_name varchar(100));

create table hib_addresses (
address_id bigint not null,
entity_id bigint not null,
address_seq  bigint not null,
address_1 varchar(100),
address_2 varchar(100),
city varchar(40),
state varchar(20),
zip varchar(12),
country_id bigint
);

create table hib_email_addresses (
email_address_id bigint not null,
entity_id bigint not null,
address_seq  bigint not null,
email_address varchar(200) not null,
nice_email_address varchar(400) not null);

create hib_person (
person_id bigint not null,
first_name varchar(100) not null,
middle_name1 varchar(100) not null,
middle_name2 varchar(100) not null,
last_name varchar(100) not null,
person_name_seq int not null
);

create table hib_business_contacts (
contact_id bigint not null,
business_id bigint not null
);


create table hib_social_network (
social_network_id bigint not null
);

create table hib_social_network_person (
social_network_person_id bigint not null,
social_network_id bigint not null,
person_id bigint not null
);


create table hib_customer (
customer_id bigint not null
);

create table hib_transaction (
transacrion_id bigint,
primary_from_id bigint not null,
primary_to_id bigint,
primary_amount decimal(20,6),
primary currency varchar(4),

