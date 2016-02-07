#!/bin/csh -f

if ( $1 == "" ) then

cat << EOD

  pks_rcvr.csh - create a simple log of receiver changes as a function of epoch
  
  example usage:

  cd /nfs/online/logs
  pks_rcvr.csh <dates>.tcs_ctrl.log[.gz]

EOD

endif

foreach log ( $argv )

  set cat_cmd = "cat"
  if ( $log =~ *.gz ) set cat_cmd = "zcat"

  set utc_date = `echo $log | awk -F. '{print $1}'`

  $cat_cmd $log | grep -F '(Usr) receiver' | uniq -f 3 | awk '$4 != "???" {print "'$utc_date'-"$1, $4}' 

end

