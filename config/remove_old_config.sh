#!/bin/sh
# remove_old_config.sh

OLD_CONFIG=""

for dir in "Util/meal" "Util/epsic" "Util/epsic/src" "."; do
    if [ -f "$dir/configure" ]; then
        OLD_CONFIG="$OLD_CONFIG $dir/configure"
    fi
    if [ -d "$dir/autom4te.cache" ]; then
        OLD_CONFIG="$OLD_CONFIG $dir/autom4te.cache"
    fi
done

rm -rf $OLD_CONFIG Util/epsic/src/config

exit 0

