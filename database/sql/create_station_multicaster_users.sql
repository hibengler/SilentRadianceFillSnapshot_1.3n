

use sr;
grant select,insert on sr.sr_names to 'station_multicaster'@'64.13.145.231' identified by 'somesecret';
grant select,insert on sr.sr_ip_addresses to 'station_multicaster'@'64.13.145.231' identified by 'somesecret';
grant select,insert on sr.sr_users to 'station_multicaster'@'64.13.145.231' identified by 'somesecret';
grant select,insert on sr.sr_user_ip_addresses to 'station_multicaster'@'64.13.145.231' identified by 'somesecret';
grant select,insert on sr.sr_web_programs to 'station_multicaster'@'64.13.145.231' identified by 'somesecret';
grant select,insert on sr.sr_user_web_programs to 'station_multicaster'@'64.13.145.231' identified by 'somesecret';
grant select,insert,update on sr.sr_sessions to 'station_multicaster'@'64.13.145.231' identified by 'somesecret';   



flush privileges;

