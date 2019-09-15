

system:
	touch system

# build_All_songs and build_all_songs.sbl is out

install: convert_sr_error_log_into_sql.sbl  filter_sr_error_log.sbl follow_one_sr_error_log get_all_sr_error_log redo_all_sr_error_log follow_all_sr_error_log
	sudo chmod 755 convert_sr_error_log_into_sql.sbl  filter_sr_error_log.sbl follow_one_sr_error_log get_all_sr_error_log redo_all_sr_error_log follow_all_sr_error_log /usr/local/bin
	sudo cp convert_sr_error_log_into_sql.sbl   filter_sr_error_log.sbl follow_one_sr_error_log get_all_sr_error_log redo_all_sr_error_log follow_all_sr_error_log /usr/local/bin
