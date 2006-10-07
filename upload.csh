#! /bin/csh

set SFUSER = `awk -F@ '{print $1}' CVS/Root`

echo "Installing doc/html as $SFUSER"

cd doc/html
tar cvf doc.tar *
gzip -f doc.tar

scp doc.tar.gz $SFUSER@shell.sourceforge.net:
ssh $SFUSER@shell.sourceforge.net 'cd psrchive/htdocs/classes/psrchive && tar zxvf ~/doc.tar.gz'

