dnl @synopsis SWIN_LIB_EPSIC
dnl 
AC_DEFUN([SWIN_LIB_EPSIC],
[
  AC_PROVIDE([SWIN_LIB_EPSIC])

  SWIN_PACKAGE_OPTIONS([epsic])

  AC_MSG_CHECKING([for EPSIC library])

  if test "$have_epsic" != "user disabled"; then

    AC_LANG_PUSH(C++)

    SWIN_PACKAGE_FIND([epsic_inc],[mode.h])
    SWIN_PACKAGE_TRY_COMPILE([epsic_inc],[#include "mode.h"])

    SWIN_PACKAGE_FIND([epsic_lib],[libepsic.*])
    SWIN_PACKAGE_TRY_LINK([epsic_lib],[#include "mode.h"],
                          [mode a ();],[-lepsic])

    AC_LANG_POP(C++)

  else
    have_epsic=no
  fi

  AC_MSG_RESULT([$have_epsic_lib])

  if test $have_epsic_lib = yes; then
    AC_DEFINE(HAVE_EPSIC,1,[Define if the Eigen library is installed])
  fi

  EPSIC_LIBS="$epsic_lib_LIBS"
  EPSIC_CFLAGS="$epsic_inc_CFLAGS"

  AC_SUBST(EPSIC_LIBS)
  AC_SUBST(EPSIC_CFLAGS)

  AM_CONDITIONAL(HAVE_EPSIC,[test "$have_epsic" = yes])

])

