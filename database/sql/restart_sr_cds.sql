delete from sr_cd_track_artists;
delete from sr_cd_artists;
delete from sr_emails;
delete from sr_artists;
delete from sr_cd_tracks;
update sr_song_info set folder=null,cd_short_name=null;
delete from sr_folders;
delete from sr_cds;

