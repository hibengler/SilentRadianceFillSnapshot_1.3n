
/*
  $Revision: 1.1 $
  $Log: journal_code.sql,v $
  Revision 1.1  2018/12/28 20:45:36  hib
  Well, added some stuff - thinking to make oscommerse into a innodb database instead of isam, and then
  add journal triggers on it.

  Revision 1.3  2016/02/29 09:26:44  hib
  Task 669

  Revision 1.2  2012/01/30 09:11:22  hib
  the yahoo version of mysql does not have views.   Needed to rework the report.

*/


delimiter |
drop procedure ddl_create_journal_table|

create procedure ddl_create_journal_table(the_schema varchar(100),the_table varchar(100) )
begin
declare done boolean default 0;
declare xtext varchar(30000);
declare xtable_schema varchar(64);
declare xtable_name varchar(64);
declare xanswer varchar(8000);
declare xcomma varchar(100);

 /* table head definiton */
declare c1 cursor for
select concat('Create table `', table_name,'_jn` (',char(10),
"jn_operation                    varchar(1)          NOT NULL,",char(10),
"jn_oracle_user                  varchar(30)         NOT NULL,",char(10),
"jn_datetime                     datetime  NOT NULL,",char(10),
"jn_endtime                     datetime not NULL,",char(10),
"jn_notes                        varchar(240)  , ",char(10),
"jn_appln                        varchar(30)   ,",char(10),
"jn_session                      int           ")
from information_schema.tables
where table_schema = the_schema
  and table_name = the_table
and not exists (select 'x' from information_schema.tables t2
  where t2.table_schema = the_schema
  and t2.table_name = concat(the_table,'_jn'));



declare c2 cursor for 
select concat('`',column_name,'` ',column_type," ",
 case is_nullable when 'NO' then ' not null' else '' end)
from information_schema.columns
where table_schema = the_schema
  and table_name = the_table
order by ordinal_position;

declare c3 cursor for 
select concat(case seq_in_index when 1 then '' else " , " end,'`',column_name,'`')
from information_schema.statistics
where table_schema = the_schema
  and table_name = the_table
  and index_name='PRIMARY'
order by seq_in_index;


declare continue handler for sqlstate '02000' set done=1;

set xtext = '';
open c1;
fetch c1 into xanswer;
while ! done do
  set xtext = xanswer;

  set done = 0;


  open c2;
  fetch c2 into xanswer; 
  while ! done do
    set xtext = concat(xtext,char(10),' ,',xanswer);
    fetch c2 into xanswer;
  end while;
  close c2;


  set done = 0;
  set xtext = concat(xtext,char(10),', constraint `',the_table,'_jn_pk` primary key (');



  set done = 0;
  open c3;
  fetch c3 into xanswer; 
  while ! done do
    set xtext = concat(xtext,char(10),xanswer);
    fetch c3 into xanswer;
  end while;
  close c3;

  set xtext = concat(xtext,",jn_datetime)",char(10),', constraint `',the_table,'_jn_uk1` unique (');

  set done = 0;
  open c3;
  fetch c3 into xanswer; 
  while ! done do
    set xtext = concat(xtext,char(10),xanswer);
    fetch c3 into xanswer;
  end while;
  close c3;

  set xtext = concat(xtext,",jn_endtime)",char(10),");");

  select xtext;

  set @s = xtext;
  prepare stmt from @s;
  execute stmt;
  deallocate prepare stmt;
  fetch c1 into xanswer;
  end while;
close c1;
end;
|
delimiter ;












































delimiter |
drop procedure ddl_create_journal_trigger|

create procedure ddl_create_journal_trigger(the_schema varchar(100),the_table varchar(100) )
begin
declare done boolean default 0;
declare xtext varchar(60000);
declare xtable_schema varchar(64);
declare xtable_name varchar(64);
declare xanswer varchar(8000);
declare xcolumn varchar(8000);
declare xcomma varchar(100);

 /* table head definiton */
declare c1 cursor for
select concat('drop trigger `', trigger_name,'`|')
from information_schema.triggers
where trigger_schema = the_schema
  and trigger_name in (concat(the_table,'_ji'),concat(the_table,'_ju'),concat(the_table,'_jd'));


declare c2 cursor for 
select column_name
from information_schema.columns
where table_schema = the_schema
  and table_name = the_table
order by ordinal_position;

declare c3 cursor for 
select concat(case seq_in_index when 1 then '' else concat(char(10),"  and ") end,'`',column_name,'`'),column_name
from information_schema.statistics
where table_schema = the_schema
  and table_name = the_table
  and index_name='PRIMARY'
order by seq_in_index;


declare continue handler for sqlstate '02000' set done=1;

set xtext = concat(char(10),'delimiter |');
/* drop old triggers */
open c1;
fetch c1 into xanswer;
while ! done do
  set xtext = concat(xtext,char(10),xanswer);
  set done = 0;
  fetch c1 into xanswer;
  end while;
close c1;

/* setup insert */

set xtext = concat(xtext,char(10),'create trigger `',the_table,'_ji` after insert on `',the_table,'` for each row begin',char(10));
  
set xtext = concat(xtext,'  declare xdate datetime;',char(10),
  '  set xdate = now();',char(10),
  'delete from `',the_table,'_jn` where ');

set done = 0;
open c3;
fetch c3 into xanswer,xcolumn; 
while ! done do
  set xtext = concat(xtext,xanswer,'=new.`',xcolumn,'`');
  fetch c3 into xanswer,xcolumn;
end while;
close c3;

set xtext = concat(xtext,char(10),
  '  and jn_datetime = xdate;',char(10),char(10));
  
set xtext = concat(xtext,'update `',the_table,'_jn` set jn_endtime = xdate',char(10),
  'where ');

set done = 0;
open c3;
fetch c3 into xanswer,xcolumn; 
while ! done do
  set xtext = concat(xtext,xanswer,'=new.`',xcolumn,'`');
  fetch c3 into xanswer,xcolumn;
end while;
close c3;

set xtext = concat(xtext,char(10),
  '  and jn_datetime <= xdate',char(10),
  '  and jn_endtime > xdate;',char(10),char(10));


set xtext = concat(xtext,'insert into `',the_table,'_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session');

set done = 0;
open c2;
fetch c2 into xanswer; 
while ! done do
  set xtext = concat(xtext,char(10),' ,`',xanswer,'`');
  fetch c2 into xanswer;
end while;
close c2;

set xtext = concat(xtext,char(10),") values ('i',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()");

set done = 0;
open c2;
fetch c2 into xanswer; 
while ! done do
  set xtext = concat(xtext,char(10),' ,new.`',xanswer,'`');
  fetch c2 into xanswer;
end while;
close c2;

set xtext = concat(xtext,char(10),");",char(10),"end;|",char(10));



/* setup update */

set xtext = concat(xtext,char(10),'create trigger `',the_table,'_ju` after update on `',the_table,'` for each row begin',char(10));
  


set xtext = concat(xtext,'  declare xdate datetime;',char(10),
  '  set xdate = now();',char(10),
  'delete from `',the_table,'_jn` where ');

set done = 0;
open c3;
fetch c3 into xanswer,xcolumn; 
while ! done do
  set xtext = concat(xtext,xanswer,'=old.`',xcolumn,'`');
  fetch c3 into xanswer,xcolumn;
end while;
close c3;

set xtext = concat(xtext,char(10),
  '  and jn_datetime = xdate;',char(10),char(10));
  
set xtext = concat(xtext,'update `',the_table,'_jn` set jn_endtime = xdate',char(10),
  'where ');

set done = 0;
open c3;
fetch c3 into xanswer,xcolumn; 
while ! done do
  set xtext = concat(xtext,xanswer,'=old.`',xcolumn,'`');
  fetch c3 into xanswer,xcolumn;
end while;
close c3;

set xtext = concat(xtext,char(10),
  '  and jn_datetime <= xdate',char(10),
  '  and jn_endtime > xdate;',char(10),char(10));


set xtext = concat(xtext,'insert into `',the_table,'_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session');

set done = 0;
open c2;
fetch c2 into xanswer; 
while ! done do
  set xtext = concat(xtext,char(10),' ,`',xanswer,'`');
  fetch c2 into xanswer;
end while;
close c2;

set xtext = concat(xtext,char(10),") values ('u',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()");

set done = 0;
open c2;
fetch c2 into xanswer; 
while ! done do
  set xtext = concat(xtext,char(10),' ,new.`',xanswer,'`');
  fetch c2 into xanswer;
end while;
close c2;

set xtext = concat(xtext,char(10),");",char(10),"end;|",char(10));






/* setup delete */

set xtext = concat(xtext,char(10),'create trigger `',the_table,'_jd` after delete on `',the_table,'` for each row begin',char(10));

  
    

set xtext = concat(xtext,'  declare xdate datetime;',char(10),
  '  set xdate = now();',char(10),
  'delete from `',the_table,'_jn` where ');

set done = 0;
open c3;
fetch c3 into xanswer,xcolumn; 
while ! done do
  set xtext = concat(xtext,xanswer,'=old.`',xcolumn,'`');
  fetch c3 into xanswer,xcolumn;
end while;
close c3;

set xtext = concat(xtext,char(10),
  '  and jn_datetime = xdate;',char(10),char(10));
  
set xtext = concat(xtext,'update `',the_table,'_jn` set jn_endtime = xdate',char(10),
  'where ');

set done = 0;
open c3;
fetch c3 into xanswer,xcolumn; 
while ! done do
  set xtext = concat(xtext,xanswer,'=old.`',xcolumn,'`');
  fetch c3 into xanswer,xcolumn;
end while;
close c3;

set xtext = concat(xtext,char(10),
  '  and jn_datetime <= xdate',char(10),
  '  and jn_endtime > xdate;',char(10),char(10));


set xtext = concat(xtext,'insert into `',the_table,'_jn` (jn_operation,jn_oracle_user,jn_datetime,jn_endtime,jn_notes,jn_appln,jn_session');

set done = 0;
open c2;
fetch c2 into xanswer; 
while ! done do
  set xtext = concat(xtext,char(10),' ,`',xanswer,'`');
  fetch c2 into xanswer;
end while;
close c2;

set xtext = concat(xtext,char(10),") values ('d',current_user(),xdate,STR_TO_DATE('2500-01-01','%Y-%m-%d'),null,null,connection_id()");

set done = 0;
open c2;
fetch c2 into xanswer; 
while ! done do
  set xtext = concat(xtext,char(10),' ,old.`',xanswer,'`');
  fetch c2 into xanswer;
end while;
close c2;

set xtext = concat(xtext,char(10),");",char(10),"end;|",char(10),"delimiter ;",char(10));

select xtext;
/*set @s = xtext;
prepare stmt from @s;
execute stmt;
deallocate prepare stmt;*/
end;
|      
delimiter ';'
















/*
call ddl_create_journal_trigger('hib','lcd_modules');



select * from lcd_modules_jn;

insert into lcd_modules values (-9,1,111,1,'hi','hi','hello','gff','fff',null);
update lcd_modules set codex='sdfddf' where module_id=-9;
delete from lcd_modules where module_id=-9;

  
*/
