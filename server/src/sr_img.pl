use File::Basename;
if ((@ARGV)<4) {
  print STDERR "usage - perl sr_img.pl <filename> <fileid> <upc> <count>\n";
  exit(-1);
  }
$filename = $ARGV[0];
$file_id = $ARGV[1];
$upc = $ARGV[2];
$count = $ARGV[3];

($name,$path,$suffix) = fileparse($filename,'\.[^\.]*');

$id = `identify $filename`;
# print "id is $id\n";

# sss sss 2560x1440
if ($id =~ /(\d+)x(\d+)/) {
  $width=$1;
  $height=$2;
  }
$max = $width;
if ($height >$max) {$max=$height;}
$pixsize=64;
while ($pixsize<$max) {
  $pixsize = $pixsize + $pixsize;
  if ($pixsize==2048) {last;}
  }

$temp = $$;

#print "$name $path $suffix $width $height $pixsize\n";
print "rm -rf 2>/dev/null /tmp/cv$temp\n";
print "mkdir /tmp/cv$temp\n";
print "export PATHO=\$PATH\nexport PATH=\$PWD:\$PATH\n";
print "convert \"$filename\" -resize " . $pixsize . "x" . $pixsize . "! /tmp/cv$temp/\"$name.png\"\n";
print "cd /tmp/cv$temp\n";
print "bpgenc -o \"$name.bpg\" \"$name.png\"\n";
print "sr_send_binary I $file_id \"$name.bpg\" \"$upc\" $width $height $count\n";
print "cd -\n";
print "export PATH=\$PATHO\n";
print "#rm -rf  2>/dev/null /tmp/cv$temp\n";
