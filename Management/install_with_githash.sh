#!/bin/sh
# This needs to be called with exactly two arguments, program name
# and dir to install to.  Should be generalized to work when
# installing multiple things, but this is ok for the current
# set of things installed this way.
if [ $# -ne 2 ]
then
	echo "Usage: $0 SOURCE DEST"
	exit 1
fi
ID=`git log -1 --format='(%cd %h)' --date=short 2>/dev/null || echo ''`
cat $1 | sed "s/PSRCHIVEGITHASH/$ID/" > $2/$1
