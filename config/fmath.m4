dnl @synopsis SWIN_FMATH
dnl
dnl Ensures that the FLIBS macro contains the required maths libraries
dnl Copied from ASCEND configure.in file
dnl 
AC_DEFUN([SWIN_FMATH],
[
  AC_PROVIDE([SWIN_FMATH])
  AC_REQUIRE([AC_F77_LIBRARY_LDFLAGS])

  # search for the fortran libraries based on the system type.
  AC_MSG_RESULT([checking for fortran libraries based on system type])
  case "$host" in
    alpha-dec-osf*)
      AC_CHECK_LIB(for, for_lle,  [FLIBS="$FLIBS -lfor"], , $FLIBS)
      AC_CHECK_LIB(ots, _OtsFill, [FLIBS="$FLIBS -lots"], , $FLIBS)
      AC_CHECK_LIB(Ufor, etime_,  [FLIBS="$FLIBS -lUfor"], , $FLIBS)
      ;;
    mips-dec-ultrix*)
      AC_CHECK_LIB(for, for_lle,  [FLIBS="$FLIBS -lfor"], , $FLIBS)
      ;;
    hppa1.1-hp-hpux*)
      AC_CHECK_LIB(cl, FTN_QATAN, [FLIBS="$FLIBS -lcl"], , $FLIBS)
      ;;
    *-ibm-aix*)
      # need to add -lc before -lxlf on AIX to
      # avoid getting the wrong getenv
      FLIBS="-lc $FLIBS"
      AC_CHECK_LIB(xlf, _xldabs,    [FLIBS="$FLIBS -lxlf"], , $FLIBS)
      AC_CHECK_LIB(xlfutil, srand_, [FLIBS="$FLIBS -lxlfutil"], , $FLIBS)
      AC_CHECK_LIB(xlf90, printf,   [FLIBS="$FLIBS -lxlf90"], , $FLIBS)
      ;;
    *-linux-*)
      AC_CHECK_LIB(f2c, pow_dd, [FLIBS="$FLIBS -lf2c"], , $FLIBS)
      AC_CHECK_LIB(g2c, pow_dd, [FLIBS="$FLIBS -lg2c"], , $FLIBS)
      ;;
    sparc-sun-solaris*)
      AC_CHECK_LIB(sunmath, d_sqrt_, [FLIBS="$FLIBS -lsunmath"], , $FLIBS)
      AC_CHECK_LIB(F77, f77_init,    [FLIBS="$FLIBS -lF77"], , $FLIBS)
      AC_CHECK_LIB(M77, inmax_,      [FLIBS="$FLIBS -lM77"], , $FLIBS)
      ;;
    sparc-sun-sunos4*)
      AC_CHECK_LIB(F77, f77_init, [FLIBS="$FLIBS -lF77"], , $FLIBS)
      AC_CHECK_LIB(M77, inmax_,   [FLIBS="$FLIBS -lM77"], , $FLIBS)
      ;;
    mips-sgi-irix*)
      AC_CHECK_LIB(F77, s_copy,     [FLIBS="$FLIBS -lF77"], , $FLIBS)
      AC_CHECK_LIB(I77, f_exit,     [FLIBS="$FLIBS -lI77"], , $FLIBS)
      AC_CHECK_LIB(isam, mkidxname, [FLIBS="$FLIBS -lisam"], , $FLIBS)
      ;;
    *)
      AC_CHECK_LIB(F77,  printf, [FLIBS="$FLIBS -lF77"],  , $FLIBS)
      AC_CHECK_LIB(M77,  printf, [FLIBS="$FLIBS -lM77"],  , $FLIBS)
      AC_CHECK_LIB(f2c,  pow_dd, [FLIBS="$FLIBS -lf2c"],  , $FLIBS)
      AC_CHECK_LIB(f77,  printf, [FLIBS="$FLIBS -lf77"],  , $FLIBS)
      AC_CHECK_LIB(for,  printf, [FLIBS="$FLIBS -lfor"],  , $FLIBS)
      AC_CHECK_LIB(ots,  printf, [FLIBS="$FLIBS -lots"],  , $FLIBS)
      AC_CHECK_LIB(Ufor, printf, [FLIBS="$FLIBS -lUfor"], , $FLIBS)
      ;;
  esac

])

