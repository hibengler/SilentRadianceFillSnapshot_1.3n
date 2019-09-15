      
. ~/.silentradiance_config
cd $SR_PLAYLIST
bash start_follow $SR_DOMAIN_SERVER &


# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function ctrl_c() {
        echo "** Trapped CTRL-C"
	stty sane
	echo "type:"
#	echo "start_follow $SR_DOMAIN_SERVER &"
#	echo " and "
	echo "playnow.sh"
	echo "to restart"
	
bash
}

bash playnow.sh 


