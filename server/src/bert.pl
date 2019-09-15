#!/usr/bin/perl467
$e == <<EOF;
/*
Silent Radiance wifi broadcast for digital silent disco.
Copyright (C) 2017 Hibbard M. Engler

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

EOF



use Time::HiRes qw( usleep ualarm gettimeofday tv_interval nanosleep
clock_gettime clock_getres clock_nanosleep clock
stat );

my $NUMBER_KMTRONICS = 10;
my $number_switches = $NUMBER_KMTRONICS * 8;

my @kmtronic;
my @kmfiledescriptor;

$kmtronic[0] = $ENV{"A"};
$kmtronic[1] = $ENV{"B"};
$kmtronic[2] = $ENV{"C"};
$kmtronic[3] = $ENV{"D"};
$kmtronic[4] = $ENV{"E"};
$kmtronic[5] = $ENV{"F"};
$kmtronic[6] = $ENV{"G"};
$kmtronic[7] = $ENV{"H"};
$kmtronic[8] = $ENV{"I"};
$kmtronic[9] = $ENV{"J"};




my $i;
for ($i=0;$i<$NUMBER_KMTRONICS;$i++) {
  open($kmfiledescriptor[$i],">",$kmtronic[$i]) or die "cannot open " . $kmtronic[$i] . " for writing\n";
  select $kmfiledescriptor[$i];
#  $|=1;
  }
select STDOUT;






# on (0-80)
sub on1 {
my ($id) = @_;
my $fd;
$id = ($id % ($number_switches));
$fd = $kmfiledescriptor[int($id/8)];
#print STDERR "on " . int($id/8) . " : " . (($id % 8) + 1) . "\n";
print $fd chr(255) . chr(($id % 8) + 1) . chr(1) ;
}
 



# on (0-15)
sub off1 {
my ($id) = @_;
my $fd;
$id = ($id % ($number_switches));
$fd = $kmfiledescriptor[int($id/8)];
print $fd chr(255) . chr(($id % 8) + 1) . "\0" ;
}



sub sync
{
my @state = @_;
my @s = @state;
my $need_to_undo = 0;
my @sent;

for (my $i=0;$i<$NUMBER_KMTRONICS;$i++) {
  $sent[$i] = 0;
  }

for (my $i = 0;$i<$number_switches;$i++) {
  if ($state_array[$i] != $state[$i]) {
    if ($state[$i]) {
      &on1($i);
      $sent[int($i/8)]++;
      }
    else {
      &off1($i);
      $sent[int($i/8)]++;
      }
    $state_array[$i] = $state[$i];
    }
  }
for (my $i=0;$i<$NUMBER_KMTRONICS;$i++) {
  if ($sent[$i]) {
    my $fd = $kmfiledescriptor[$i];
    print $fd "\n";
    }
  }
}







@state_array = ();
@next_state =  ();
for ($i=0;$i<$number_switches;$i++) {
  $state_array[$i]=1;
  $next_state[$i]=0;
  }









sub simple_test {

for ($i=0;$i<1000000;$i++) {
  $n = $i % $number_switches;
  for ($j=0;$j<$number_switches;$j++) {
    $next_state[$j] = int(rand()*2.0);
    }
  $next_state[$n] = 1;
  &sync(@next_state);
  $time = 10000 + rand()*300000;
  $tempo = 61*3;
  $tempo_per_second = $tempo / 60.;
  $seconds_per_beat = 1.0 / $tempo_per_second;
  $microseconds_per_beat = $seconds_per_beat * 1000000.;
   usleep($microseconds_per_beat);
  

  
  
  
  # about 8 ms absolute min
  
  }
}






$fliptree = 0;
@tranalate_to = ();
@translate_from = ();
@tranlate_bits = ();
# virtual space -> flip_tree? -> translate_to bits -> translate_bits -> next_state
# syn will sync everything that needs to be synced.

# the sections is 2d arrys 0 -> 7 y * 0->39 x
#
@grid = ();
@virtual_state = ();

# 2d is a 2 minsional array represented as a single array of 320 points.  8 * 40
# then we need to tranlate that into x and y for the sincles
#
# panel 0
# rows
# 0 
# 1 
# 2
# 3
# 4
# 5
# 6
# 7
# These rows are on if there are any coluns under the row that are on.
# Coluns
# 1 2 3 4 5 6 7 8
# 0 1 2 3 4 5 6 7
# These coluns are on if there are any rows under the column that are on.
#


sub translate_virtual_to_actual {
my @flipper;
# flip the tree for the panel
if ($fliptree) {
  for ($i=0;$i<80;$i++) {
    $flipper[$i] = $virtual_state[ 1-($i%2) + int($i/2)*2 ];
    }
  }
else {
  for ($i=0;$i<80;$i++) {
    $flipper[$i] = $virtual_state[$i];
    }
  }
# print " do the translate  
for ($i=0;$i<80;$i++) {
  $next_state[$i] = 1-$flipper[$i];
  }
}



sub init_2d() {
my $i;
my $j;
for ($i=0;$i<80;$i++) {
  $translate_to[$i] = $i;
  $translate_from[$i] = $i;
  $translate_bits[$i] = 1;
  }
for ($i=0;$i<8;$i++) {
  for ($j=0;$j<40;$j++) {
    $grid[$i*40+$j] = 0;
    }
  }
for ($i=0;$i<80;$i++) {
  $virtual_state[$i] = 0;
  }
}


sub on_panel() {
my ($panel_number) = @_;
my $c = $panel_number * 8;
my $i;
my $j;

for ($i=0;$i<8;$i++) {
  for ($j=0;$j<8;$j++) {
    
    $grid[$i*40+$j+$c] = 1;
    }
  }
}


sub off_panel() {
my ($panel_number) = @_;
my $c = $panel_number * 8;
my $i;
my $j;
for ($i=0;$i<8;$i++) {
  for ($j=0;$j<8;$j++) {
    $grid[$i*40+$j+$c] = 0;
    }
  }
}



sub set_panel() {
my ($panel_number,$value) = @_;
my $c = $panel_number * 8;
my $i;
my $j;
for ($i=0;$i<8;$i++) {
  for ($j=0;$j<8;$j++) {
    $grid[$i*40+$j+$c] = 0;
    }
  }
}




sub show_grid {
for ($i=0;$i<8;$i++) {
  for ($j=0;$j<40;$j++) {
    if ($grid[40*$i+$j]) {
      if ($i==0) {
        print "b";
	}
      else {
        print "*";
	}
      }
    else {
      print ".";
      }
    }
  print "\n";
  }
}


sub show_states {
my @state = @_;
my $i,$j;
for ($i=0;$i<10;$i++) {
  printf ("   %2d    ",($i+1)); 
  }
printf("\n");
for ($i=0;$i<10;$i++) {
  for ($j=0;$j<8;$j++) {
    if ($state[$i*8+$j]) {
      print "*";
      }
    else {
      print ".";
      }
    }
  print " ";
  }
printf("\n");  
}     


sub sync_2d {
my $panel_number;
#show_grid();
for ($panel_number = 0; $panel_number <5;$panel_number++) {
  if (&panel_is_stable($panel_number)) {
    my $i;$j;
    my $c;
    my $rowpos = $panel_number * 16;
    my $columnpos = $panel_number * 16 + 8;
    $c = $panel_number*8;
    # initialize all the rowpos and column pos to 0
    for ($i=0;$i<8;$i++) {
      $virtual_state[$rowpos+$i] = 0;
      $virtual_state[$columnpos+$i] = 0;      
      }

    # set the row and column to 1 if they are set. 
    # we already checked to see if they are stable    
    my $c = $panel_number * 8;
    for ($i=0;$i<8;$i++) {
      for ($j=0;$j<8;$j++) {  
        if ($grid[$i*40+$c+$j]) {
	  $virtual_state[$rowpos+$i] = 1;
	  $virtual_state[$columnpos+$j] = 1;
	  } # grid =
	} # for each x
      } # for each y
    } # if we are stable 
  else { # panel is unstable - we cannot do all rows.  Just do one row.
    my $i;$j;
    my $choosen_column;
    my $rowpos = $panel_number * 16;
    my $columnpos = $panel_number * 16 + 8;
    
#    if (int(rand()*2)==0) {
if (1==1){
#      $choosen_column = int(rand()*8);
      $choosen_column = $xcount % 8;
      # initialize all the rowpos and column pos to 0
      for ($i=0;$i<8;$i++) {
        $virtual_state[$rowpos+$i] = 0;
        $virtual_state[$columnpos+$i] = 0;      
        }

      $virtual_state[$columnpos + $choosen_column] = 1;
    
      # set the row and column to 1 if they are set. 
      # we already checked to see if they are stable    
      my $c = $panel_number * 8;
      for ($i=0;$i<8;$i++) {
        if ($grid[$i*40+$c+$choosen_column]) {
          $virtual_state[$rowpos+$i] = 1;
  	  } # grid =
        } # for each y
      } # if done by column
    else {
      $choosen_row = int(rand()*8);
      # initialize all the rowpos and column pos to 0
      for ($i=0;$i<8;$i++) {
        $virtual_state[$rowpos+$i] = 0;
        $virtual_state[$columnpos+$i] = 0;      
        }

      $virtual_state[$rowpos + $choosen_row] = 1;
    
      # set the row and column to 1 if they are set. 
      # we already checked to see if they are stable    
      my $c = $panel_number * 8;
      for ($j=0;$j<8;$j++) {
        if ($grid[$choosen_row*40+$c+$j]) {
          $virtual_state[$columnpos+$j] = 1;
  	  } # 
        } # for each x
      } # if done by row      
    } # if we are stable 
  }
#show_states(@virtual_state);
&translate_virtual_to_actual();
#show_states(@next_state);
&sync(@next_state);
}








sub calibrate() {
my ($panel,$x,$y) = @_;
  &off_panel(0);  
  &off_panel(1);  
  &off_panel(2);  
  &off_panel(3);  
  &off_panel(4);  
if ($panel != (-1)) {
  &calibrate_panel($panel,$x,$y);
  }
elsif ($x > 8) {
  &calibrate_panel(int($x/8),$x%8,$y);
  }
else {
  my $i;
  for ($i=0;$i<5;$i++) {
    &calibrate_panel($i,$x,$y);
    }
  }    
}






sub calibrate_panel() {
my ($panel,$x,$y) = @_;
$c = $panel * 8;
my $i,$j;
# clear out the 
for ($i=0;$i<8;$i++) {
  for ($j=0;$j<8;$j++) {
    $grid[$i*40+$c+$j] = 0;
    }
  }

if ($x != (-1)) {
  if ($y != (-1)) {
    # single x,y coordinate
    $grid[$y*40+$c+$x] = 1;
    } # single pixel
  else { # all x column
    for ($i=0;$i<8;$i++) {
      $grid[$i*40+$c+$x] = 1;
      }
    }
  } # there is a defined x
else { # x is -1
  if ($y != (-1)) {
    # all y column
    for ($j=0;$j<8;$j++) {
      $grid[$y*40+$c+$j] = 1;
      }
    } # if Y is defined
  else {  # Lets just set the whole thing on
    for ($i=0;$i<8;$i++) {
      for ($j=0;$j<8;$j++) {
        $grid[$i*40+$c+$j] = 1;
	print STDERR " i $i, c $c, j $j\n";
        }
      }
    } # if we are all on one panel
  } # OK
&sync_2d();
}

sub panel_is_stable {
my ($panel) = @_;
@row_grid_types = ();
$row_grid_type_count = 0;
@column_grid_types = ();
$column_grid_type_count = 0;
my $c = $panel_number * 8;
my $i,$j,$c;
my $s;
$c = $panel * 8;
# find row grid types - and if more than 1, we need a complex build
for ($i = 0;$i<8;$i++) {
  $s = "";
  for ($j = 0;$j<8; $j++) {
    if ($grid[$i*40+$c+$j]) {
      $s = $s . "*";
      }
    else {
      $s = $s . ".";
      }
    }
  for ($j=0;$j<$row_grid_type_count;$j++) {
    if ($s eq $row_grid_types[$j]) {
      $s = "";
      break;
      }
    }
  if (($s ne "")&&($s ne "00000000")) {
    $row_grid_types[$row_grid_type_count++] = $s;
    }
  }
#  print stderr "row grid is $row_grid_type_count\n";
if ($row_grid_type_count <=1) { return(1);}      
return(0);
}





init_2d();
print STDERR "this takes three numbers: Panel x y\n";
print STDERR " where panel is the panel number (0-4), or -1 for all panels or fiull x\n";
print STDERR " x is the x coordinAte 0-7 within panel, if higher and panel is -1, can do the entiry 0-39 x \n";
print STDERR " y is the y coordinate (0-7) 0 is the top black light\n";


if (1==0) {
while (<>) {
  $w = $_;
  my ($panel,$x,$y) = split(/ /,$w);
  my $u;
  print "Calibrating panel $panel,x $x,y $y\n";
#  &calibrate($panel,$x,$y);
for ($i=0;$i<40*8;$i++) {
  $grid[$i] = 0;
  }
for ($i=0;$i<8;$i++) {
  $grid[$i*40+$i+1] = 1;
  }
sync_2d();  
#  for ($u=0;$u<5;$u++) {
#    print STDERR chr(65+$u) . " Calibration\n";
#    &calibrate($u,-1,-1);
#    sleep(10);
#    }
  print "try again...\n\n";
  }
  }
  


$xcount = 0;
while (1) {

for ($i=0;$i<40*8;$i++) {
  $grid[$i] = 0;
  }
sync_2d();
$w = (int($count/150) % 40) -1;
for ($z=0;$z<5;$z++) {

  for ($i=1;$i<8;$i++) {
    $grid[$i*40+$i-1+$z*8] = 1;
    $grid[$i*40+8-$i-1+$z*8] = 1;
#  $grid[$i*40]=1;
  $xcount = $xcount + 1;  
    }
  }
sync_2d();
usleep(15000);
}
