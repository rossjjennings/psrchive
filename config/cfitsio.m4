dnl @synopsis SWIN_LIB_CFITSIO
dnl 
AC_DEFUN([SWIN_LIB_CFITSIO],
[
  AC_PROVIDE([SWIN_LIB_CFITSIO])
  AC_REQUIRE([ETR_SOCKET_NSL])

  AC_ARG_WITH([cfitsio-dir],
              AC_HELP_STRING([--with-cfitsio-dir=DIR],
                             [CFITSIO is in DIR]))

  AC_ARG_WITH([cfitsio-include-dir],
              AC_HELP_STRING([--with-cfitsio-include-dir=DIR],
                             [CFITSIO header files are in DIR]))

  AC_ARG_WITH([cfitsio-lib-dir],
              AC_HELP_STRING([--with-cfitsio-lib-dir=DIR],
                             [CFITSIO library is in DIR]))

  CFITSIO_CFLAGS=""
  CFITSIO_LIBS=""

  if test x"$with_cfitsio_dir" = x"no" ||
     test x"$with_cfitsio_include-dir" = x"no" ||
     test x"$with_cfitsio_lib_dir" = x"no"; then
    # user disabled cfitsio. Leave cache alone.
    have_cfitsio="User disabled CFITSIO."
  else

    # "yes" is not a specification
    if test x"$with_cfitsio_dir" = xyes; then
      with_cfitsio_dir=
    fi
    if test x"$with_cfitsio_include_dir" = xyes; then
      if test x"$with_cfitsio_dir" = xyes; then
        with_cfitsio_include_dir=$with_cfitsio_dir/include
      else
        with_cfitsio_include_dir=
      fi
    fi
    if test x"$with_cfitsio_lib_dir" = xyes; then
      if test x"$with_cfitsio_dir" = xyes; then
        with_cfitsio_lib_dir=$with_cfitsio_dir/lib
      else
        with_cfitsio_lib_dir=
      fi
    fi

    AC_MSG_CHECKING([for CFITSIO installation])

    ## Look for the header file ##
    cf_include_path_list="$with_cfitsio_include_dir .
                          /usr/local/include/cfitsio
                          /usr/local/cfitsio/include"

    ac_save_CPPFLAGS="$CPPFLAGS"

    for cf_dir in $cf_include_path_list; do
      CPPFLAGS="-I$cf_dir $ac_save_CPPFLAGS"
      AC_TRY_COMPILE([#include <fitsio.h>], [],
                     have_cfitsio=yes, have_cfitsio=no)
      if test x"$have_cfitsio" = xyes; then
        break
      fi
    done

    ## Look for the library ##
    cf_lib_path_list="$with_cfitsio_lib_dir .
                      /usr/local/lib
                      /usr/local/cfitsio/lib
                      ${PSRHOME}/packages/${LOGIN_ARCH}/cfitsio"

    ac_save_LIBS="$LIBS"

    for cf_dir in $cf_lib_path_list; do
      LIBS="-L$cf_dir -lcfitsio $SOCKET_LIBS $ac_save_LIBS"
      AC_TRY_LINK([#include <fitsio.h>], [fits_movnam_hdu(0,0,0,0,0);],
                  have_cfitsio=yes, have_cfitsio=no)
      if test x"$have_cfitsio" = xyes; then
        if test x"$cf_dir" == x.; then
          CFITSIO_LIBS="-lcfitsio $SOCKET_LIBS"
        else
          CFITSIO_LIBS="-L$cf_dir -lcfitsio $SOCKET_LIBS"
        fi
        break
      fi
    done

    LIBS="$ac_save_LIBS"
    CPPFLAGS="$ac_save_CPPFLAGS"

  fi

  AC_MSG_RESULT([$have_cfitsio])

  if test x"$have_cfitsio" = xyes; then
    AC_DEFINE([HAVE_CFITSIO], [1], [Define if the CFITSIO library is present])
    [$1]
  else
    echo "	The PSRFITS code will not be compiled"
   [$2]
  fi

  AC_SUBST(CFITSIO_LIBS)
  AC_SUBST(CFITSIO_CFLAGS)
  AM_CONDITIONAL(HAVE_CFITSIO,[test x"$have_cfitsio" = xyes])

])

