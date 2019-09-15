select filename,rp.start_unix_time,rp.end_unix_time,count(distinct session_code) number_sessions
 from 
sr_radio_plays rp
inner join sr_station_web_programs swp 
on rp.station_name = swp.station_name
left outer join sr_sessions s
on s.dns_name = swp.dns_name
and s.program_name = swp.program_name
and (
   (s.start_unix_time <= rp.start_unix_time and (s.end_unix_time >= rp.start_unix_time) )
or (s.start_unix_time <= rp.end_unix_time and (s.end_unix_time >= rp.end_unix_time) )
or (s.start_unix_time >= rp.start_unix_time and (s.end_unix_time <= rp.end_unix_time) )
or (s.start_unix_time <= rp.start_unix_time and (s.end_unix_time >= rp.end_unix_time) )
  )
where rp.station_name = 'ds.silentradiance.com' and rp.end_unix_time > unix_timestamp() -10000
group by filename,rp.start_unix_time,rp.end_unix_time order by 2;




select user_code,session_code,start_unix_time,end_unix_time from
sr_sessions s
where s.dns_name='ds.silentradiance.com'
and s.end_unix_time >  unix_timestamp() -10000 
;
