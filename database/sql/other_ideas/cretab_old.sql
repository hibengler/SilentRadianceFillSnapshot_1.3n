create table artists (
artist_id,
);



/* isrc http://www.ifpi.org/content/library/isrc_handbook.pdf
uniquely identifying sound recordings 
The International Standa
rd Recording Code (ISRC) provides a means of uniquely identifying
sound
recordings and music videos internationally. For the purposes of this handbook, a music
video
recording is defined as a short form music video initially produced to support the release
of a track.


Each distinct recording is to have its own unique ISRC
The first owner or current owner of the rights to a recor
ding normally assigns the ISRC,
p
roviding no
ISRC has been issued before. It identifies the recording throughout its life and is
intended for use by
producers of sound recordings and music videos (i.e. record labels), as
well as by copyright
organisations, broadcasting organisations, libr
aries, licensees etc.


Any new or materially changed recording must be provided with a new ISRC (see Section
4.9.1Re
-
mixes/ Edits / ). Re
-
use of an ISRC that has already been allocated for another
recording is not
permitted see Section 4.1.3 No re
-
use.





*/



/* 3901-2001 ISRC */




/* ISO 3166 
https://www.iso.org/obp/ui/#search/code/
*/
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
entity_name,address,other_details,department,person_name)
...



create table isrc_registrant_codes (
country_code varchar(2) not null references isrc_country_codes(isrc_country_codes);
registrant_code varchar(3) not null,
registrant_id int references isrc_entities(registrant_id),
date_registrant_code_issused date,
constraint isrc_registrant_codes_pk primey key (country_code,registrant_code)
);



create table isrc_registrant_years ( 
country_code varchar(2) not null references isrc_country_codes(isrc_country_codes);
registrant_code varchar(3) not null,
registrant_year varchar(2) not null references isrc_years(year_code),
constraint isrc_registrant_years_pk primary key (country_code,registrant_code),
constraint isrc_registrant_years_fk1 foreign key (country_code,registrant_code) references 
 isrc_registrant_codes(country_code,registrant_code)
);







/*http://www.ifpi.org/content/library/isrc_handbook.pdf */
create table isrcs (
isrc_code varchar(12) not null primary key,
country_code varchar(2) not null,
registrant_code varchar(3) not null,
year_of_reference varchar(2) not null references isrc_years(year_code),
designation_code varchar(5) not null


registrant_code,
audio_or_video,
registrant_name_and_address,
department_or_person_to_contact,
date_registrant_code_issued,
)


licensee
licensor


