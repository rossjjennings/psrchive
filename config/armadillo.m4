# PSR_LIB_ARMADILLO([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([PSR_LIB_ARMADILLO],
[
  AC_PROVIDE([PSR_LIB_ARMADILLO])

  AC_ARG_WITH([armadillo-dir],
              AC_HELP_STRING([--with-armadillo-dir=DIR],
                             [armadillo is installed in DIR]))

  ARMADILLO_CFLAGS=""
  ARMADILLO_LIBS=""

  if test x"$with_armadillo_dir" = xno; then
    # user disabled armadillo. Leave cache alone.
    have_armadillo="User disabled armadillo."
  else

    AC_MSG_CHECKING([for armadillo installation])

    # "yes" is not a specification
    if test x"$with_armadillo_dir" = xyes; then
      with_armadillo_dir=
    fi

    ARMADILLO_CFLAGS="`pkg-config --cflags armadillo`"
    ARMADILLO_LIBS="`pkg-config --libs armadillo`"

    have_armadillo="not found"

    ac_save_CPPFLAGS="$CPPFLAGS"
    ac_save_LIBS="$LIBS"

    CPPFLAGS="$ARMADILLO_CFLAGS $CPPFLAGS"
    LIBS="$ARMADILLO_LIBS $LIBS"

    AC_LANG_PUSH(C++)

    AC_TRY_LINK([#include <armadillo>], 
                [arma::gmm_diag model;],
                have_armadillo=yes, have_armadillo=no)

    AC_LANG_POP(C++)

    if test "$have_armadillo" != "yes"; then
      ARMADILLO_CFLAGS=""
      ARMADILLO_LIBS=""
    fi

    LIBS="$ac_save_LIBS"
    CPPFLAGS="$ac_save_CPPFLAGS"

  fi

  AC_MSG_RESULT([$have_armadillo])

  if test "$have_armadillo" = "yes"; then
    AC_DEFINE([HAVE_ARMADILLO], [1], [Define if the armadillo library is present])
    [$1]
  else
    AC_MSG_NOTICE([Ensure that the PKG_CONFIG_PATH environment variable points to])
    AC_MSG_NOTICE([the lib/pkgconfig sub-directory of the root directory where])
    AC_MSG_NOTICE([the armadillo library was installed.])
    AC_MSG_NOTICE([Alternatively, use the --with-armadillo-dir option.])
    [$2]
  fi

  AC_SUBST(ARMADILLO_LIBS)
  AC_SUBST(ARMADILLO_CFLAGS)
  AM_CONDITIONAL(HAVE_ARMADILLO,[test "$have_armadillo" = "yes"])

])

