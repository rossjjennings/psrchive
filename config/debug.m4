dnl @synopsis SWIN_DEBUG
dnl 
AC_DEFUN([SWIN_DEBUG],
[
  AC_PROVIDE([SWIN_DEBUG])

  AC_ARG_ENABLE([debug],
                AC_HELP_STRING([--enable-debug],
                               [Enable debugging information]),
                [swin_debug=yes])

  if test x"$swin_debug" != xyes; then
    if test x"$CXXFLAGS" = x; then
      CXXFLAGS="-O2"
    fi
    if test x"$CFLAGS" = x; then
      CFLAGS="-O2"
    fi
    if test x"$FFLAGS" = x; then
      FFLAGS="-O2"
    fi
  else
    AC_MSG_WARN([Debugging information enabled.  Binaries will be large.])
  fi

])

