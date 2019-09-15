/*
drop view sr_songs_to_consumables_v;
*/

create view sr_songs_to_consumables_v as 
select 
'i' mode,
/* this converts song_info and cds into a consumable */
si.station_name,
si.filename,
si.cd_short_name,
si.sha256,
null consumable_id,
0 can_update_keys,
 concat(
  concat('',case cd.cd_short_name when null then 'no cd ' else '' end),
   case l2a.affiliation_code when l2a.affiliation_code then 
     case aac.affiliation_code when aac.affiliation_code then '' else 
       concat('No affiliation_action for action 2 affiliation ',l2a.affiliation_code) end     
   else concat('No affiliation code for song_info marketing_label ' ,ifnull(si.marketing_label,'NULL')) end)
 the_error,
'product' consumable_type,
          concat('Song: ',ifnull(si.title,
              concat(si.featured_artist,
           case si.sound_recording_title when  si.sound_recording_title then
        concat(' - ',si.sound_recording_title) else ''
       end,
       case si.album_title when si.album_title then
         concat(' - ',si.album_title,
           case si.track when si.track then concat(' ',
             case substr(si.track,1,1) when '0' then substr(si.track,2,length(si.track)-1)
                                                else si.track end )
            else '' end
             )
          else '' end,
        case si.marketing_label when si.marketing_label then
           concat(' - ',si.marketing_label) else '' end
       )) ) consumable_name,
       si.isrc isrc_code,
       si.ean,
       null bn,
       'Killer Cool East' business,
          concat('Song: ',ifnull(si.title,
              concat(si.featured_artist,
           case si.sound_recording_title when  si.sound_recording_title then
        concat(' - ',si.sound_recording_title) else ''
       end,
       case si.album_title when si.album_title then
         concat(' - ',si.album_title,
           case si.track when si.track then concat(' ',
             case substr(si.track,1,1) when '0' then substr(si.track,2,length(si.track)-1)
                                                else si.track end )
            else '' end
             )
          else '' end,
        case si.marketing_label when si.marketing_label then
           concat(' - ',si.marketing_label) else '' end
       )) ) item_name,
       case si.cd_short_name when si.cd_short_name then concat(concat(si.cd_short_name,'-'),
         si.track)
          else substr(sha256,1,12) end item_number,
          ifnull(aac.default_price,0.97) amount,
          'USD' currency_code,
          null shipping,
          null shipping2,
          si.demo_video_url,
          ifnull(si.picture,cd.image_url) image_url,
          cd.imagehead_url,
          1 undefined_quantity,
          'sr@killercool.net' receiver_email,
          null mrb,   
          null pal,
          1 no_shipping,
          1 no_note,
          concat('Song: ',ifnull(si.title,
              concat(si.featured_artist,
           case si.sound_recording_title when  si.sound_recording_title then
        concat(' - ',si.sound_recording_title) else ''
       end,
       case si.album_title when si.album_title then
         concat(' - ',si.album_title,
           case si.track when si.track then concat(' ',
             case substr(si.track,1,1) when '0' then substr(si.track,2,length(si.track)-1)
                                                else si.track end )
            else '' end
             )
          else '' end,
        case si.marketing_label when si.marketing_label then
           concat(' - ',si.marketing_label) else '' end
       )) ) title,
          si.featured_artist artist,
          si.marketing_label label,
          si.cd_short_name catalogue,
          'Digital Download' format,
          null barcode,
          cd.the_release,
	   aac.action_id,
	   aac.affiliation_code
      from sr_song_info si left outer join sr_cds cd
        on si.station_name = cd.station_name
        and si.cd_short_name = cd.cd_short_name
	left outer join sr_label_to_affiliations l2a
	on l2a.label = si.marketing_label
	left outer join sr_affiliation_actions aac
	on aac.affiliation_code = l2a.affiliation_code
	and aac.action_id = 2
        where  not exists (select 1 from sr_song_info_consumables sic,sr_consumables sc
              where sic.station_name = si.station_name
                and sic.filename = si.filename
                and sic.consumable_id = sc.consumable_id
		and sc.affiliation_code = l2a.affiliation_code
		and sc.action_id = 2)
union all      
select 
'u' mode,
/* this converts song_info and cds into a consumable */
si.station_name,
si.filename,
si.cd_short_name,
si.sha256,
sic.consumable_id,
0 can_update_keys,
 concat(
  concat('',case cd.cd_short_name when null then 'no cd ' else '' end),
   case l2a.affiliation_code when l2a.affiliation_code then 
     case aac.affiliation_code when aac.affiliation_code then '' else 
       concat('No affiliation_action for action 2 affiliation ',l2a.affiliation_code) end     
   else concat('No affiliation code for song_info marketing_label ' ,ifnull(si.marketing_label,'NULL')) end)
 the_error,
'product' consumable_type,
          concat('Song: ',ifnull(si.title,
              concat(si.featured_artist,
           case si.sound_recording_title when  si.sound_recording_title then
        concat(' - ',si.sound_recording_title) else ''
       end,
       case si.album_title when si.album_title then
         concat(' - ',si.album_title,
           case si.track when si.track then concat(' ',
             case substr(si.track,1,1) when '0' then substr(si.track,2,length(si.track)-1)
                                                else si.track end )
            else '' end
             )
          else '' end,
        case si.marketing_label when si.marketing_label then
           concat(' - ',si.marketing_label) else '' end
       )) ) consumable_name,
       si.isrc isrc_code,
       si.ean,
       null bn,
       'Killer Cool East' business,
          concat('Song: ',ifnull(si.title,
              concat(si.featured_artist,
           case si.sound_recording_title when  si.sound_recording_title then
        concat(' - ',si.sound_recording_title) else ''
       end,
       case si.album_title when si.album_title then
         concat(' - ',si.album_title,
           case si.track when si.track then concat(' ',
             case substr(si.track,1,1) when '0' then substr(si.track,2,length(si.track)-1)
                                                else si.track end )
            else '' end
             )
          else '' end,
        case si.marketing_label when si.marketing_label then
           concat(' - ',si.marketing_label) else '' end
       )) ) item_name,
       case si.cd_short_name when si.cd_short_name then concat(concat(si.cd_short_name,'-'),
         si.track)
          else substr(sha256,1,12) end item_number,
          ifnull(aac.default_price,0.97) amount,
          'USD' currency_code,
          null shipping,
          null shipping2,
          si.demo_video_url,
          ifnull(si.picture,cd.image_url) image_url,
          cd.imagehead_url,
          1 undefined_quantity,
          'sr@killercool.net' receiver_email,
          null mrb,   
          null pal,
          1 no_shipping,
          1 no_note,
          concat('Song: ',ifnull(si.title,
              concat(si.featured_artist,
           case si.sound_recording_title when  si.sound_recording_title then
        concat(' - ',si.sound_recording_title) else ''
       end,
       case si.album_title when si.album_title then
         concat(' - ',si.album_title,
           case si.track when si.track then concat(' ',
             case substr(si.track,1,1) when '0' then substr(si.track,2,length(si.track)-1)
                                                else si.track end )
            else '' end
             )
          else '' end,
        case si.marketing_label when si.marketing_label then
           concat(' - ',si.marketing_label) else '' end
       )) ) title,
          si.featured_artist artist,
          si.marketing_label label,
          si.cd_short_name catalogue,
          'Digital Download' format,
          null barcode,
          cd.the_release,
	   aac.action_id,
	   aac.affiliation_code
      from sr_song_info si left outer join sr_cds cd
        on si.station_name = cd.station_name
        and si.cd_short_name = cd.cd_short_name
	left outer join sr_label_to_affiliations l2a
	on l2a.label = si.marketing_label
	left outer join sr_affiliation_actions aac
	on aac.affiliation_code = l2a.affiliation_code
	and aac.action_id = 2	
        inner join sr_song_info_consumables sic
              on sic.station_name = si.station_name
                and sic.filename = si.filename
	inner join sr_consumables sc
		on  sic.consumable_id = sc.consumable_id
                and sc.affiliation_code = l2a.affiliation_code
		and  sc.action_id = 2;

