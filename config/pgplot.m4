# SWIN_LIB_CPGPLOT([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_PGPLOT],
[
  AC_PROVIDE([SWIN_LIB_PGPLOT])
  AC_REQUIRE([AC_F77_LIBRARY_LDFLAGS])
  AC_REQUIRE([SWIN_LIB_X11])

  AC_MSG_CHECKING([for PGPLOT installation])

  PGPLOT_CFLAGS="-I$PGPLOT_DIR"
  PGPLOT_LIBS="-L$PGPLOT_DIR -lcpgplot -lpgplot $FLIBS"


  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  LIBS="$ac_save_LIBS $PGPLOT_LIBS"
  CFLAGS="$ac_save_CFLAGS $PGPLOT_CFLAGS"

  AC_TRY_LINK([#include <cpgplot.h>],[cpgopen(""); cpgend();],
              have_pgplot=yes, have_pgplot=no)

  if test x"$have_pgplot" = xno; then
    PGPLOT_LIBS="$PGPLOT_LIBS $X_LIBS"
    LIBS="$LIBS $X_LIBS"
    AC_TRY_LINK([#include <cpgplot.h>],[cpgopen(""); cpgend();],
                have_pgplot=yes, have_pgplot=no)
  fi


  AC_MSG_RESULT($have_pgplot)

  LIBS="$ac_save_LIBS"
  CFLAGS="$ac_save_CFLAGS"

  if test x"$have_pgplot" = xyes; then
    AC_DEFINE([HAVE_PGPLOT], [1], [Define to 1 if you have the PGPLOT library])
    [$1]
  else
    echo "	Please ensure that the PGPLOT_DIR environment variable is set"
    PGPLOT_CFLAGS=""
    PGPLOT_LIBS=""
   [$2]
  fi

  AC_SUBST(PGPLOT_CFLAGS)
  AC_SUBST(PGPLOT_LIBS)
  AM_CONDITIONAL(HAVE_PGPLOT, [test x"$have_pgplot" = xyes])

])

