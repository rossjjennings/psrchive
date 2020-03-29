#
# PSR_LIB_SPLINTER
#
# This m4 macro checks availability of the SPLINTER (SPLine INTERpolation)
# library for multivariate function approximation with splines, written by
# Bjarne Grimstad and others.
#
# SPLINTER_CFLAGS - autoconfig variable with flags required for compiling
# SPLINTER_LIBS   - autoconfig variable with flags required for linking
# HAVE_SPLINTER   - automake conditional
# HAVE_SPLINTER   - pre-processor macro in config.h
#
# This macro tries to link a test program, using
#
#    -I$SPLINTER/include -L$SPLINTER/lib -ltempo2pred
#
# ----------------------------------------------------------
dnl @synopsis PSR_LIB_SPLINTER
dnl 
AC_DEFUN([PSR_LIB_SPLINTER],
[
  AC_PROVIDE([PSR_LIB_SPLINTER])

  SWIN_PACKAGE_OPTIONS([splinter])

  AC_MSG_CHECKING([for SPLINTER (SPLine INTERpolation) library.])

  if test "$have_splinter" != "user disabled"; then

    AC_LANG_PUSH(C++)

    SWIN_PACKAGE_FIND([splinter],[bspline.h])
    SWIN_PACKAGE_TRY_COMPILE([splinter],[#include "bspline.h"])

    SWIN_PACKAGE_FIND([splinter_lib],[libsplinter-3-0.*])
    SWIN_PACKAGE_TRY_LINK([splinter_lib],[#include "datatable.h"],
                          [SPLINTER::DataTable samples;],[-lsplinter-3-0])

    AC_LANG_POP(C++)

    have_splinter=$have_splinter_lib
  else
    have_splinter=no
  fi

  AC_MSG_RESULT([$have_splinter])

  if test $have_splinter = yes; then
    AC_DEFINE(HAVE_SPLINTER,1,[Define if the SPLINTER library is installed])
  fi

  SPLINTER_LIBS="$splinter_lib_LIBS"
  SPLINTER_CFLAGS="$splinter_inc_CFLAGS"

  AC_SUBST(SPLINTER_LIBS)
  AC_SUBST(SPLINTER_CFLAGS)

  AM_CONDITIONAL(HAVE_SPLINTER,[test "$have_splinter" = yes])

])

