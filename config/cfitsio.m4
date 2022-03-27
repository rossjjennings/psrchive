# SWIN_LIB_CFITSIO([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_CFITSIO],
[
  AC_PROVIDE([SWIN_LIB_CFITSIO])

  AC_ARG_WITH([cfitsio-dir],
              AC_HELP_STRING([--with-cfitsio-dir=DIR],
                             [cfitsio is installed in DIR]))

  CFITSIO_CFLAGS=""
  CFITSIO_LIBS=""

  if test x"$with_cfitsio_dir" = xno; then
    # user disabled cfitsio. Leave cache alone.
    have_cfitsio="User disabled cfitsio."
  else

    AC_MSG_CHECKING([for cfitsio installation])

    # "yes" is not a specification
    if test x"$with_cfitsio_dir" = xyes; then
      with_cfitsio_dir=
    fi

    CFITSIO_CFLAGS="`pkg-config --cflags cfitsio`"
    CFITSIO_LIBS="`pkg-config --libs cfitsio`"

    have_cfitsio="not found"

    ac_save_CPPFLAGS="$CPPFLAGS"
    ac_save_LIBS="$LIBS"

    CPPFLAGS="$CFITSIO_CFLAGS $CPPFLAGS"
    LIBS="$CFITSIO_LIBS $LIBS"

    AC_TRY_LINK([#include <fitsio.h>], 
                [fits_movnam_hdu(0,0,0,0,0);],
                have_cfitsio=yes, have_cfitsio=no)

    if test "$have_cfitsio" != "yes"; then
      CFITSIO_CFLAGS=""
      CFITSIO_LIBS=""
    fi

    LIBS="$ac_save_LIBS"
    CPPFLAGS="$ac_save_CPPFLAGS"

  fi

  AC_MSG_RESULT([$have_cfitsio])

  if test "$have_cfitsio" = "yes"; then
    AC_DEFINE([HAVE_CFITSIO], [1], [Define if the cfitsio library is present])
    [$1]
  else
    AC_MSG_NOTICE([Ensure that the PKG_CONFIG_PATH environment variable points to])
    AC_MSG_NOTICE([the lib/pkgconfig sub-directory of the root directory where])
    AC_MSG_NOTICE([the cfitsio library was installed.])
    AC_MSG_NOTICE([Alternatively, use the --with-cfitsio-dir option.])
    [$2]
  fi

  AC_SUBST(CFITSIO_LIBS)
  AC_SUBST(CFITSIO_CFLAGS)
  AM_CONDITIONAL(HAVE_CFITSIO,[test "$have_cfitsio" = "yes"])

])

