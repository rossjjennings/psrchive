dnl @synopsis SWIN_LIB_FFTW
dnl 
AC_DEFUN([SWIN_LIB_FFTW],
[
  AC_PROVIDE([SWIN_LIB_FFTW])
  AC_MSG_CHECKING([for FFTW installation])

  ac_save_LIBS="$LIBS"       
  LIBS="-lfftw $ac_save_LIBS"
  AC_TRY_LINK([#include <fftw.h>],
              [fftw_create_plan (1024, FFTW_FORWARD, FFTW_ESTIMATE);],
              have_fftw=yes, have_fftw=no)

  have_fftw_msg="no"
  FFTW_LIBS=""

  if test x"$have_fftw" = xyes; then
    AC_DEFINE(HAVE_FFTW,1,[Define if the FFTW library is installed])
    FFTW_LIBS="-lfftw"
    have_fftw_msg="fftw"
  fi

  LIBS="-lrfftw -lfftw $ac_save_LIBS"
  AC_TRY_LINK([#include <fftw.h>],
              [rfftw_create_plan (1024, FFTW_FORWARD, FFTW_ESTIMATE);],
              have_rfftw=yes, have_rfftw=no)

  if test x"$have_rfftw" = xyes; then
    AC_DEFINE(HAVE_RFFTW,1,[Define if the FFTW real library is installed])
    FFTW_LIBS="-lrfftw -lfftw"
    have_fftw_msg="$have_fftw_msg rfftw"
  fi

  LIBS="-lfftw3f $ac_save_LIBS"
  AC_TRY_LINK([#include <fftw3.h>],
              [fftwf_plan_dft_1d (1024, 0, 0, FFTW_FORWARD, FFTW_ESTIMATE);],
              have_fftw3=yes, have_fftw3=no)

  if test x"$have_fftw3" = xyes; then
    AC_DEFINE(HAVE_FFTW3,1,[Define if the FFTW3 library is installed])
    FFTW_LIBS="-lfftw3f $FFTW_LIBS"
    if test x"$have_fftw_msg" = xno; then
      have_fftw_msg="fftw3"
    else
      have_fftw_msg="$have_fftw_msg fftw3"
    fi
  fi

  AC_MSG_RESULT([$have_fftw_msg])
  AC_SUBST(FFTW_LIBS)
  LIBS="$ac_save_LIBS"

])

