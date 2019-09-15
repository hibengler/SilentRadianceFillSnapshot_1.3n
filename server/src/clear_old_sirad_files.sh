#!/bin/bash
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
ls -d /var/www/*silentradiance.com/radio |
  awk '{print "find " $0 " -name '\''output0*.sirad'\'' -cmin +10 -exec rm -rf {} '\'';'\''"}' |
    bash
exit 0
