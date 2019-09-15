/* drop view sri_delta_v_last_month */
create view sri_delta_v_last_month as
select 'delete' type,b.payment_date txn_date,b.jn_datetime journal_time,b.txn_id,
   b.payer_id,b.receiver_id,b.first_name,b.mc_currency,
   -b.mc_gross mc_gross,
   -b.payment_gross payment_gross,
   -b.mc_fee mc_fee,
   -b.quantity quantity
   ,b.item_number,b.item_name
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
select 'undo' type,b.payment_date txn_date,b.jn_datetime journal_time,b.txn_id,
   b.payer_id,b.receiver_id,b.first_name,b.mc_currency,
   -b.mc_gross mc_gross,
   -b.payment_gross payment_gross,
   -b.mc_fee mc_fee,
   -b.quantity quantity
   ,b.item_number,b.item_name
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
union
select 'new' type,b.payment_date txn_date,b.jn_datetime journal_time,b.txn_id,
   b.payer_id,b.receiver_id,b.first_name,b.mc_currency,
   b.mc_gross mc_gross,
   b.payment_gross payment_gross,
   b.mc_fee mc_fee,
   b.quantity quantity
   ,b.item_number,b.item_name
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
select 'redo' type,b.payment_date txn_date,b.jn_datetime journal_time,b.txn_id,
   b.payer_id,b.receiver_id,b.first_name,b.mc_currency,
   b.mc_gross mc_gross,
   b.payment_gross payment_gross,
   b.mc_fee mc_fee,
   b.quantity quantity
   ,b.item_number,b.item_name
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


select aa.affiliation_code,c.action_id,
t.type,t.txn_date,t.journal_time,t.txn_id,t.payer_id,t.receiver_id,t.first_name,
t.mc_currency,t.mc_gross,t.payment_gross,t.mc_fee,
t.payment_gross-t.mc_fee net_from_paypal,
t.payment_gross-t.mc_fee -
ifnull(aa.default_kickback,t.payment_gross-mc_fee-floor(100*t.payment_gross*aa.default_kickback_percentage)/100)
hib_fees,
ifnull(aa.default_kickback,t.payment_gross-mc_fee-floor(100*t.payment_gross*aa.default_kickback_percentage)/100)
proceeds
from sri_delta_v_last_month t 
left outer join sr_consumables c
on c.consumable_id = t.item_number
left outer join sr_affiliation_actions aa
on aa.affiliation_code = c.affiliation_code
and aa.action_id = c.action_id
order by affiliation_code,t.txn_date,t.journal_time;



hib_charge,
