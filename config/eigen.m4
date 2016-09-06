dnl @synopsis SWIN_LIB_EIGEN
dnl 
AC_DEFUN([SWIN_LIB_EIGEN],
[
  AC_PROVIDE([SWIN_LIB_EIGEN])

  SWIN_PACKAGE_OPTIONS([eigen])

  AC_MSG_CHECKING([for Eigen C++ template library for linear algebra])

  if test "$have_eigen" != "user disabled"; then

    AC_LANG_PUSH(C++)

    SWIN_PACKAGE_FIND([eigen],[Dense])
    SWIN_PACKAGE_TRY_COMPILE([eigen],[#include <Eigen/Dense>])

    AC_LANG_POP(C++)

  else
    have_eigen=no
  fi

  AC_MSG_RESULT([$have_eigen])

  if test $have_eigen = yes; then
    AC_DEFINE(HAVE_EIGEN,1,[Define if the Eigen library is installed])
    EIGEN_CFLAGS="$eigen_CFLAGS $EIGEN_CFLAGS"
  fi

  AC_SUBST(EIGEN_CFLAGS)

  AM_CONDITIONAL(HAVE_EIGEN,[test "$have_eigen" = yes])

])

