/* simple but usable entity id for accounting */



create table sri_entity_types (
entity_type varchar(20) not null,
entity_type_table varchar(60),
entity_type_view varchar(60),
description varchar(2000),
constraint entity_type_pk  primary key(entity_type)
);

create table sri_entities (
entity_id bigint not null auto_increment,
entity_type varchar(20) not null,
constraint sri_entities_pk primary key(entity_id),
constraint sri_entities_fk1 foreign key(entity_type) references sri_entity_types(entity_type)
);







create table sri_account_names (
account_name varchar(200) not null,
constraint sri_account_names primary key(account_name)
);

create table sri_paypal_accounts (
paypal_id varchar(20) not null,
constraint sri_paypal_accounts_pk primary key(paypal_id)
);

create table sri_email_addresses (
email_address varchar(200) not null,
formatted_email_address varchar(200),
paypal_id varchar(20) ,
constraint sri_email_addresses_pk primary key(email_address),
constraint sri_email_addresses_uk1 unique (formatted_email_address),
constraint sri_email_addresses_fk1 foreign key (paypal_id) references sri_paypal_accounts(paypal_id)
);



create table sri_taxable_entities (
entity_id bigint not null auto_increment,
us_taxpayer_id varchar(9),
paypal_email_address varchar(200) not null,
affiliation_code varchar(30), /*references sr_consumables affiliation_code */
constraint sri_taxable_pk primary key(entity_id),
constraint sri_taxable_uk unique(us_taxpayer_id),
constraint sri_taxable_fk1 foreign key(entity_id) references sri_entities(entity_id),
constraint sri_taxable_fk2 foreign key(paypal_email_address) references sri_email_addresses(email_address),
constraint sri_taxable_uk2 unique(affiliation_code)
);


create table sri_businesses (
business_id bigint not null,
parent_business_id bigint,
business_name varchar(200) not null,
constraint sri_businesses_pk primary key(business_id),
constraint sri_business_uk1 unique(parent_business_id,business_name),
constraint sri_business_uk2 unique(business_name,parent_business_id),
constraint sri_businesses_fk1 foreign key(business_id) references sri_taxable_entities(entity_id)
);


create table sri_people (
person_id bigint not null,
first_name varchar(40),
last_name varchar(40),
email_address varchar(200),
constraint sri_people_pk primary key(person_id),
constraint sri_people_uk1 unique(email_address),
constraint sri_people_fk1 foreign key(person_id) references sri_taxable_entities(entity_id),
constraint sri_people_fk2 foreign key(email_address) references sri_email_addresses(email_address)
);



create table sri_currencies (
currency varchar(20) not null,
constraint sri_currencies_pk primary key (currency)
);

create table sri_currency_conversion_rates (
from_currency varchar(20) not null,
to_currency varchar(20) not null,
rate double not null,
constraint sri_currencies_conversion_rates_pk primary key (from_currency,to_currency),
constraint sri_currencies_conversion_rates_fk1 foreign key (from_currency)
  references sri_currencies(currency),
constraint sri_currencies_conversion_rates_fk2 foreign key (to_currency) 
  references sri_currencies(currency)
);


create table sri_accounts (
account_id bigint not null,
entity_id bigint not null,
name varchar(100) not null,
currency varchar(20) not null,
constraint sri_accounts_pk primary key (account_id),
constraint sri_accounts_uk1 unique (entity_id,name),
constraint sri_accounts_fk1 foreign key (account_id) references sri_entities(entity_id),
constraint sri_accounts_fk2 foreign key (entity_id) references sri_taxable_entities(entity_id),
constraint sri_accounts_fk3 foreign key (currency) references sri_currencies(currency)
);



create table sri_transactions (
transaction_id bigint auto_increment,
parent_transaction_id bigint,
transaction_sequence int not null,
entity_id bigint not null,
from_currency varchar(20) not null,
to_currency varchar(20) not null,
amount_from decimal(16,6),
amount_to decimal(16,6),
conversion_rate_used double not null, 
paypal_txn_id varchar(40) not null,
paypal_item_sequence int,
account_name varchar(200),
is_balanced int not null,
ref_transaction_id bigint,
notes varchar(200) not null,
constraint sri_transactions_pk primary key(transaction_id),
constraint sri_transactions_uk1 unique(parent_transaction_id,transaction_sequence),
constraint sri_transactions_fk1 foreign key (entity_id) references sri_taxable_entities(entity_id),
constraint sri_transactions_fk2 foreign key (from_currency,to_currency) references 
  sri_currency_conversion_rates(from_currency,to_currency),
constraint sri_transactions_fk3 foreign key (paypal_txn_id) references sri_raw_paypal_orders(txn_id),
constraint sri_transactions_fk4 foreign key (paypal_txn_id,paypal_item_sequence) 
   references sri_raw_paypal_order_items(txn_id,item_sequence)
);

alter table sri_transactions  constraint sri_transactions_fk5 foreign key (ref_transaction_id)
  references sri_transactions(transaction_id);
alter table sri_transactions add constraint sri_transactions_fk6 foreign key (parent_transaction_id)
  references sri_transactions(transaction_id);

  
      

/*
insert account_names

insert paypal accounts
insert email addresses
  insert entity
  insert taxable_entity
  insert people
    insert  accounts purchase
      insert transactions
        insert entity prtax
	   insert subtransaction - 11 cents
	   insert subsubtransaction - pr tax
	insert accounts killercool revenu
           insert subtransaction 9 cents
	      insert subsubtransaction - killercool revenue
	insert accounts entity prime records -> proceeds
	   insert subtransaction 81 cents
               insert subsubtransaction proceeds 81 cents	   
	      
*/
