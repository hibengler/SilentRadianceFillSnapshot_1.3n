echo id3info - use for mp3
export SR_BIN=~/src
{ echo moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3  ; id3info  moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3; }  |cat

echo "snobol says:"
{ echo moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3  ; id3info  moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3; }  |
   snobol4 ~/src/guess_info.sno

echo
echo
echo gettags use for other

{ echo moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3  ; gettags  moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3; }  | cat
echo "snobol says:"
{ echo moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3  ; gettags  moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3; }  | 
   snobol4 ~/src/guess_info.sno


echo stand alone - snobol used the file name
echo moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3   | cat
echo snobol says:
echo moby_free/moby001-Moby_-_Long_Ambients_1_Calm_Sleep/01\ LA1.mp3   | 
   snobol4 ~/src/guess_info.sno
