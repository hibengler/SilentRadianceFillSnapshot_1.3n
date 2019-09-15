#!/bin/bash
# usage: cd .../client/src
# find . -name '*.[ch]' | awk '{print "bash make_c_code_international.sh " $1;}' | bash
# well, in linux:
# find . -name '*.[ch]' | awk '{print "bash make_c_code_international.sh " $1;}' | grep -v ./libbpg | grep -v ./libpng |    grep -v ./display/external  grep -v impact.c | bash
#
# android:
# cat local_files.txt  | awk '{print "bash make_c_code_international.sh " $1;}'   | bash

echo 1>&2 " processing $1"
echo >>lang.log "// processing $1"
#echo 1>&2 "// snobol4 -P10M make_c_code_international.sbl <$1 >t$$ 2>>lang.log -u $1"
snobol4 -P10M make_c_code_international.sbl <$1 >t$$ 2>>lang.log -u $1
mv $1 "$1"O
mv t$$ $1

# display/core.c freeze

