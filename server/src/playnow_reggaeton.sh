#/bin/bash
# $Revision: 1.1 $
# from 1.16 of playnow.sh
# this variation does more of the 'hits' so that it is more likely to hear one of the good songs
# compared to the wheat songs.  However, every song gets its day still
# later we will make a dont_do_duplicates_within_10 filter
# to keep this from duplicating songs next to eachother.
echo "Playing for $SR_DOMAIN_SERVER"
echo "playnow_reggaeton.sh directory $PWD"
echo "best to do redo_all_songs_dat $SR_DOMAIN_SERVER and"
echo "redo_playlist_log $SR_DOMAIN_SERVER &    "
echo ""

export SR_BIN="${SR_BIN:-/usr/local/bin}"

while :
do
{
# echo "making a randomized playlist"
 rm -f 2>/dev/null /tmp/y$$
 {
 find Prime_Records -follow -name '*.wav'
 find Prime_Records -follow -name '*.flac'
 find Prime_Records -follow -name '*.shnf'
 find Prime_Records -follow -name '*.ogg'
 find Prime_Records -follow -name '*.mp3' 
 find Prime_Records -follow -name '*.m4a' 
 } >/tmp/y$$
 
 cat /tmp/y$$
 cat /tmp/y$$
 cat /tmp/y$$
 cat /tmp/y$$
 cat /tmp/y$$
 cat /tmp/y$$
 
 rm -f 2>/dev/null /tmp/y$$

 find Reggaeton_Rap -follow -name '*.wav'
 find Reggaeton_Rap -follow -name '*.flac'
 find Reggaeton_Rap -follow -name '*.shnf'
 find Reggaeton_Rap -follow -name '*.ogg'
 find Reggaeton_Rap -follow -name '*.mp3' 
 find Reggaeton_Rap -follow -name '*.m4a'
  
 find Reggae_Rap -follow -name '*.wav'
 find Reggae_Rap -follow -name '*.flac'
 find Reggae_Rap -follow -name '*.shnf'
 find Reggae_Rap -follow -name '*.ogg'
 find Reggae_Rap -follow -name '*.mp3' 
 find Reggae_Rap -follow -name '*.m4a' 
 
 find Tropical_Reggaeton -follow -name '*.wav'
 find Tropical_Reggaeton -follow -name '*.flac'
 find Tropical_Reggaeton -follow -name '*.shnf'
 find Tropical_Reggaeton -follow -name '*.ogg'
 find Tropical_Reggaeton -follow -name '*.mp3' 
 find Tropical_Reggaeton -follow -name '*.m4a' 
} | randomize  `date +%m%s` | sed s/\'/\'\\\\\'\'/g |
  awk '{print "srplayer '\''" $0 "'\''" }' >playlist.txt
  if [ -s playlist.txt ] 
  then
    cp playlist.txt playlist_`date +%Y%m%d%H%M%s`.ptxt
    ~/src/one_step_ahead playlist.txt 2>>playlist.log
  else
    echo "empty playlist"
    echo "  probably should restart the mounted filesystem, if there is one"
    echo "  sleeping for one minute...."
    sleep 60
    echo "  wakeup now"
    echo ""
  fi
done

# $Log: playnow_reggaeton.sh,v $
# Revision 1.1  2019/09/11 04:47:34  hib
# Made a reggaeton playnow variant that plays the hits once every 5 songs on a playlist
#
# Revision 1.16  2019/09/01 00:25:11  hib
# fix for guess info
#
# Revision 1.15  2019/06/01 22:21:41  hib
# reverse login
#
# Revision 1.14  2019/06/01 22:19:40  hib
# wip
#
# Revision 1.13  2019/06/01 22:10:52  hib
# made playlist.sh slow down if the playlist is empty.
#
# Revision 1.12  2019/05/17 23:23:36  hib
# Adding support for a new song provider: Kim. Also made it print out the server name so we don't get confused.
#
# Revision 1.11  2018/12/12 13:43:23  hib
# wip - cleaning up radio auto play
#
# Revision 1.10  2018/12/01 16:27:01  hib
# fixing radio servers to show song name
#
# Revision 1.9  2018/10/31 02:57:23  hib
# k
#
# Revision 1.8  2018/10/31 02:48:27  hib
# Set up sr000 for full and incremental logging
#
# Revision 1.7  2018/07/22 00:10:28  hib
# cleanup on money server
#
# Revision 1.6  2018/03/29 00:48:13  hib
# Getting the greatful dead tapes set up
#
# Revision 1.5  2018/03/17 08:17:58  hib
# playnow - minor.
# upload_to_server - got this to upload only one file at a time. It will start the next file
# early so that it is there in time.
#
