#!/usr/bin/perl 
#use strict;
#use warnings;
$|= 1;
use LWP;
use HTTP::Request::Common;
$HTTP::Request::Common::DYNAMIC_FILE_UPLOAD = 1;

$totBytes = 0;
my $ua = LWP::UserAgent->new;

my $server        = 'soap';
my $name = $ARGV[0];
$name = "0000000000000000" unless defined($name);

my $bytesize =1024*1024*100 + 2048;

binmode(STDIN);

my $fileheader = HTTP::Headers->new;
$fileheader->header('Content-Type' => 'application/sirad',
                      'Content-Length' => $bytesize,);
                                   



#    fn => [$file],

my $req = POST "http://$server/sirad/upload.pl",
  Content_Type => 'form-data',
  Content => [
          'filename' => "$name",
          'button' => '1',
	  'upfile' => [ "/dev/null" ],
	   ];
#



print $req->as_string();

$callback = $req->content;
print STDERR "callback is $callback\n";
#my $size = $req->header('content-length');
#$bytesize = $size;
$req->content(\&wrapper);
print STDERR "about to request\n";

#print $req->as_string();

my $resp = $ua->request($req);


my $state = 0;

sub wrapper2 {
  my $chunk = &$callback(); 
  print STDERR "Chunk $chunk\n\n";
  $state= 1;
  return($chunk);
}

sub wrapper {
if ($state==0) {
  my $chunk = &$callback(); 
  print STDERR "Chunk $chunk\n";
  $state= 1;
  return($chunk);
  }
elsif ($state==1) {
  $state = 2;
  return "Content-Disposition: form-data; name=\"upfile\"; filename=\"a.txt\"
Content-Type: Application/sirad\n\n";
  }
elsif ($state==2) {
  if ($totBytes<=$bytesize) {
#  	  		   print STDERR ".";
			   my $buffer;
			   my $nBytes = read(STDIN, $buffer, 1024);
			   if ($nBytes==0) {$state = 4;return;}
                           $totBytes += $nBytes;
                           if ($totBytes>=$bytesize) {
			     print STDERR "done... $totBytes\n";
                             $state = 3;
                             return $buffer;
			     }
                           return $buffer;
                           }
                         $state = 9;
                         return;
  }
elsif ($state==3) {
  my $chunk = &$callback(); 
  print STDERR "Chdunk $chunk\n";
  $state = 4;
  return $chunk;
  }
elsif ($state==4) {
  my $chunk = &$callback(); 
  print STDERR "Chddunk $chunk\n";
  $state= 4;
  return $chunk;
  }
}

