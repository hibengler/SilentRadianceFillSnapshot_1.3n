#/bin/bash
# $Revision: 1.2 $
export SR_BIN="${SR_BIN:-/usr/local/bin}"


export PATH=/usr/bin:/usr/sbin:/usr/local/bin:/bin:/sbin

cd /var/log/apache2
cp /var/log/sr/reports/aquerdo.tsv /var/log/sr/reports/aquerdo.tsv_b4
cat `ls ssl_reggaeton_access.log* | grep -v 'gz$'`   | 
   sbl $SR_BIN/gather_acuerdo_stats.sbl  2>/var/log/sr/reports/aquerdo.err >>/var/log/sr/reports/aquerdo.tsv_b4 
   sort -u </var/log/sr/reports/aquerdo.tsv_b4 >/var/log/sr/reports/aquerdo.tsv   
