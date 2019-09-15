
/*

drop table sr_affiliation_actions;
drop table sr_label_to_affiliations;
drop table sr_cd_artists;
drop table sr_cd_track_artists;
drop table sr_cd_tracks;
drop table sr_emails;
drop table sr_artists;
drop table sr_cds;

 drop table sr_song_info_consumables;
  drop table sr_radio_plays;
  drop table sr_song_info;
  drop table sr_album_info;
   drop table sr_consumable_bill_of_materials;
    drop table sr_actions;
drop table sr_consumables;
drop table sr_consumable_types;
drop table sr_international_articles;
drop table sr_international_standard_recording_codes;
drop table isrc_registrant_years;
drop table isrc_registrant_codes;
drop table isrc_entities;
drop table isrc_years;
drop table isrc_country_codes;
drop table sr_picture_info;
drop table sr_hash_code_links;
drop table sr_hash_codes;
drop table sr_station_web_programs;
drop table sr_stations;


*/



create table sr_stations (
station_name varchar(100) not null,
description varchar(2000),
constraint sr_station_pk primary key (station_name));

create table sr_station_web_programs (
station_name varchar(100) not null,
dns_name varchar(200) not null,
program_name varchar(200) not null,
description varchar(2000),
constraint sr_station_web_programs_pk primary key (station_name,dns_name,program_name),
constraint sr_station_web_programs_uk1 unique (dns_name,program_name,station_name),
constraint sr_station_web_programs_fk1 foreign key (station_name) references sr_stations(station_name),
constraint sr_station_web_programs_fk2 foreign key (dns_name,program_name) references sr_web_programs(dns_name,program_name)
);




create table sr_hash_codes (
sha256 varchar (64),
data blob,
description varchar(2000),
notes varchar(2000),
constraint hash_codes_pk primary key(sha256)
);

create table sr_hash_code_links (
 sha256 varchar(64),
 table_name varchar(100),
 column_name varchar(100),
constraint sr_hash_code_links_pk primary key (sha256,table_name,column_name),
constraint sr_hash_code_links_uk1 unique (table_name,column_name,sha256)
);

create table sr_picture_info (
station_name varchar(100) not null,
picture varchar(200),
picture_ipc varchar(200),
picture_width int,
 picture_height int,
constraint sr_picture_info_pk primary key (station_name,picture)
);








/*http://www.ifpi.org/content/library/isrc_handbook.pdf */


create table isrc_country_codes (
country_code varchar(2) not null,
country_name varchar(100),
constraint isrc_country_code_pk primary key (country_code),
constraint isrc_country_code_uk1 unique (country_name))
;


create table isrc_years (
year_code varchar(2) not null,
constraint isrc_years_pk primary key (year_code)
);



create table isrc_entities (
entity_id bigint not null auto_increment,
entity_name varchar(150) not null,
address varchar(400)  null,
other_details varchar(2000)  null,
department varchar(100),
person_name varchar(200),
constraint isrc_entities_pk primary key (entity_id),
constraint isrc_entities_uk1 unique (entity_name)
);



create table isrc_registrant_codes (
country_code varchar(2) not null,
registrant_code varchar(3) not null,
registrant_id bigint,
date_registrant_code_issused date,
constraint isrc_registrant_codes_pk primary key (country_code,registrant_code),
constraint isrc_registrant_codes_fk1 foreign key (country_code) references isrc_country_codes(country_code),
constraint isrc_registrant_codes_fk2 foreign key (registrant_id) references isrc_entities(entity_id)
);



create table isrc_registrant_years (
country_code varchar(2) not null,
registrant_code varchar(3) not null,
registrant_year varchar(2) not null references isrc_years(year_code),
constraint isrc_registrant_years_pk primary 
key (country_code,registrant_code,registrant_year),
constraint isrc_registrant_years_fk1 foreign key (country_code,registrant_code) references
 isrc_registrant_codes(country_code,registrant_code),
constraint isrc_registrant_years_fk2 foreign key (country_code)
  references isrc_country_codes(country_code),
constraint isrc_registrant_years_fk3 foreign key (registrant_year)
  references isrc_years(year_code)
 );
 
 
 
create table sr_international_standard_recording_codes (
isrc_code varchar(12) not null,
country_code varchar(2) not null,
registrant_code varchar(3) not null,
year_of_reference varchar(2) not null references isrc_years(year_code),
designation_code varchar(5) not null,
audio_or_video varchar(5),
constraint sr_isrcs_pk primary key(isrc_code),
constraint sr_isrcs_uk1 unique (country_code,registrant_code,year_of_reference,designation_code),
constraint sr_isrcs_fk1 foreign key (country_code,registrant_code,year_of_reference) 
  references isrc_registrant_years(country_code,registrant_code,registrant_year)
);
    
    

create table sr_international_articles (
ean varchar(16),
constraint sr_international_articles_pk primary key(ean)
);





create table sr_consumable_types (
consumable_type varchar(20),
description varchar(2000),
constraint sr_consumable_types_pk primary key (consumable_type)
);




create table sr_affiliations (
affiliation_code varchar(30) not null,
constraint sr_affiliations_pk primary key (affiliation_code)
);

insert into sr_affiliations(affiliation_code) values ('apple');
insert into sr_affiliations(affiliation_code) values ('Geomagnetic');
insert into sr_affiliations(affiliation_code) values ('Prime');
insert into sr_affiliations(affiliation_code) values ('Killercool');


create table sr_actions (
action_id int not null,
action_name varchar(100) not null,
description varchar(2000) not null,
constraint sr_actions_pk primary key(action_id),
constraint sr_actions_uk1 unique (action_name)
);


insert into sr_actions values (1,'Apple Reference','Reference into Apple');
insert into sr_actions values (2,'Sell song','Sell single song');
insert into sr_actions values (3,'Sell Album','Sell Digital Album');
insert into sr_actions values (4,'Shoutout','Sell Shoutout');

  
create table sr_affiliation_actions(
affiliation_code varchar(30) not null,
action_id int not null,
default_price decimal(20,6),
default_kickback      decimal(20,6),
default_kickback_percentage      decimal(20,6),
constraint sr_affiliation_actions_pk primary key(affiliation_code,action_id),
constraint sr_affiliation_actions_fk1 foreign key (affiliation_code) 
    references sr_affiliations(affiliation_code),
constraint sr_affiliation_actions_fk2 foreign key (action_id) references sr_actions(action_id)
);

insert into sr_affiliation_actions(affiliation_code,action_id,default_price,default_kickback,
default_kickback_percentage)
values ('apple',1,null,null,null);


insert into sr_affiliation_actions(affiliation_code,action_id,default_price,default_kickback,
default_kickback_percentage)
values ('Geomagnetic',2,0.97,0.9,null);

insert into sr_affiliation_actions(affiliation_code,action_id,default_price,default_kickback,
default_kickback_percentage)
values ('Prime',2,1.10,0.12,null);

insert into sr_affiliation_actions(affiliation_code,action_id,default_price,default_kickback,
default_kickback_percentage)
values ('Killercool',2,0.97,null,null);


insert into sr_affiliation_actions(affiliation_code,action_id,default_price,default_kickback,
default_kickback_percentage)
values ('Geomagnetic',3,9.95,null,0.10);

insert into sr_affiliation_actions(affiliation_code,action_id,default_price,default_kickback,
default_kickback_percentage)
values ('Prime',3,9.95,null,0.10);

insert into sr_affiliation_actions(affiliation_code,action_id,default_price,default_kickback,
default_kickback_percentage)
values ('Killercool',3,9.95,null,null);


/*our_youtube is for our silent radiance channel */
create table sr_consumables (
consumable_id bigint auto_increment,
consumable_type varchar(20),
consumable_name varchar(400),
description varchar(2000),
template_consumable_id bigint,
notes varchar(2000),
isrc_code varchar(12),
ean varchar(16)
,
bn varchar(200),
business varchar(200),
item_name varchar(200),
item_number varchar(100),
amount decimal(20,6),
currency_code varchar(6),
shipping decimal(20,6),
shipping2 decimal(20,6),
demo_video_url varchar(200),
our_youtube_video_url varchar(200), 
vendor_youtube_video_url varchar(200),
live_youtube_video_url varchar(200),
image_url varchar(200),
imagehead_url varchar(200),
undefined_quantity decimal(20,6),
receiver_email varchar(200), 
mrb varchar(200),
pal varchar(200),
no_shipping int,
no_note int,
title varchar(200),
artist varchar(200),
label varchar(200),
catalogue varchar(200),
format varchar(200),
barcode varchar(200),
the_release varchar(200),
constraint sr_consumables_pk primary key(consumable_id),
constraint sr_consumables_uk1 unique(consumable_name),
/*constraint sr_consumables_uk2 unique(isrc_code),
constraint sr_consumabl;es_uk3 unique(ean),*/
constraint sr_consumables_fk1 foreign key (consumable_type) references sr_consumable_types(consumable_type),
constraint sr_consumables_fk2 foreign key (isrc_code) 
  references sr_international_standard_recording_codes(isrc_code)
);


alter table sr_consumables add action_id int not null;


alter table sr_consumables add affiliation_code varchar(30) not null;

alter table sr_consumables add constraint sr_consumables_fk6
 foreign key (affiliation_code,action_id) references sr_affiliation_actions(affiliation_code,action_id);


create table sr_cds (
station_name varchar(100) not null,
cd_short_name varchar(15) not null,
cmd varchar(200),
bn varchar(200),
business varchar(200),
item_name varchar(200),
item_number varchar(100),
amount decimal(20,6),
currency_code varchar(6),
shipping decimal(20,6),
shipping2 decimal(20,6),
image_url varchar(200),
imagehead_url varchar(200),
undefined_quantity decimal(20,6),
receiver_email varchar(200),
mrb varchar(200),
pal varchar(200),
no_shipping int,
no_note int,
title varchar(200),
artist varchar(200),
label varchar(200),
catalogue varchar(200),
format varchar(200),
barcode varchar(200),
the_release varchar(200),
constraint sr_cds_pk primary key (station_name,cd_short_name)
);

alter table sr_cds add column demo_video_url varchar(200);

create table sr_folders (
station_name varchar(100) not null,
cd_short_name varchar(15) not null,
folder varchar(250) not null,
unescaped_folder varchar(250) not null,
constraint sr_folders_pk primary key(station_name,folder),
constraint sr_folders_uk unique(station_name,unescaped_folder),
constraint sr_folders_fk1 foreign key (station_name,cd_short_name) 
  references sr_cds(station_name,cd_short_name)
);

create table sr_artists (
artist_id bigint not null auto_increment,
artist_code varchar(60) not null,
group_id bigint,
group_sequence int,
artist_name varchar(60) not null,
shoutout_available int not null,
shoutout_price decimal(20,6),
constraint sr_artists_pk primary key(artist_id),
constraint sr_artists_uk1 unique(artist_code),
constraint sr_artists_uk2 unique(group_id,group_sequence)
);

alter table sr_artists add 
constraint sr_artists_fk1 foreign key (group_id) references sr_artists(artist_id);






create table sr_emails (
email_address varchar(200) not null,
email_address_full varchar(200),
artist_id bigint,
artist_seq int,
constraint sr_emails_pk primary key (email_address),
constraint sr_emails_uk1 unique (email_address_full),
constraint sr_emails_uk2 unique (artist_id,artist_seq),
constraint sr_emails_dk1 foreign key (artist_id) references sr_artists(artist_id)
);


create table sr_cd_tracks (
station_name varchar(100) not null,
cd_short_name varchar(15) not null,
track int not null,
raw_track_text varchar(200),
title varchar(200),
constraint sr_cd_tracks_pk primary key(station_name,cd_short_name,track),
constraint sr_cd_tracks_fk1 foreign key(station_name,cd_short_name)
   references sr_cds(station_name,cd_short_name)
);


create table sr_cd_track_artists (
station_name varchar(100) not null,
cd_short_name varchar(15) not null,
track int not null,
sequence int not null,
type varchar(50) not null, /* Collaborator,primary,credit W&P,. CRedit Mastered by */
artist_id bigint not null,
constraint sr_cd_track_artists_pk primary key (station_name,cd_short_name,track,sequence),
constraint sr_cd_track_artists_fk1 foreign key (station_name,cd_short_name,track)
  references sr_cd_tracks(station_name,cd_short_name,track),
constraint sr_cd_track_artists_fk2 foreign key (artist_id)
 references sr_artists(artist_id)
);

create table sr_cd_artists (
station_name varchar(100) not null,
cd_short_name varchar(15) not null,
sequence int not null,
type varchar(50) not null, /* Collaborator,primary,credit W&P,. CRedit Mastered by */
artist_id bigint not null,
constraint sr_cd_artists_pk primary key (station_name,cd_short_name,sequence),
constraint sr_cd_artists_fk1 foreign key (station_name,cd_short_name)
  references sr_cds(station_name,cd_short_name),
constraint sr_cd_artists_fk2 foreign key (artist_id)
 references sr_artists(artist_id)
);



create table sr_song_info (
station_name varchar(100) not null,
 filename varchar(250) not null,
 unescaped_filename varchar(250) not null,
 length bigint,
 ean varchar(16),
 featured_artist varchar(200),
 sound_recording_title varchar(200),
 isrc varchar(12),
 album_title varchar(200),
 marketing_label varchar(200),
 track varchar(200),
 title varchar(400),
 year varchar(20),
 genre varchar(40),
 extra1 varchar(200),
 extra2 varchar(200),
 front_picture varchar(200),
 info_picture varchar(200), 
 picture_height int,
 sha256 varchar(64),
constraint sr_song_info_pk primary key (station_name,filename),
constraint sr_song_info_uk1 unique (station_name,unescaped_filename),
constraint sr_song_info_uk2 unique (filename,station_name),
constraint sr_song_info_uk3 unique (unescaped_filename,station_name),
constraint sr_song_info_fk1 foreign key (station_name,picture) references sr_picture_info(station_name,picture),
constraint sr_song_info_fk2 foreign key (station_name) references sr_stations(station_name),
/*constraint sr_song_info_fk3 foreign key (ean) references sr_international_articles(ean),*/
constraint sr_song_info_fk4 foreign key(sha256) references sr_hash_codes(sha256)
);

alter table sr_song_info add column cd_short_name varchar(15);

alter table sr_song_info add
  constraint sr_song_info_fk5 foreign key(station_name,cd_short_name) 
  references sr_cds(station_name,cd_short_name);

alter table sr_song_info add column folder varchar(250);
alter table sr_song_info add
  constraint sr_song_info_fk6 foreign key(station_name,folder) 
  references sr_folders(station_name,folder);

alter table sr_song_info add column cd_track int;
alter table sr_song_info add
  constraint sr_song_info_fk7 foreign key(station_name,cd_short_name,cd_track) 
  references sr_cd_tracks(station_name,cd_short_name,track);

alter table sr_song_info add column demo_video_url varchar(200);

create table sr_radio_plays (
 station_name varchar(100) not null,
 filename varchar(250) not null,
 start_unix_time bigint not null,
 end_unix_time bigint not null,
 length bigint not null,
 command varchar(400),
 constraint sr_radio_plays_pk primary key (start_unix_time,filename,station_name)
/*  constraint sr_radio_plays_fk1 foreign key (station_name,filename) references sr_song_info(station_name,filename */
 );




create table sr_song_info_consumables (
station_name varchar(100) not null,
filename varchar(250) not null,
consumable_id bigint not null,
display_sequence int,
for_sale_in_general int not null,
constraint sr_song_info_consumables_pk primary key (filename,station_name,consumable_id),
constraint sr_song_info_consumables_fk1
  foreign key (station_name,filename) references sr_song_info(station_name,filename),
constraint sr_song_info_consumables_fk2 
  foreign key (consumable_id) references sr_consumables(consumable_id)
);

  

/*for_sale_web int not null,
for_sale_google_app int not null,
for_sale_ios_app int not null,
 image_url varchar(250)  null,
 sound_byte_url varchar(250) null,
 video_url varchar(250)  null,
 google_play_affiliate_url varchar(250)  null,
 ios_affiliate_url varchar(250)  null, 
display_style varchar(200), /* primary:digital,primary:CD,image:digital,image:print,shoutout */
base_price_in_dollars decimal(10,2)
);
*/


create table sr_tags (
tag varchar(30) not null,
constraint sr_tags_pk primary key(tag)
);

insert into sr_tags values ('generic');

create table sr_reflinks (
reflink varchar(255) not null,
consumable_id bigint not null,
affiliation_code varchar(30) not null,
tag varchar(30) not null,
actual_link varchar(512) not null,
constraint sr_reflinks_pk primary key(reflink),
constraint sr_reflinks_uk1 unique(consumable_id,affiliation_code,tag),
constraint sr_reflinks_fk1 
  foreign key (consumable_id) references 
  sr_consumables(consumable_id),
constraint sr_reflinks_fk2 
  foreign key (affiliation_code) references 
  sr_affiliations(affiliation_code),
constraint sr_reflinks_fk3 
  foreign key (tag) references 
  sr_tags(tag)
);


create table sr_consumable_bill_of_materials (
 bill_id bigint auto_increment,
 used_action_sequence int not null,
 listed_action_sequence int not null,
 template_bill_id bigint not null,
 parent_consumable_id bigint not null,
 action_id int not null,
 child_consumable_id bigint not null,
 description varchar(2000),
 technical_notes varchar(2000),
 constraint sr_consumable_bill_of_materials_pk primary key(bill_id),
 constraint sr_consumable_bill_of_materials_uk1 unique (parent_consumable_id,used_action_sequence),
 constraint sr_consumable_bill_of_materials_uk2 unique (parent_consumable_id,listed_action_sequence),
 constraint sr_consumable_bill_of_materials_fk1 foreign key (parent_consumable_id) references  sr_consumables(consumable_id),
 constraint sr_consumable_bill_of_materials_fk2 foreign key (child_consumable_id) references  sr_consumables(consumable_id),
 constraint sr_consumable_bill_of_materials_fk3 foreign key (action_id) references sr_actions(action_id)
 );
 



create table sr_label_to_affiliations (
  label varchar(100) not null primary key,
  affiliation_code varchar(30) not null,
  constraint sr_label_to_affiliations_fk1 foreign key (affiliation_code) references sr_affiliations(
    affiliation_code));
    
 
insert into sr_label_to_affiliations values ('Ohm Ganesh Pro','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Ascending Records','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Audio Nautix','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Bass Star Records','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Digital Drugs Coalition','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Dubstep SF','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Fresh Frequencies','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('GeoMagnetic Records','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Goa Records','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Killer Cool Development','Killercool');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Killer Cool Development, LLC.',
'Killercool');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Last known Recording','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('markjeting_label','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Ohm Ganesh Pro','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Parabolic Music','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Powerhouse Records','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('Prime Records','Prime');
insert into sr_label_to_affiliations(label,affiliation_code) values ('SpiralTrax Records','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('The Great Beyond','Geomagnetic');
insert into sr_label_to_affiliations(label,affiliation_code) values ('The Live Tapes','Geomagnetic');

