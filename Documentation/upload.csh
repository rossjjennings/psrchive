#! /bin/csh

set SFUSER = `awk -F@ '{print $1}' CVS/Root`
set SFPATH = /home/groups/p/ps/psrchive/htdocs/classes

echo "Installing html/* as $SFUSER"

echo "Creating gzipped tarball ..."
cd html
tar cf doc.tar *
gzip -f doc.tar

echo "Secure copying to shell.sourceforge.net ..."

scp doc.tar.gz $SFUSER@shell.sourceforge.net:
ssh $SFUSER@shell.sourceforge.net "cd $SFPATH && rm -rf * && tar zxvf ~/doc.tar.gz"

