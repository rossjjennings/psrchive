#!/bin/csh

set Msamp=1024

echo Simulating $Msamp Msamples ...

set resoff=`simpol -s 0.98,0,0,0 -n $Msamp $argv |& grep mean`
set reson=`simpol -s 1.02,0,0.04,0 -n $Msamp $argv |& grep mean`

set IUoff=`echo $resoff | sed -e 's|(|,|g' | awk -F, '{print $2, $4}'`
set IUon=`echo $reson | sed -e 's|(|,|g' | awk -F, '{print $2, $4}'`

echo $IUoff $IUon | awk '{printf ("fractional polarization = %lf\n", ($4-$2)/($3-$1));}'

