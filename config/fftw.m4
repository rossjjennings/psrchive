# SWIN_LIB_FFTW([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([SWIN_LIB_FFTW],
[
  AC_PROVIDE([SWIN_LIB_FFTW])

  AC_ARG_WITH([fftw3f-dir],
              AC_HELP_STRING([--with-fftw3f-dir=DIR],
                             [fftw3f is installed in DIR]))

  FFTW_CFLAGS=""
  FFTW_LIBS=""

  if test x"$with_fftw3f_dir" = xno; then
    # user disabled fftw3f. Leave cache alone.
    have_fftw3="User disabled fftw3f."
  else

    AC_MSG_CHECKING([for single-precision FFTW-3 library])

    # "yes" is not a specification
    if test x"$with_fftw3f_dir" = xyes; then
      with_fftw3f_dir=
    fi

    FFTW_CFLAGS="`pkg-config --cflags fftw3f`"
    FFTW_LIBS="`pkg-config --libs fftw3f`"

    have_fftw3="not found"

    ac_save_CPPFLAGS="$CPPFLAGS"
    ac_save_LIBS="$LIBS"

    CPPFLAGS="$FFTW_CFLAGS $CPPFLAGS"
    LIBS="$FFTW_LIBS $LIBS"

    AC_TRY_LINK([#include <fftw3.h>], 
                [fftwf_plan_dft_1d(0,0,0,FFTW_FORWARD,FFTW_ESTIMATE);],
                have_fftw3=yes, have_fftw3=no)

    if test "$have_fftw3" != "yes"; then
      FFTW_CFLAGS=""
      FFTW_LIBS=""
    fi

    LIBS="$ac_save_LIBS"
    CPPFLAGS="$ac_save_CPPFLAGS"

  fi

  AC_MSG_RESULT([$have_fftw3])

  if test "$have_fftw3" = "yes"; then
    AC_DEFINE([HAVE_FFTW3], [1], [Define if the fftw3f library is present])
    [$1]
  else
    AC_MSG_NOTICE([Ensure that the PKG_CONFIG_PATH environment variable points to])
    AC_MSG_NOTICE([the lib/pkgconfig sub-directory of the root directory where])
    AC_MSG_NOTICE([the fftw3f library was installed.])
    AC_MSG_NOTICE([Alternatively, use the --with-fftw3f-dir option.])
    [$2]
  fi

  AC_SUBST(FFTW_LIBS)
  AC_SUBST(FFTW_CFLAGS)
  AM_CONDITIONAL(HAVE_FFTW3,[test "$have_fftw3" = "yes"])

])

