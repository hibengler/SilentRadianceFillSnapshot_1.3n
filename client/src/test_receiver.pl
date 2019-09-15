#!/usr/bin/perl
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

use IO::Socket::INET;
use IO::Select;
use Term::ReadKey;

use Time::HiRes qw( usleep ualarm gettimeofday tv_interval nanosleep
clock_gettime clock_getres clock_nanosleep clock );

$InputMode = "firstChar";

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
  &process_line_command($foo);
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


sub handle_input {
if ($foo eq chr(3)) {
  ReadMode 0;
  exit(-1);
  }
eval("&" . $InputMode . "();");
}


ReadMode 5;

$selecktors = IO::Select->new();
$selecktors->add(\*STDIN);

$socket = IO::Socket::INET->new( Proto => 'udp',
  LocalPort => 12937
 ) || die "Unable to create socket: $@\n";
 
$selecktors->add($socket);

do {
  @ready = $selecktors->can_read(10000);
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
	    print STDERR ".";
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
  } while (1); 
 
 
