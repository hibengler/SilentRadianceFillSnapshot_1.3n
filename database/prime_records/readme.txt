01 ... is ok - 02 an 03 or build the thing is not because we made 3 consumables for each station - and there should be 1



select * from sr_song_info_consumables sic,
 sr_consumables c
where sic.consumable_id = c.consumable_id
and sic.station_name like 'reggaeton%'
order by sic.filename,station_name;
 delete from sr_consumables where not exists (select 1 from sr_song_info_consumables sic where sic.consumable_id = sr_consumables.consumable_id) and action_id=2;
 
 
 
desc sr_song_info_consumables;
insert into sr_song_info_consumables(station_name,filename,consumable_id,display_sequence,
  for_sale_in_general)
select 'reggaeton2.silentradiance.com',filename,consumable_id,display_sequence,for_sale_in_general
from sr_song_info_consumables
where station_name = 'reggaeton.silentradiance.com';

insert into sr_song_info_consumables(station_name,filename,consumable_id,display_sequence,
  for_sale_in_general)
select 'reggaeton3.silentradiance.com',filename,consumable_id,display_sequence,for_sale_in_general
from sr_song_info_consumables
where station_name = 'reggaeton.silentradiance.com';



note: there are duplicate ean numbers in psytrance:

MariaDB [sr]> select ean,count(*) from sr_consumables group by ean having count(*) >1;
select * from sr_consumables where ean='pao2cd016,03';

Need to do some snobol analysis on that. - or perhaps the cd is different in sr_cds?

  
turns out the CD names didnt handle the cd1 or cd2 correctly - should be fixed - but had to rescrape
