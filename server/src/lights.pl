#!/usr/bin/perl

$e = <<EOF;
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

use IO::Socket::INET;
use IO::Select;
use Term::ReadKey;

use Time::HiRes qw( usleep ualarm gettimeofday tv_interval nanosleep
clock_gettime clock_getres clock_nanosleep clock
stat );



$a = $ENV{"A"};
$b = $ENV{"B"};



$safety_max = 12;






open($afd,">",$a) or die "cannot open $a for writing\n";
select $afd;
#$|=1;
open($bfd,">",$b) or die "cannot open $B for writing\n";
select $bfd;
#$|=1;
select STDOUT;






# on (0-15)
sub on1 {
my ($id) = @_;
my $fd;
$id = ($id % 16);
if ($id >= 8) {
  $id = $id - 8;
  $fd = $bfd;
  }
else {
  $fd = $afd;
  }
print $fd chr(255) . chr($id + 1) . chr(1) ;
}
 



# on (0-15)
sub off1 {
my ($id) = @_;
my $fd;
$id = ($id % 16);
if ($id >= 8) {
  $id = $id - 8;
  $fd = $bfd;
  }
else {
  $fd = $afd;
  }
print $fd chr(255) . chr($id + 1) . "\0";
}



sub sync
{
my @state = @_;
my @s = @state;
my $c=0;
my $need_to_undo = 0;

# count the number on - and make sure it is safe
for (my $i = 0;$i<16;$i++) {
  if ($state[$i]) { $c++; }
  }
# randomly turn some off if it is too many
$need_to_undo = $c - $safety_max;
while ($need_to_undo>0) {
  my $r = int(rand(16));
  if ($state[$r]) {
    $state[$r] = 0;
    $need_to_undo --;
    }
  }

for (my $i = 0;$i<16;$i++) {
  if ($state_array[$i] != $state[$i]) {
    if ($state[$i]) {
      &on1($i);
      }
    else {
      &off1($i);
      }
    $state_array[$i] = $state[$i];
    }
  }
print $afd "\n";
print $bfd "\n";
}







@state_array = (1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
@next_state =  (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);







sub initial_startout_reference {

my %startout = {
  "tempo"   , 121,
  "_microseconds_per_tempo" , (1000000 * 60/  (121)),
  "length"  , 8,
  "_length_microseconds", 
  "repeat"  , 1,
  "start_time" , 0.,
  "end_time" , 0.,
  };

$startout{'_microseconds_per_tempo'} = int((1000000 * 60 / $startout{'tempo'}));
$startout{"_length_microseconds"} 
  = $startout{'_microseconds_per_tempo'} * $startout{'length'};

  
my %startout_info = {
  "start_time" ,0.,
  "end_time" , 0.,
  "length" , 0.,
  "tempo" , 121,
  };

$startout_info{'_microseconds_per_tempo'} = int((1000000 * 60 / $startout_info{'tempo'}));
$startout_info{'_start_time_microsencds'} = 
#   $startout_info('start_time') * startout_info{'_microseconds_per_tempo'};
#$startout_info{'_end_time_microsencds'} = 
#   $startout_info('end_time') * startout_info{'_microseconds_per_tempo'};
#$startout_info{'_length_microsencds'} = 
#   $startout_info('length') * startout_info{'_microseconds_per_tempo'};
   
my  @lights = (".",".",".",".",".",".",".",".",
               ".",".",".",".",".",".",".",".");
	       

$startout_info{'lights'} = \@lights;

@point_array = ($startout_info);

$startout{'points'} = \@point_array;
\%startout;
}





sub read_file() {

my ($filename,$ref_to_copy) = (@_);
#%info = *%ref_to_copy;
my $fd;
fopen($fd,$filename,"<",$filename);

  select $fd;
  my @ary = ();
  $info{'points'} = \@ary;
  $info{'start_time'} = 0;
  $info{'end_time'} = 0;
  
  while (<>) {
    $line = @_;
    if ($line !~ /^\#/) {
      }
    else {
      @values = split(/\|/,$line);
      if ($values[0] eq "lights") {
        my ($numerator,$denominator,$stuff);
	$numerator = $values[1];
	$denominator = $values[2];
	$stuff = $values[3];
	my %light_info = {
	  "numerator", $numerator,
	  "denominator",$denominator};
	  
        $light_info{"start_time"} = $info{'end_time'};
      } # if we are not a comment, and actual command
    }
}

  
\%info;
}












sub simple_test {

for ($i=0;$i<1000000;$i++) {
  $n = $i % 16;
  for ($j=0;$j<16;$j++) {
    $next_state[$j] = 0;
    }
  $next_state[$n] = 1;
  &sync(@next_state);
  usleep(1000000);
  # about 8 ms absolute min
  
  }
}






sub fornow {
<<<<<<< lights.pl
&init_synchronization();
$tempo = 1000000*60/60;
=======
&init_synchronization();
$tempo = 1000000*45/60;
>>>>>>> 1.5
@sequence = ();
@wait = ();
$b = 1;
for ($j=0;$j<2;$j++) {
  $beat = $tempo / 6.;
 for ($i=0;$i<4;$i++) {
  push(@wait,0); push(@sequence,"A 6");
 
  push(@wait,$beat); push(@sequence,"....*...........");
  push(@wait,$beat); push(@sequence,".....*..........");
  push(@wait,$beat); push(@sequence,"......*.........");
  push(@wait,$beat); push(@sequence,".......*........");
  push(@wait,$beat); push(@sequence,"........*.......");
  push(@wait,$beat); push(@sequence,".........*......");
  }
 $beat = $tempo / 8.;
 for ($i=0;$i<2;$i++) {
  push(@wait,0); push(@sequence,"b 8");
  
  push(@wait,$beat); push(@sequence,"..........*.....");
  push(@wait,$beat); push(@sequence,"..........*.....");
  push(@wait,$beat); push(@sequence,"...........*....");
  push(@wait,$beat); push(@sequence,"...........*....");
  push(@wait,$beat); push(@sequence,"............*...");
  push(@wait,$beat); push(@sequence,".............*..");
  push(@wait,$beat); push(@sequence,"..............*.");
  push(@wait,$beat); push(@sequence,"...............*");
  
  push(@wait,0); push(@sequence,"B 8");
  
  push(@wait,$beat); push(@sequence,"..............*.");
  push(@wait,$beat); push(@sequence,".............*..");
  push(@wait,$beat); push(@sequence,"............*...");
  push(@wait,$beat); push(@sequence,"............*...");
  push(@wait,$beat); push(@sequence,"...........*....");
  push(@wait,$beat); push(@sequence,"...........*....");
  push(@wait,$beat); push(@sequence,"..........*.....");
  push(@wait,$beat); push(@sequence,"...........*....");
  }
 } 
 
for ($j=0;$j<1;$j++) {
  $beat = $tempo / 6.;
 for ($i=0;$i<4;$i++) {
  push(@wait,0); push(@sequence,"A 6");
  
  push(@wait,$beat); push(@sequence,"....*...........");
  push(@wait,$beat); push(@sequence,".....*..........");
  push(@wait,$beat); push(@sequence,"......*.........");
  push(@wait,$beat); push(@sequence,".......*........");
  push(@wait,$beat); push(@sequence,"........*.......");
  push(@wait,$beat); push(@sequence,".........*......");
  }
 $beat = $tempo / 8.;
 for ($i=0;$i<2;$i++) {
  push(@wait,0); push(@sequence,"b 8");
  push(@wait,$beat); push(@sequence,"....*.....*.....");
  push(@wait,$beat); push(@sequence,".....*....*.....");
  push(@wait,$beat); push(@sequence,"......*....*....");
  push(@wait,$beat); push(@sequence,".......*...*....");
  push(@wait,$beat); push(@sequence,"........*...*...");
  push(@wait,$beat); push(@sequence,".........*...*..");
  push(@wait,$beat); push(@sequence,"..........*...*.");
  push(@wait,$beat); push(@sequence,"....*..........*");
  
  push(@wait,0); push(@sequence,"B 8");
  push(@wait,$beat); push(@sequence,".....*........*.");
  push(@wait,$beat); push(@sequence,"......*......*..");
  push(@wait,$beat); push(@sequence,".......*....*...");
  push(@wait,$beat); push(@sequence,"........*...*...");
  push(@wait,$beat); push(@sequence,".........*.*....");
  push(@wait,$beat); push(@sequence,".......**..*....");
  push(@wait,$beat); push(@sequence,"......*..**.....");
  push(@wait,$beat); push(@sequence,".....*..*..*....");
  }
 } 
for ($j=0;$j<1;$j++) {
  $beat = $tempo / 12.;
 for ($i=0;$i<2;$i++) {
  push(@wait,0); push(@sequence,"A 12");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  }
 $beat = $tempo / 16.;
 for ($i=0;$i<2;$i++) {
  push(@wait,0); push(@sequence,"B 16");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  }
  $beat = $tempo / 12.;
 for ($i=0;$i<1;$i++) {
  push(@wait,0); push(@sequence,"A 12");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  push(@wait,$beat); push(@sequence,"....******......");
  }
 $beat = $tempo / 16.;
 for ($i=0;$i<1;$i++) {
  push(@wait,0); push(@sequence,"B 16");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  push(@wait,$beat); push(@sequence,"..........******");
  }
  $beat = $tempo / 24.;
  for ($i=0;$i<1;$i++) {
  push(@wait,0); push(@sequence,"C 24");
  
  push(@wait,$beat); push(@sequence,"....****..*.....");
  push(@wait,$beat); push(@sequence,"....**..**.*....");
  push(@wait,$beat); push(@sequence,"......****..*...");
  push(@wait,$beat); push(@sequence,"....**.......*..");
  push(@wait,$beat); push(@sequence,"......**......*.");
  push(@wait,$beat); push(@sequence,"........**.....*");
  push(@wait,$beat); push(@sequence,"....****..*.....");
  push(@wait,$beat); push(@sequence,"....**..**.*....");
  push(@wait,$beat); push(@sequence,"......****..*...");
  push(@wait,$beat); push(@sequence,"....**.......*..");
  push(@wait,$beat); push(@sequence,"......**......*.");
  push(@wait,$beat); push(@sequence,"........**.....*");
  
  push(@wait,$beat); push(@sequence,"....****..*.....");
  push(@wait,$beat); push(@sequence,"....**..**.*....");
  push(@wait,$beat); push(@sequence,"......****..*...");
  push(@wait,$beat); push(@sequence,"....**.......*..");
  push(@wait,$beat); push(@sequence,"......**......*.");
  push(@wait,$beat); push(@sequence,"........**.....*");
  push(@wait,$beat); push(@sequence,"....****..*.....");
  push(@wait,$beat); push(@sequence,"....**..**.*....");
  push(@wait,$beat); push(@sequence,"......****..*...");
  push(@wait,$beat); push(@sequence,"....**.......*..");
  push(@wait,$beat); push(@sequence,"......**......*.");
  push(@wait,$beat); push(@sequence,"........**.....*");
  }  

  for ($i=0;$i<1;$i++) {
  push(@wait,0); push(@sequence,"D 24");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  push(@wait,$beat); push(@sequence,"....************");
  }
 } 
  
  
<<<<<<< lights.pl
&loop();
}






sub firstChar {
if (($foo ge '1')&&($foo le '9')) {
  &set_the_measure($foo);
  return;
  }
$line = "";
$InputMode = "onALine";
&onALine();
}

sub onALine {
if (($foo eq chr(13))||($foo eq chr(10))) {
  print STDERR "\r\n";
  &process_line_command($line);
  $InputMode = "firstChar";
  return;
  }
if ($foo eq chr(8)) {
  print STDERR chr(8);
  $line = substr $line,1,strlen($line)-1;
  return;
  }
print STDERR $foo;
$line .= $foo;
}


sub process_line_command {
}

sub handle_input {
if ($foo eq chr(3)) {
  ReadMode 0;
  exit(-1);
  }
eval("&" . $InputMode . "();");
}

sub init_synchronization {
$InputMode = "firstChar";
ReadMode 5;

$selecktors = IO::Select->new();
$selecktors->add(\*STDIN);

$socket = IO::Socket::INET->new( Proto => 'udp',
  LocalPort => 12937
 ) || die "Unable to create socket: $@\r\n";
 
$selecktors->add($socket);
}


sub finish_loop_nothing() {
if ($onset==1) {
  print STDERR "#";
  }
else {
  print STDERR ".";
  }
{
  if ($jjj==$end) {
    &set_sequence();
    }
##  for ($jjj=$start;$jjj!= $end;$jjj+=$increment) {
  if ($wait[$jjj]!=0) {        
    $seq = $sequence[$jjj];
    if (($bwd==1)||($bwd==3)) {
      $seq = substr($seq,0,3) . substr($seq,10,6) . substr($seq,4,6);
      }
    for ($k=0;$k<16;$k++) {
      if (substr($sequence[$jjj],$k,1) eq "*") {
        $next_state[$k] = 1;
        }
      else {
        $next_state[$k] = 0;
        }
      }
	
    &sync(@next_state);
    ($sec2,$msec2) = gettimeofday();
    $jjj += $increment;
    }
  else {
    $jjj += $increment;
    &finish_loop_nothing();
    return;
    }
  }
($sec2,$msec2) = gettimeofday();
$delta = ($sec2-$sec) + ($msec2-$msec)*0.000001;
$loopTiming=($wait[$jjj]*0.000001 - $delta);
if ($onset==1) { # make this go cool 
  $loopTiming=10;
  }
#print STDERR "waiting $jjj: " . $wait[$jjj]*0.000001;
}



sub finish_loop_sequencing() {
if ($sequence_style==0) {
  for ($c=0;$c<$width;$c++) {  
    for ($r=0;$r<$height;$r++) { 
      $grid[$r*$width+$c]=0; 
      }
    }
  for ($r=0;$r<$height;$r++) { 
    $grid[$r*$width+$sequence]=1; 
    }
  $sequence = ($sequence+1) % $width;
  if ($sequence==0) {
    $sequence_style = ($sequence_style+1)%2;
    }
  }
elsif ($sequence_style==1) {
  for ($c=0;$c<$width;$c++) {  
    for ($r=0;$r<$height;$r++) { 
      $grid[$r*$width+$c]=0; 
      }
    }
  for ($c=0;$c<$width;$c++) {  
    $grid[$sequence*$width+$c]=1; 
    }
  $sequence = ($sequence+1) % $height;
  if ($sequence==0) {
    $sequence_style = ($sequence_style+1)%2;
    }
  }
print STDERR $sequence;
&finish_loop_nothing();
}


sub set_sequence {
&loop();
}






sub firstChar {
if (($foo ge '1')&&($foo le '9')) {
  &set_the_measure($foo);
  return;
  }
$line = "";
$InputMode = "onALine";
&onALine();
}

sub onALine {
if (($foo eq chr(13))||($foo eq chr(10))) {
  print STDERR "\r\n";
  &process_line_command($line);
  $InputMode = "firstChar";
  return;
  }
if ($foo eq chr(8)) {
  print STDERR chr(8);
  $line = substr $line,1,strlen($line)-1;
  return;
  }
print STDERR $foo;
$line .= $foo;
}


sub process_line_command {
}

sub handle_input {
if ($foo eq chr(3)) {
  ReadMode 0;
  exit(-1);
  }
eval("&" . $InputMode . "();");
}

sub init_synchronization {
$InputMode = "firstChar";
ReadMode 5;

$selecktors = IO::Select->new();
$selecktors->add(\*STDIN);

$socket = IO::Socket::INET->new( Proto => 'udp',
  LocalPort => 12937
 ) || die "Unable to create socket: $@\r\n";
 
$selecktors->add($socket);
}


sub finish_loop_nothing() {
if ($onset==1) {
  print STDERR "#";
  }
else {
  print STDERR ".";
  }
{
  if ($jjj==$end) {
    &set_sequence();
    }
##  for ($jjj=$start;$jjj!= $end;$jjj+=$increment) {
  if ($wait[$jjj]!=0) {        
    $seq = $sequence[$jjj];
    if (($bwd==1)||($bwd==3)) {
      $seq = substr($seq,0,3) . substr($seq,10,6) . substr($seq,4,6);
      }
    for ($k=0;$k<16;$k++) {
      if (substr($sequence[$jjj],$k,1) eq "*") {
        $next_state[$k] = 1;
        }
      else {
        $next_state[$k] = 0;
        }
      }
	
    &sync(@next_state);
    ($sec2,$msec2) = gettimeofday();
    $jjj += $increment;
    }
  else {
    $jjj += $increment;
    &finish_loop_nothing();
    return;
    }
  }
($sec2,$msec2) = gettimeofday();
$delta = ($sec2-$sec) + ($msec2-$msec)*0.000001;
$loopTiming=($wait[$jjj]*0.000001 - $delta);
#print STDERR "waiting $jjj: " . $wait[$jjj]*0.000001;
}



sub finish_loop_sequencing() {
if ($sequence_style==0) {
  for ($c=0;$c<$width;$c++) {  
    for ($r=0;$r<$height;$r++) { 
      $grid[$r*$width+$c]=0; 
      }
    }
  for ($r=0;$r<$height;$r++) { 
    $grid[$r*$width+$sequence]=1; 
    }
  $sequence = ($sequence+1) % $width;
  if ($sequence==0) {
    $sequence_style = ($sequence_style+1)%2;
    }
  }
elsif ($sequence_style==1) {
  for ($c=0;$c<$width;$c++) {  
    for ($r=0;$r<$height;$r++) { 
      $grid[$r*$width+$c]=0; 
      }
    }
  for ($c=0;$c<$width;$c++) {  
    $grid[$sequence*$width+$c]=1; 
    }
  $sequence = ($sequence+1) % $height;
  if ($sequence==0) {
    $sequence_style = ($sequence_style+1)%2;
    }
  }
print STDERR $sequence;
&finish_loop_nothing();
}


sub set_sequence {
>>>>>>> 1.5
  $bwd = int(rand(2));
  if (($bwd==0)||($bwd==2)) {
    $start = 0;
    $end = @sequence;
    $increment = 1;
    }
  else {
    $start = @sequence-1;
    $end = -1;
    $increment = -1;
    }
$jjj = $start;
}





sub loop {
$loopMode="nothing";
$loopTiming=0;
&set_sequence();
($sec2,$msec2) = gettimeofday();
do {
  $sec = $sec2;
  $msec = $msec2;
  $onset = 0; 
#  print STDERR "\r\n$loopTiming ";
  if ($loopTiming<0) {$loopTiming=0;}
  @ready = $selecktors->can_read($loopTiming);
  foreach $fh (@ready) {
    if($fh == $socket) { 
      my $data;
      my $ip = $socket->recv( $data, 8000);
      my @datab = split //, substr $data,0,24;
      my $i;
      $onset = 0; 
      for ($i=0;($i<@datab) && ($i<3*8);$i+=8) {
        $mstime = ord($datab[$i]) + (ord($datab[$i+1])<<8)
+      (ord($datab[$i+2])<<16) + (ord($datab[$i+3])<<24);

        $word = ord($datab[$i+4]) + (ord($datab[$i+5])<<8)
+      (ord($datab[$i+6])<<16) + (ord($datab[$i+7])<<24);
   
    
        my $sign = ($word & 0x80000000) ? -1 : 1;
        my $expo = (($word & 0x7F800000) >> 23) - 127;
        my $mant = ($word & 0x007FFFFF | 0x00800000);
        my $float = $sign * (2 ** $expo) * ( $mant / (1 << 23));
        if ($mstime>=1048576000) {
          if ($i==0) {
	    $onset=1;
	    }
          $mstime=$mstime-1048576000;
#          if ($i) { print STDERR "	";}
#          print STDERR "$mstime $float";
          }
        push(@onsets2,$mstime); 
        push(@strengths2,$float);
        }
      if ($onset != 0) {
        if ($status_thread{"isOnset"}==0) {
          $status_thread{"isOnset"}=$onset;
          }
        }
      $status_thread{"onset0"} = $onsets2[0]; 
      $status_thread{"strength0"} = $strengths2[0];
      } # if we are the socket
    else {
#      chomp($foo = <STDIN>);
      $foo = ReadKey(0);
      &handle_input($foo);
      }
    } # for each selecktor
#   if it is time to talk to the output
  ($sec2,$msec2) = gettimeofday();
  my $delta = ($sec2-$sec) + ($msec2-$msec)*0.000001;
  if (($loopTiming<=$d)    ||($onset==1))  
    {
    eval("&finish_loop_" . $loopMode . "();");
    }
  else {
    $loopTiming = $loopTiming -$delta;
    }
  } while (1); 
}


&fornow();





