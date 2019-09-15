select * from sr_consumables 
where consumable_id in (select consumable_id from sr_song_info_consumables where station_name='reggaeton.silentradiance.com')
and action_id=2;

select station_name,consumable_id from sr_songs_to_consumables_v
where station_name like 'reggaet%';



update sr_consumables 
set amount=0.97
where consumable_id in (select consumable_id from sr_song_info_consumables where station_name='reggaeton.silentradiance.com')
and action_id=2
;
