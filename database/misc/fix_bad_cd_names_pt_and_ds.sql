the cd names are not taking into account the CD 1 and CD2 - fixed the parser
update sr_song_info set cd_track=null,cd_short_name = null 
where station_name in ('pt.silentradiance.com','ds.silentradiance.com');
delete from sr_cd_tracks where station_name in ('pt.silentradiance.com','ds.silentradiance.com');
delete from sr_cd_track_artists where station_name in ('pt.silentradiance.com','ds.silentradiance.com');

build_cds_from_songs pt.silentradiance.com
build_cds_from_songs ds.silentradiance.com
  



# well - have to rescrape things. which takes a while - but that will fix cd_tracks and artists
