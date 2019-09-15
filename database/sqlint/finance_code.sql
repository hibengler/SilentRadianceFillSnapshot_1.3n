/* $Revision: 1.1 $
Basically this loads up transactions
*/


declare cundo cursor for
select 'd' type,b.txn_id,business,first_name,item_number,mc_currency,mc_fee,mc_gross,payment_date,payment_gross,
quantity,shipping,shipping_discount,payer_email,payer_id 
from sri_raw_paypal_orders_jn b
where txn_id in (select txn_id
  from sri_raw_paypal_orders_jn a
  where a.jn_datetime > str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
  and jn_datetime <= str_to_date('2019-07-01 02:33:55','%Y-%m-%d %H:%i:%s')
  )
and b.jn_datetime <= str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
and b.jn_endtime > str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
and b.jn_operation != 'd'  
and ((not exists (
  select 1
  from sri_raw_paypal_orders_jn c
  where c.txn_id = b.txn_id
  and c.jn_datetime <= str_to_date('2019-07-01 00:00:00','%Y-%m-%d %H:%i:%s')
  and c.jn_endtime > str_to_date('2019-07-01 00:00:00','%Y-%m-%d %H:%i:%s')
  and c.jn_operation != 'd'  
  )))
union all  
select 'undo',b.txn_id,b.business,
b.first_name,b.item_number,b.mc_currency,
b.mc_fee,b.mc_gross,b.payment_date,b.payment_gross,
b.quantity,b.shipping,b.shipping_discount,b.payer_email,b.payer_id 
from sri_raw_paypal_orders_jn b
join sri_raw_paypal_orders_jn d
on d.txn_id = b.txn_id
and d.jn_datetime <= str_to_date('2019-07-01 00:00:00','%Y-%m-%d %H:%i:%s')
and d.jn_endtime > str_to_date('2019-07-01 00:00:00','%Y-%m-%d %H:%i:%s')
and d.jn_operation != 'd'
and ifnull(d.mc_gross,0) != ifnull(b.mc_gross,0) /* add more */	    
where b.txn_id in (select txn_id
  from sri_raw_paypal_orders_jn a
  where a.jn_datetime > str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
  and jn_datetime <= str_to_date('2019-07-01 02:33:55','%Y-%m-%d %H:%i:%s')
  )
and b.jn_datetime <= str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
and b.jn_endtime > str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
and b.jn_operation != 'd'  
;



/* ^^^ undo */



declare cadd cursor for 
select 'i' type,b.txn_id,business,first_name,item_number,mc_currency,mc_fee,mc_gross,payment_date,payment_gross,
quantity,shipping,shipping_discount,payer_email,payer_id 
from sri_raw_paypal_orders_jn b
where txn_id in (select txn_id
  from sri_raw_paypal_orders_jn a
  where a.jn_datetime > str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
  and jn_datetime <= str_to_date('2019-07-01 02:33:55','%Y-%m-%d %H:%i:%s')
  )
and b.jn_datetime <= str_to_date('2019-07-01 00:00:00','%Y-%m-%d %H:%i:%s')
and b.jn_endtime > str_to_date('2019-07-01 00:00:00','%Y-%m-%d %H:%i:%s')
and b.jn_operation != 'd'  
and ((not exists (
  select 1
  from sri_raw_paypal_orders_jn c
  where c.txn_id = b.txn_id
  and c.jn_datetime <= str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
  and c.jn_endtime > str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
  and c.jn_operation != 'd'  
  )))
union all  
select 'redo',b.txn_id,b.business,
b.first_name,b.item_number,b.mc_currency,
b.mc_fee,b.mc_gross,b.payment_date,b.payment_gross,
b.quantity,b.shipping,b.shipping_discount,b.payer_email,b.payer_id 
from sri_raw_paypal_orders_jn b
join sri_raw_paypal_orders_jn d
on d.txn_id = b.txn_id
and d.jn_datetime >= str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
and d.jn_endtime < str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
and d.jn_operation != 'd'
and ifnull(d.mc_gross,0) != ifnull(b.mc_gross,0) /* add more */	    
where b.txn_id in (select txn_id
  from sri_raw_paypal_orders_jn a
  where a.jn_datetime >= str_to_date('2019-06-01 00:00:00','%Y-%m-%d %H:%i:%s')
  and jn_datetime < str_to_date('2019-07-01 02:33:55','%Y-%m-%d %H:%i:%s')
  )
and b.jn_datetime >= str_to_date('2019-07-01 00:00:00','%Y-%m-%d %H:%i:%s')
and b.jn_endtime < str_to_date('2019-07-01 00:00:00','%Y-%m-%d %H:%i:%s')
and b.jn_operation != 'd'  
;

declare done boolean default 0;
declare continue handler for sqlstate '02000' set done=1;

declare xtype varchar(10);
declare xtxn_id varchar(40);
declare xbusiness varchar(40);
declare xfirst_name varchar(200);
declare xlast_name varchar(200);
declare xitem_number varchar(200);
declare xitem_name varchar(200);
declare xinsurance_amount decimal(10,2);
declare xmc_currency varchar(40);
declare xmc_fee decimal(10,2);
declare xmc_gross decimal(10,2);
declare payment_date datetime;
declare payment_gross decimal(10,2);
declare quantity decimal(10,2);
declare xshipping decimal(10,2);
declare xshipping_discount decimal(10,2);
declare xpayer_email varchar(255),
declare xpayer_id varchar(255);
declare xtax decimal(10,2);
declare xpayer_entity_id bigint;
declare xaccount_paypal bigint;

set xtext = '';
open c1;
fetch c1 into xtype,xtxn_id,xbusiness,xfirst_name,xlast_name,xitem_number,xitem_name,xinsurance_amount,
  xmc_currency,xmc_fee,xmc_gross,xpayment_date,xpayment_gross,zquantity,xshipping,xpayer_email,xpayer_id;

while ! done do
  set xtax = xgross = xmc_gross;
  
  get_or_make_entity_id_of_customer(xpayer_email,xpayer_id,xfirst_name,xlast_name,xpayer_entity_id);

  set done = 0;

