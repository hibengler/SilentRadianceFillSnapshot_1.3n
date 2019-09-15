#/usr/bin/perl
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
#
# cpan
# install Digest::SHA

$who=$ARGV[1];
$event=$ARGV[2];
$dj=$ARGV[3];


use Digest::SHA qw(sha256_hex);


$date = `date -u +"%F %T"`;
chomp($date);

$cred = $ENV{"CRED"};
if (!defined($cred)) {
  $cred = `echo ~/cred`;
  chomp($cred);
  }




open($in, "-|", "arp -a -n -i wlan0 | awk -F '(' '{print \$2}'| awk -F ')' '{print \$1}'")
  or die " cant get arp listing";

while (my $ip = <$in>) {
  chomp($ip);
  $credline = "R|$who|$event|$date|$ip|";
  print STDOUT "$credline\n";
  $entry = sha256_hex($data . "\n");
  $e1 = substr($entry,0,2);
  $e2 = substr($entry,2,2);
#  print STDOUT "mkdir 2>/dev/null $cred/D$e1 $cred/D$e1/$e2";
  system("mkdir 2>/dev/null $cred/D$e1 $cred/D$e1/$e2");
  my $filename = "$cred/D$e1/$e2/$entry";
  
  open(my $fh, '>', "$filename.txt") or die "Could not open file '$filename.txt' $!";
  print $fh "$credline\n";
  close($fh);

  $gpg = `echo \"$credline\" | gpg --detach-sig --clearsign`;
  open(my $fh, '>', "$filename.sig") or die "Could not open file '$filename.txt' $!";
  print $fh "$gpg";
  close($fh);

  $entry2=sha256_hex($gpg);
  $e1 = substr($entry2,0,2);
  $e2 = substr($entry2,2,2);
#  print STDOUT "mkdir 2>/dev/null $cred/D$e1 $cred/D$e1/$e2";
  system("mkdir 2>/dev/null $cred/D$e1 $cred/D$e1/$e2");
  my $filename = "$cred/D$e1/$e2/$entry2";
  open(my $fh, '>', "$filename.ref") or die "Could not open file '$filename.txt' $!";
  print $fh "$entry|$credline\n";
  close($fh);

  $command = "./sr_command '$credline$entry2'";
  print STDOUT "$command\n";
  system($command);  
  system($command);  
  system($command);  
  system($command);  
  system($command);  
  }

close($in);
