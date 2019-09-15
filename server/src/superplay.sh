ls trance/*.mp3 | randomize `date +%m%s` | head -12000 | sed s/\'/\'\\\\\'\'/g |
  awk 'BEGIN {print "mplayer  \\"} 
  {print "'\''" $0 "'\'' \\" }
  END {print ""}' >/tmp/l$$
bash /tmp/l$$
