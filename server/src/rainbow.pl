use POSIX;

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

  for ($h=0.;$h<=1.;$h += 1./256.) {
    my ($r,$g,$b) = &hsv2rgb($h,1.,1.);
    $s = sprintf("./sr_command C%2.2x%2.2x%2.2x",floor($r*255),floor($g*255),floor($b*255));
    system $s;
    }
system "./sr_command C000000";
system "./sr_command C000000";
system "./sr_command C000000";

sub hsv2rgb {
my ($H,$S,$V) = @_;
$H *= 360;   #adapt for 0-1 hue input
if ($H==360.) {
  $H = 0;
  }
else {
  $H = $H / 60.;
  }
$fract = $H - floor($H);
$P = $V*(1. - $S);
    $Q = $V*(1. - $S*$fract);
    $T = $V*(1. - $S*(1. - $fract));

    if     ((0. <= $H) && ($H < 1.)) {
        return($V,$T,$P);
        }
    elsif ((1. <= $H) && ($H < 2.)) {
        return($Q,$V,$P);
        }
    elsif ((2. <= $H) && ($H < 3.)) {
        return($P,$V,$T);
        }
    elsif ((3. <= $H) && ($H < 4.)) {
        return($P,$Q,$V);
        }
    elsif ((4. <= $H) && ($H < 5.)) {
        return($T,$P,$V);
        }
    elsif ((5. <= $H) && ($H < 6.)) {
        return($V,$P,$Q);
        }
    else {
        return(0.,0.,0.);
      }
}


