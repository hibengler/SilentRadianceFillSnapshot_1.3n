#!/bin/bash
# This is run on a machine that had access to the source files
export SR_BIN="${SR_BIN:-/usr/local/bin}"

echo "select transaction_code from sr_paypal_raw_transactions where status = 'VERIFIED' and build_status is null
  order by last" | mysql -u order_fulfillment -h dtb.silentradiance.com -s sr |
awk '{print "order_fulfillment_on_verified_transaction.sh " $0}' | bash




# here is some code to reset paypal orders
#delete from sri_raw_order_fulfillment_items;
#delete from sri_raw_paypal_order_items;
#delete from sri_raw_paypal_orders;
#update sr_paypal_raw_transactions set build_status=null where build_status is not null;
