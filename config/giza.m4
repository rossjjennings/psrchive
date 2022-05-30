# SWIN_LIB_GIZA([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_GIZA],
[
  AC_PROVIDE([SWIN_LIB_GIZA])

  AC_ARG_WITH([giza-dir],
              AC_HELP_STRING([--with-giza-dir=DIR],
                             [giza is installed in DIR]))

  GIZA_CFLAGS=""
  GIZA_LIBS=""

  if test x"$with_giza_dir" = xno; then
    # user disabled giza. Leave cache alone.
    have_giza="User disabled giza."
  else

    AC_MSG_CHECKING([for giza installation])

    # "yes" is not a specification
    if test x"$with_giza_dir" = xyes; then
      with_giza_dir=
    fi

    GIZA_CFLAGS="`pkg-config --cflags giza`"
    GIZA_LIBS="`pkg-config --libs giza` -lcpgplot"

    have_giza="not found"

    ac_save_CPPFLAGS="$CPPFLAGS"
    ac_save_LIBS="$LIBS"

    CPPFLAGS="$GIZA_CFLAGS $CPPFLAGS"
    LIBS="$GIZA_LIBS $LIBS"

    AC_TRY_LINK([#include <cpgplot.h>],[cpgopen(""); cpgend();],
                have_giza=yes, have_giza=no)

    if test "$have_giza" != "yes"; then
      GIZA_CFLAGS=""
      GIZA_LIBS=""
    fi

    LIBS="$ac_save_LIBS"
    CPPFLAGS="$ac_save_CPPFLAGS"

  fi

  AC_MSG_RESULT([$have_giza])

  if test "$have_giza" = "yes"; then
    AC_DEFINE([HAVE_GIZA], [1], [Define if the giza library is present])
    [$1]
  else
    AC_MSG_NOTICE([Ensure that the PKG_CONFIG_PATH environment variable points to])
    AC_MSG_NOTICE([the lib/pkgconfig sub-directory of the root directory where])
    AC_MSG_NOTICE([the giza library was installed.])
    AC_MSG_NOTICE([Alternatively, use the --with-giza-dir option.])
    [$2]
  fi

  AC_SUBST(GIZA_LIBS)
  AC_SUBST(GIZA_CFLAGS)
  AM_CONDITIONAL(HAVE_GIZA,[test "$have_giza" = "yes"])

])

