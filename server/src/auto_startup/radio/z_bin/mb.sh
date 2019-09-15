#!/bin/bash
#
# $Revision: 1.2 $
#
export DISPLAY=:0
sleep 20
xterm -bg darkblue -fg white -e "/home/z/bin/mb2.sh || bash"
