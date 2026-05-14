#!/bin/sh
# check_submodules.sh <path_to_src>

path_to_src=.

if test "x$1" != "x"; then
  path_to_src=$1
fi

MISSING=""

for dir in "Util/meal" "Util/epsic"; do
    if [ ! -f "$path_to_src/$dir/configure.ac" ]; then
        MISSING="$MISSING $dir"
    fi
done

if [ -n "$MISSING" ]; then
    echo
    echo "ERROR: The following submodules are missing or empty:"
    echo
    echo "$MISSING"
    echo
    echo "Please run:"
    echo
    if test "$path_to_src" != "."; then
	    echo "cd $path_to_src"
    fi
    echo "git submodule update --init --recursive"
    echo
    exit 1
fi

exit 0

