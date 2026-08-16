#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status, 
# and treat unset variables as an error
set -euo pipefail 

mkdir -p /pulsar/src

for pkg in tempo2 psrcat splinter; do
  echo Installing $pkg
  sed -e 's|@prefix@|/pulsar|g' -e 's|@download@|curl -O -f -L|g' -e 's|@CC@|gcc|g' ${pkg}.sh.in > /tmp/${pkg}.sh
  chmod a+x /tmp/${pkg}.sh
  /tmp/${pkg}.sh -j4
  rm -f /tmp/${pkg}.sh
done

rm -rf /pulsar/src

