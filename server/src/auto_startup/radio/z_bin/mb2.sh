#!/bin/bash
#
# $Revision: 1.3 $
#
. ~/.silentradiance_config
cd /home/z/src
nohup xload &



# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function ctrl_c() {
        echo "** Trapped CTRL-C"
	stty sane
	echo "type "
	echo " start_broadcast $SR_DOMAIN_SERVER"
	echo "to restart"
bash
}


start_broadcast $SR_DOMAIN_SERVER || bash
#
# $Log: mb2.sh,v $
# Revision 1.3  2018/12/12 13:43:23  hib
# wip - cleaning up radio auto play
#
# Revision 1.1  2018/12/12 11:19:16  hib
# wip
#
