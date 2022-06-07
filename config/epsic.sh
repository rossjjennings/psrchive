#! /bin/sh

dir=.

if test "x$1" != "x"; then
  dir=$1
fi

owd=$PWD

if test ! -d $dir/Util/epsic/src; then
  echo Updating EPSIC submodule
  cd $dir
  git submodule update --init --recursive
  cd $owd
fi

if test ! -f $dir/Util/epsic/src/configure; then
  echo Running EPSIC bootstrap script
  cd $dir/Util/epsic/src
  ./bootstrap
  cd $owd
fi

