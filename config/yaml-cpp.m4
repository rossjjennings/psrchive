# PSR_LIB_YAMLCPP([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
AC_DEFUN([PSR_LIB_YAMLCPP],
[
  AC_PROVIDE([PSR_LIB_YAMLCPP])

  AC_ARG_WITH([yamlcpp-dir],
              AC_HELP_STRING([--with-yamlcpp-dir=DIR],
                             [yaml-cpp is installed in DIR]))

  YAMLCPP_CFLAGS=""
  YAMLCPP_LIBS=""

  if test x"$with_yamlcpp_dir" = xno; then
    # user disabled yamlcpp. Leave cache alone.
    have_yamlcpp="User disabled yaml-cpp."
  else

    AC_MSG_CHECKING([for yaml-cpp installation])

    # "yes" is not a specification
    if test x"$with_yamlcpp_dir" = xyes; then
      with_yamlcpp_dir=
    fi

    PKGS="$prefix/share/pkgconfig:$PKG_CONFIG_PATH"
    YAMLCPP_CFLAGS="`PKG_CONFIG_PATH=$PKGS pkg-config --cflags yaml-cpp`"
    YAMLCPP_LIBS="`PKG_CONFIG_PATH=$PKGS pkg-config --libs yaml-cpp`"

    have_yamlcpp="not found"

    ac_save_CPPFLAGS="$CPPFLAGS"
    ac_save_LIBS="$LIBS"

    CPPFLAGS="$YAMLCPP_CFLAGS $CPPFLAGS"
    LIBS="$YAMLCPP_LIBS $LIBS"

    AC_LANG_PUSH(C++)

    AC_TRY_LINK([#include <yaml-cpp/yaml.h>], 
                [YAML::Node primes = YAML::Load("");],
                have_yamlcpp=yes, have_yamlcpp=no)

    AC_LANG_POP(C++)

    if test "$have_yamlcpp" != "yes"; then
      YAMLCPP_CFLAGS=""
      YAMLCPP_LIBS=""
    fi

    LIBS="$ac_save_LIBS"
    CPPFLAGS="$ac_save_CPPFLAGS"

  fi

  AC_MSG_RESULT([$have_yamlcpp])

  if test "$have_yamlcpp" = "yes"; then
    AC_DEFINE([HAVE_YAMLCPP], [1], [Define if the yaml-cpp library is present])
    [$1]
  else
    AC_MSG_NOTICE([Ensure that the PKG_CONFIG_PATH environment variable points to])
    AC_MSG_NOTICE([the lib/pkgconfig sub-directory of the root directory where])
    AC_MSG_NOTICE([the yaml-cpp library was installed.])
    AC_MSG_NOTICE([Alternatively, use the --with-yamlcpp-dir option.])
    [$2]
  fi

  AC_SUBST(YAMLCPP_LIBS)
  AC_SUBST(YAMLCPP_CFLAGS)
  AM_CONDITIONAL(HAVE_YAMLCPP,[test "$have_yamlcpp" = "yes"])

])

