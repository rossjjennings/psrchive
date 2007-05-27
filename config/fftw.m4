dnl @synopsis SWIN_LIB_FFTW
dnl 
AC_DEFUN([SWIN_LIB_FFTW],
[
  AC_PROVIDE([SWIN_LIB_FFTW])

  SWIN_PACKAGE_OPTIONS([fftw])

  AC_MSG_CHECKING([for single-precision FFTW-2 library])

  if test "$have_fftw" != "user disabled"; then

    SWIN_PACKAGE_FIND([fftw],[fftw.h])
    SWIN_PACKAGE_TRY_COMPILE([fftw],[#include <fftw.h>],
                             [#ifndef FFTW_ENABLE_FLOAT
                              #error must have single-precision library
                              #endif])

    if test $have_fftw = yes; then
      SWIN_PACKAGE_FIND([fftw],[libfftw.*])
      SWIN_PACKAGE_TRY_LINK([fftw],[#include <fftw.h>],
                            [fftw_create_plan(64,FFTW_FORWARD,FFTW_ESTIMATE);],
                            [-lfftw])
    fi

  fi

  AC_MSG_RESULT([$have_fftw])

  if test $have_fftw = yes; then

    AC_DEFINE(HAVE_FFTW,1,[Define if the FFTW library is installed])

    FFTW_CFLAGS="$fftw_CFLAGS"
    FFTW_LIBS="$fftw_LIBS"

    AC_MSG_CHECKING([for FFTW-2 real-to-complex library])

    SWIN_PACKAGE_TRY_LINK([rfftw],[#include <fftw.h>],
                          [rfftw_create_plan(64,FFTW_FORWARD,FFTW_ESTIMATE);],
                          [-lrfftw -lfftw])

    AC_MSG_RESULT($have_rfftw)

    if test $have_rfftw = yes; then
      AC_DEFINE(HAVE_RFFTW,1,[Define if the FFTW real library is installed])
      FFTW_LIBS="$rfftw_LIBS"
    fi

  fi

  AC_MSG_CHECKING([for single-precision FFTW-3 library])

  if test "$have_fftw" != "user disabled"; then

    SWIN_PACKAGE_FIND([fftw3],[fftw3.h])
    SWIN_PACKAGE_TRY_COMPILE([fftw3],[#include <fftw3.h>])

    if test $have_fftw3 = yes; then
      SWIN_PACKAGE_FIND([fftw3],[libfftw3f.*])
      SWIN_PACKAGE_TRY_LINK([fftw3],[#include <fftw3.h>],
                            [fftwf_plan_dft_1d(0,0,0,FFTW_FORWARD,FFTW_ESTIMATE);],
                            [-lfftw3f])
    fi

  fi

  AC_MSG_RESULT([$have_fftw3])

  if test $have_fftw3 = yes; then
    AC_DEFINE(HAVE_FFTW3,1,[Define if the FFTW3 library is installed])
    FFTW_LIBS="$fftw3_LIBS $FFTW_LIBS"
    FFTW_CFLAGS="$fftw3_CFLAGS $FFTW_CFLAGS"
  fi

  AC_SUBST(FFTW_LIBS)
  AC_SUBST(FFTW_CFLAGS)
  LIBS="$ac_save_LIBS"

  AM_CONDITIONAL(HAVE_FFTW,[test "$have_fftw" = yes])
  AM_CONDITIONAL(HAVE_FFTW3,[test "$have_fftw3" = yes])

])

