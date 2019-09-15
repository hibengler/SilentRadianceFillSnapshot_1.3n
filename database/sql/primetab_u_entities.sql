



insert into sri_entity_types (entity_type,entity_type_table,entity_type_view,description)
values ('taxable','sri_taxable_entities',null,'Taxable Entity');
insert into sri_entity_types (entity_type,entity_type_table,entity_type_view,description)
values ('entity','sri_entities',null,'Generic Entity');
insert into sri_entity_types (entity_type,entity_type_table,entity_type_view,description)
values ('person','sri_people',null,'A person');

insert into sri_entity_types (entity_type,entity_type_table,entity_type_view,description)
values ('business','sri_businesses',null,'A business, or part of business');

insert into sri_entity_types (entity_type,entity_type_table,entity_type_view,description)
values ('account','sri_accounts',null,'An internal account for the business or person.');




insert into sri_currencies(currency) values ('USD');


insert into sri_currency_conversion_rates(from_currency,to_currency,rate)
values ('USD','USD',1.);



insert into sri_account_names(account_name) values ('Orders');
insert into sri_account_names(account_name) values ('Gross');
insert into sri_account_names(account_name) values ('Puerto Rico Sales Tax');
insert into sri_account_names(account_name) values ('Sales Comission');
insert into sri_account_names(account_name) values ('Proceeds');

insert into sri_paypal_accounts values ('7RMPM6NN4UXVS');
insert into sri_paypal_accounts values ('CDKPH6G89NM78');


insert into sri_email_addresses (email_address,formatted_email_address,paypal_id)
values ('h@killercool.net','h@killercool.net','7RMPM6NN4UXVS');

insert into sri_email_addresses (email_address,formatted_email_address,paypal_id)
values ('sr@killercool.net','sr@killercool.net','CDKPH6G89NM78');

insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');
insert into sri_entities(entity_type) values('entity');

/* first 10 reserved */

insert into sri_entities(entity_type) values('business');
insert into sri_taxable_entities(entity_id,us_taxpayer_id,paypal_email_address) 
values (11,'390885727','sr@killercool.net');

insert into sri_businesses(business_id,business_name) values (11,'DBA Killer Cool East');

insert into sri_entities(entity_type) values ('account');
insert into sri_accounts(account_id,entity_id,name,currency) values (15,11,'Proceeds','USD');



insert into sri_entities(entity_type) values('person');
insert into sri_taxable_entities(entity_id,us_taxpayer_id,paypal_email_address) 
values (14,null,'h@killercool.net');

insert into sri_people(person_id,first_name,last_name,email_address)
values (14,'Hibbard','Engler','h@killercool.net');

insert into sri_entities(entity_type) values ('account');
insert into sri_accounts(account_id,entity_id,name,currency) values (16,14,'Orders','USD');

insert into sri_email_addresses(email_address,formatted_email_address) values 
('primerecords@yahoo.com','primerecords@yahoo.com');

insert into sri_entities(entity_type) values('business');
insert into sri_taxable_entities(entity_id,us_taxpayer_id,paypal_email_address,affiliation_code)
values (17,NULL,'primerecords@yahoo.com','Prime');
insert into sri_businesses(business_id,business_name) values (17,'Prime Records');


insert into sri_email_addresses(email_address,formatted_email_address) values 
('spook@geomagnetic.tv','spook@geomagnetic.tv');
insert into sri_entities(entity_type) values('business');
insert into sri_taxable_entities(entity_id,us_taxpayer_id,paypal_email_address,affiliation_code)
values (18,NULL,'spook@geomagnetic.tv','Geomagnetic');
insert into sri_businesses(business_id,business_name) values (18,'Geomagnetic');

insert into sri_entities(entity_type) values ('account');
insert into sri_accounts(account_id,entity_id,name,currency) values (19,17,'Proceeds','USD');

insert into sri_entities(entity_type) values ('account');
insert into sri_accounts(account_id,entity_id,name,currency) values (20,18,'Proceeds','USD');


insert into sri_entities(entity_type) values ('account');
insert into sri_accounts(account_id,entity_id,name,currency) values (21,11,'Gross','USD');


insert into sri_entities(entity_type) values ('account');
insert into sri_accounts(account_id,entity_id,name,currency) values (22,11,'Sales Comission','USD');


