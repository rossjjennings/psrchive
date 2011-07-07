#! /bin/csh

if ( "$1" == "" ) then
  echo 'USAGE: upload.csh <USER>'
  echo 'where <USER> is your sourceforge user name'
  exit -1
endif

set SFUSER = $1
set SFPATH = ${SFUSER},psrchive@web.sourceforge.net:htdocs/classes

set SFHTML = "Util MEAL psrchive"

echo "Installing $SFHTML as $SFUSER"

cd html

rsync -avz $SFHTML $SFPATH/

