#! /bin/csh

set SFUSER = `awk -F@ '{print $1}' CVS/Root`
set SFPATH = shell.sourceforge.net:/home/groups/p/ps/psrchive/htdocs/classes

set SFHTML = "Util MEAL psrchive"

echo "Installing $SFHTML as $SFUSER"

cd html

rsync -Crvz --rsh="ssh -l $SFUSER" $SFHTML $SFPATH/

