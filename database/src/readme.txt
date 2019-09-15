For all time:
export MYSQL_PWD=secret_for_staqtion_multicaster
redo_all_sr_error_log
        get_all_sr_error_log gip >midfile
	    filter_sr_error_log.sbl
	zips it into midfule
	Then 
	  convert_sr_error_log_into_sql.sbl
          through fieldu -1
	  through mysql
	  

	  
		  
for now:

export MYSQL_PWD=secret_for_staqtion_multicaster
follow_all_sr_error_log
    follow_one_sr_error_log <filename> <command>
       filter_sr_error_log.sbl |
         convert_sr_error_log_into_sql.sbl |  mysql
			  	  

convert_sr_error_log_into_sql.sbl - reads the error log files and creates insert and update statements to record them.
filter_sr_error_log.sbl
follow_one_sr_error_log <filename> <command>
get_all_sr_error_log
follow_all_sr_error_log
redo_all_er_error_log
install.txt - installation script
readme.txt - this script
