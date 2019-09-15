#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "credit_database.h"
#include "glue.h"
/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017-2019 Hibbard M. Engler

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License   
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/








// WORLD_INTERNATIONAL off

int create_initial_tables() {
do_sql("CREATE TABLE IF NOT EXISTS events(event VARCHAR,event_month VARCHAR,"
"  number_of_creds int,"
"  redemption_code varchar,"
"  valid_flag int not null"
"  ,"
"PRIMARY KEY(event,event_month));");

do_sql("create table if not exists djs(dj varchar primary key);");

do_sql("CREATE TABLE IF NOT EXISTS credits(credit_sign varchar primary key,"
"                                   credit_base varchar,"
"                                   event varchar not null,"
"                                   event_month varchar not null,"
"                                   dj varchar not null references djs(dj),"
"                                   event_utc varchar not null,"
"                                   ip_address varchar not null,"
"                                   valid_flag int not null,"
"                                   foreign key (event,event_month) references events(event,event_month)"
"                                   );");

do_sql("create index credits_idx1 on credits(event,event_month);");
return 0;
}



static char *quote(char *to_buffer,char *quoted) {
int i;
int j;
if (!quoted[0]) {
  strcpy(to_buffer,"NULL");
  return to_buffer;
  }
j=0;
to_buffer[j++] = 39;
for (i=0;quoted[i];i++) {
  to_buffer[j]=quoted[i];
  if (to_buffer[j]==39) {
    j++;
    to_buffer[j]=39;
    }
  j++;
  }
to_buffer[j++]=39;
to_buffer[j++]='\0';
return to_buffer;
}

// 'R|test|hib|2016-09-16 01:05:33|10.70.178.80|d4fb704b647984a05b64295d84f92eec9f19dbae7b9a47a0b4a7f219b74b71f1'

int load_credit(char *token[],int count) {
char command[4000]; /* good because fields are guarenteed to be less than 256 */
if (count<6) return -1;
char *event = token[1];
char *dj = token[2];
char *date = token[3];
char *ip = token[4];
char *credit_sign = token[5]; /* hash of the signature */
char year_month[10];

/* make the year month */
strncpy(year_month,date,7);
year_month[7]='\0';

char a[400];
char b[400];
char c[400];
char d[400];
char e[400];
char f[400];

/* add dj if not there */
sprintf(command,"insert into djs(dj) values(%s);",quote(a,dj));
do_sql(command);

/* add event if not there */
sprintf(command,"insert into events(event,event_month,number_of_creds,valid_flag) "
" values(%s,%s,0,1);",quote(a,event),quote(b,year_month));
do_sql(command);

/* add credit */
sprintf(command,"insert into credits(credit_sign,event,event_month,dj,event_utc,ip_address,valid_flag)"
  "values (%s,%s,%s,%s,%s,%s,1);",quote(a,credit_sign),quote(b,event),quote(c,year_month),quote(d,dj),
  quote(e,date),quote(f,ip));
if (do_sql(command)) return -2;

/* update the count - if we added a record */
sprintf(command,"update events set number_of_creds = number_of_creds + 1 where event=%s "
  "and event_month=%s;",quote(a,event),quote(b,year_month));
do_sql(command);
  

return(0);
}




extern int get_credit_count_from_database(char *event,char * event_month) {
char command[4000];
char a[400];
char b[400];
sprintf(command,"select count(*) from credits where event=%s "
  "and event_month=%s;",quote(a,event),quote(b,event_month));
return(select_int_sql(command));
    
}
