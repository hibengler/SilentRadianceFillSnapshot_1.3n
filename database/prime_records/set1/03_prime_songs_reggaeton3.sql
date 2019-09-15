/* 2 Farruko Ft. Benny Benni "La Caza Bichotes".wav */
/* Prime_Records/pricd001-Various-Los_Gladiators/02 - Farruko feat. Benny Benni - La Caza Bichotes.wav */
insert into sr_artists(artist_code,artist_name) 
select 'Benny Benni','Benny Benni' where not exists (  select 1 from sr_artists where artist_code = 'Benny Benni'  
union all     select 1 from sr_artists where artist_name = 'Benny Benni');

insert into sr_artists(artist_code,artist_name) 
select 'Farruko','Farruko' where not exists (  select 1 from sr_artists where artist_code = 'Farruko'  
union all     select 1 from sr_artists where artist_name = 'Farruko');


insert into sr_cds(station_name,cd_short_name,business,item_name,image_url)
select 'reggaeton3.silentradiance.com','GLADIAT','Prime','GLADIAT'
,'set0/Los_Gladiators/front_cover.png'
where not exists (select 1 from sr_cds where
station_name='reggaeton3.silentradiance.com'
and cd_short_name='GLADIAT');


insert into sr_cd_tracks(station_name,cd_short_name,track,raw_track_text,title)
select 'reggaeton3.silentradiance.com','GLADIAT','2','La Caza Bichotes','La Caza Bichotes' 
where not exists(select 1 from sr_cd_tracks where station_name = 'reggaeton3.silentradiance.com'      
and cd_short_name = 'GLADIAT' and track = '2');

insert into sr_cd_track_artists(station_name,cd_short_name,track,sequence,type,artist_id)
select 'reggaeton3.silentradiance.com','GLADIAT','2',1,'Main',(select artist_id from sr_artists where artist_code= 'Farruko' ) 
where not exists(select 1 from sr_cd_track_artists where station_name = 'reggaeton3.silentradiance.com' 
  and cd_short_name = 'GLADIAT' and track = '2' and sequence = 1)
and not exists(select 1 from sr_cd_track_artists where station_name = 'reggaeton3.silentradiance.com'      and cd_short_name =
 'GLADIAT' and track = '2' and artist_id =(select artist_id from sr_artists where artist_code= 'Farruko'));
 

insert into sr_cd_track_artists(station_name,cd_short_name,track,sequence,type,artist_id)
select 'reggaeton3.silentradiance.com','GLADIAT','2',2,'Main',(select artist_id from sr_artists where artist_code= 'Benny Benni' ) 
where not exists(select 1 from sr_cd_track_artists where station_name = 'reggaeton3.silentradiance.com' 
  and cd_short_name = 'GLADIAT' and track = '2' and sequence = 2)
and not exists(select 1 from sr_cd_track_artists where station_name = 'reggaeton3.silentradiance.com'      and cd_short_name =
 'GLADIAT' and track = '2' and artist_id =(select artist_id from sr_artists where artist_code= 'Benny Benni'));
 

/* '1 Drakote & Chezina "XplozioÌn".wav'  */
/* Prime_Records/pricd001-Various-Los_Gladiators/01 - Drakote and Chezina - Xplozion.wav */


/* '1 Falo  Pal Cruse.wav' */
/* Prime_Records/pricd002-Falo-Ritmo_Professional_Explicit/01 - Falo - Pal Cruse.wav */


/* '1 La recta final.wav' */
/* Prime_Records/pricd003-DJ_Negro_and_Vico_C-La_Recta_Final/01 - DJ Negro and Vico C - La Recta Final.wav */

/* '2 Nana ya llego  La Nana.wav' */
/* Prime_Records/pricd004-Various-Prime_Underground_3_Explicit/02 - Nana Ya Llego - La Nana.wav */

/* '3 Drakote "Mal de amores".wav' */
/* Prime_Records/pricd001-Various-Los_Gladiators/03 - El Drakote - Mal de Amores Explicit.wav */

/* '3 Mundo artificial.wav' */
/* Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/03 - Vico C - Mundo Artificial.wav */

/* '4 Extra  Falo & Bam Bam.wav' */
/* Prime_Records/pricd004-Various-Prime_Underground_3_Explicit/04 - Falo and Bam Bam - Extra.wav */

/* '5 La inglesa.wav' */
/* Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/05 - Vico C - La Inglesa.wav */

/* '5 Medicina.wav' */
/* Prime_Records/pricd006-Kid_Power_Posse-La_Medicina_Explicit/05 - Kid Power Posse - Medicina.wav */

/* '5 Te ves buena.wav' */
/* Prime_Records/pricd007-El_General-Estas_Buena_Explicit/05 - El General - Te Ves Buena.wav */

/* '6 El Gordo.wav' */
/* Prime_Records/pricd006-Kid_Power_Posse-La_Medicina_Explicit/06 - Kid Power Posse - El Gordo.wav */

/* '6 Pu Tun Tun.wav' */
/*  Prime_Records/pricd007-El_General-Estas_Buena_Explicit/06 - El General - Pu Tun Tun.wav"


/* '6 Tu Pum Pum.wav' */
/* Prime_Records/pricd008-Lisa_M-No_Lo_Derrumbes/05 - Lisa M - Tu Pum Pum.wav */


/* '6 Xplosion.wav' */
/* Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/06 - Vico C - Xplosion.wav */


/* 7 Falo  Colecta.wav */
/* Prime_Records/pricd009-Falo-Colecta/07 - Falo - Colecta.wav */

/* '7 Saborealo.wav' */
/* Prime_records/pricd005-Vico_C-Greatest_Hits_Explicit/07 - Vico C - Saborealo.wav */

/* '8 Maria.wav' */
/* Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/08 - Vico C - Maria.wav */

/* '9 Baby J Me Dizzen.wav */
/* Prime_Records/pricd010-Various-Prime_Underground_1_Explicit/08 - Baby J - Me Dizzen.wav */

/* '9 Cut It Up Baron (Long Version).wav' */
/* Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/09 - Vico C - Cut it Up Baron (Long Version).wav */

/* 'Halloween.wav' */
/* Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/02 - Vico C - Viernes 13.wav */



update sr_cds set imagehead_url=null,image_url='Prime_Records/pricd001-Various-Los_Gladiators/front_cover.png'  
where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd001';
update sr_cds set  imagehead_url=null,
image_url='Prime_Records/pricd002-Various-Prime_Underground_1_Explicit/front_cover.png'  
where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd002';
update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd003-DJ_Negro_and_Vico_C-La_Recta_Final/front_cover.png'  
where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd003';

update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd004-Various-Prime_Underground_3_Explicit/front_cover.png'  
where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd004';
update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd004-Various-Prime_Underground_3_Explicit/front_cover.png'  
where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd004';

 update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd005-Vico_C-Greatest_Hits_Explicit/front_cover.png'  
 where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd005';
 update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd006-Kid_Power_Posse-La_Medicina_Explicit/front_cover.png'  
 where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd006';
 
 update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd007-El_General-Estas_Buena_Explicit/front_cover.png'  
 where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd007';
 update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd008-Lisa_M-No_Lo_Derrumbes/front_cover.png'  
 where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd008';
update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd009-Falo-Colecta/front_cover.png'  
where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd009';
update sr_cds set  imagehead_url=null,image_url='Prime_Records/pricd010-Various-Prime_Underground_1_Explicit/front_cover.png'  
where station_name='reggaeton3.silentradiance.com' and cd_short_name='pricd010';

 
