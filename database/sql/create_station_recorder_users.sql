
/*
drop user  'station_recorder'@'64.13.145.233' ;
create user 'station_recorder'@'64.13.145.233' identified by 'somesecret';  
*/

use sr;
grant select,insert on sr.sr_stations to 'station_recorder'@'64.13.145.233' identified by 'somesecret';
grant select,insert on sr.sr_hash_codes to 'station_recorder'@'64.13.145.233' identified by 'somesecret';
grant select,insert on sr.sr_hash_code_links to 'station_recorder'@'64.13.145.233' identified by 'somesecret';
grant select,insert,update on sr.sr_radio_plays to 'station_recorder'@'64.13.145.233' identified by 'somesecret';
grant select,insert on sr.sr_station_web_programs to 'station_recorder'@'64.13.145.233' identified by 'somesecret';
grant select,insert,update on sr.sr_song_info to 'station_recorder'@'64.13.145.233' identified by 'somesecret';   
grant select,insert,update on sr.sr_album_info to 'station_recorder'@'64.13.145.233' identified by 'somesecret';   
grant select,insert on sr.sr_picture_info to 'station_recorder'@'64.13.145.233' identified by 'somesecret';

use sr;
grant select,insert on sr.sr_stations to 'station_recorder'@'64.13.145.235' identified by 'somesecret';
grant select,insert on sr.sr_hash_codes to 'station_recorder'@'64.13.145.235' identified by 'somesecret';
grant select,insert on sr.sr_hash_code_links to 'station_recorder'@'64.13.145.235' identified by 'somesecret';
grant select,insert,update on sr.sr_radio_plays to 'station_recorder'@'64.13.145.235' identified by 'somesecret';
grant select,insert on sr.sr_station_web_programs to 'station_recorder'@'64.13.145.235' identified by 'somesecret';
grant select,insert,update on sr.sr_song_info to 'station_recorder'@'64.13.145.235' identified by 'somesecret';   
grant select,insert,update on sr.sr_album_info to 'station_recorder'@'64.13.145.235' identified by 'somesecret';   
grant select,insert on sr.sr_picture_info to 'station_recorder'@'64.13.145.235' identified by 'somesecret';




use sr;
grant select,insert on sr.sr_stations to 'station_recorder'@'64.13.145.234' identified by 'somesecret';
grant select,insert on sr.sr_hash_codes to 'station_recorder'@'64.13.145.234' identified by 'somesecret';
grant select,insert on sr.sr_hash_code_links to 'station_recorder'@'64.13.145.234' identified by 'somesecret';
grant select,insert,update on sr.sr_radio_plays to 'station_recorder'@'64.13.145.234' identified by 'somesecret';
grant select,insert on sr.sr_station_web_programs to 'station_recorder'@'64.13.145.234' identified by 'somesecret';
grant select,insert,update on sr.sr_song_info to 'station_recorder'@'64.13.145.234' identified by 'somesecret';   
grant select,insert,update on sr.sr_album_info to 'station_recorder'@'64.13.145.234' identified by 'somesecret';   
grant select,insert on sr.sr_picture_info to 'station_recorder'@'64.13.145.234' identified by 'somesecret';



use sr;
grant select,insert on sr.sr_stations to 'station_recorder'@'64.13.145.236' identified by 'somesecret';
grant select,insert on sr.sr_hash_codes to 'station_recorder'@'64.13.145.236' identified by 'somesecret';
grant select,insert on sr.sr_hash_code_links to 'station_recorder'@'64.13.145.236' identified by 'somesecret';
grant select,insert,update on sr.sr_radio_plays to 'station_recorder'@'64.13.145.236' identified by 'somesecret';
grant select,insert on sr.sr_station_web_programs to 'station_recorder'@'64.13.145.236' identified by 'somesecret';
grant select,insert,update on sr.sr_song_info to 'station_recorder'@'64.13.145.236' identified by 'somesecret';   
grant select,insert,update on sr.sr_album_info to 'station_recorder'@'64.13.145.236' identified by 'somesecret';   
grant select,insert on sr.sr_picture_info to 'station_recorder'@'64.13.145.236' identified by 'somesecret';


use sr;
grant select,insert on sr.sr_stations to 'station_recorder'@'64.13.145.225' identified by 'somesecret';
grant select,insert on sr.sr_hash_codes to 'station_recorder'@'64.13.145.225' identified by 'somesecret';
grant select,insert on sr.sr_hash_code_links to 'station_recorder'@'64.13.145.225' identified by 'somesecret';
grant select,insert,update on sr.sr_radio_plays to 'station_recorder'@'64.13.145.225' identified by 'somesecret';
grant select,insert on sr.sr_station_web_programs to 'station_recorder'@'64.13.145.225' identified by 'somesecret';
grant select,insert,update on sr.sr_song_info to 'station_recorder'@'64.13.145.225' identified by 'somesecret';   
grant select,insert,update on sr.sr_album_info to 'station_recorder'@'64.13.145.225' identified by 'somesecret';   
grant select,insert on sr.sr_picture_info to 'station_recorder'@'64.13.145.225' identified by 'somesecret';


flush privileges;

