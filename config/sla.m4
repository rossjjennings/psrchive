# SWIN_LIB_CSLA([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_SLA],
[
  AC_PROVIDE([SWIN_LIB_SLA])
  AC_REQUIRE([AC_F77_LIBRARY_LDFLAGS])

  AC_MSG_CHECKING([for SLA installation])

  SLA_CFLAGS="-I/usr/local/src/include"
  SLA_LIBS="-lsla"

  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  LIBS="$ac_save_LIBS $SLA_LIBS"
  CFLAGS="$ac_save_CFLAGS $SLA_CFLAGS"

  AC_TRY_LINK([#include<slalib.h>;],
              [double gmst = slaGmst(0);],
              [have_sla="yes: C"], [have_sla=no])
  
  if test "$have_sla" = no; then
    SLA_CFLAGS=""
    SLA_LIBS="-lsla $FLIBS"
    LIBS="$ac_save_LIBS $SLA_LIBS"

    sla_underscore=""
    sla_def=""

    sla_trial_list="sla_gmst sla_gmst_ sla_gmst__"
    for sla_trial in $sla_trial_list; do
      AC_TRY_LINK([double $sla_trial(double *);],
                  [double gmst = $sla_trial(0);],
                  [have_sla="yes: fortran"$sla_underscore], [have_sla=no])
      if test "$have_sla" != no; then
        sla_def="name"
        if test x"$sla_underscore" != x; then
          sla_def="$sla_def ## $sla_underscore"
        fi
        break
      fi
      sla_underscore="$sla_underscore"_
    done

  fi

  LIBS="$ac_save_LIBS"
  CFLAGS="$ac_save_CFLAGS"

  AC_MSG_RESULT([$have_sla])

  if test "$have_sla" != no; then
    AC_DEFINE([HAVE_SLA], [1], [Define to 1 if you have the SLA library])
    AH_TEMPLATE([SLA_FUNC], [Fortran name mangling in libsla])
    AC_DEFINE_UNQUOTED([SLA_FUNC(name,NAME)],$sla_def)
    [$1]
  else
    SLA_CFLAGS=""
    SLA_LIBS=""
    [$2]
  fi

  AC_SUBST(SLA_CFLAGS)
  AC_SUBST(SLA_LIBS)

  AM_CONDITIONAL(HAVE_SLA_FORTRAN, [test x"$sla_def" != x])

])
