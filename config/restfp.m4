# _STAR_RESTFP_FIX
# ----------------
#
# redwards -- modified version of macro provided at 
# http://lists.apple.com/archives/fortran-dev/2004/Jul/msg00036.html
#
# Determines if we need to make any library fixes to get things to link
# properly. In fact, there's only a problem on OSX/Darwin, since the
# GCC installation which provides g77 and the (system) GCC which provides
# gcc can generate slightly incompatible object code. The following test
# is therefore pretty specific to OSX/Darwin.
#
# If there are any libraries that need to be added to the path, this adds
# them to FCLIBS, in the same way that AC_FC_LIBRARY_LDFLAGS does.
#
AC_DEFUN([_STAR_RESTFP_FIX],
[
  AC_REQUIRE([AC_F77_LIBRARY_LDFLAGS])
  AC_REQUIRE([AC_FC_LIBRARY_LDFLAGS])
AC_CACHE_CHECK([whether we need any library fixups],
[star_cv_restfp_fixup],
[if expr $build_os : 'darwin7' >/dev/null; then
dnl Only affects OSX/Darwin
if test "$GCC" = yes; then
dnl The problem only affects g77/gcc, so we know we're dealing with gcc below
AC_LANG_PUSH(C)
rm -f conftest*
star_cv_restfp_fixup=unknown
AC_LANG_CONFTEST(AC_LANG_PROGRAM([], restFP()))
{ AC_TRY_COMMAND($CC -o conftest.x -S conftest.c)
test $ac_status = 0
} &&
sed 's/_restFP/restFP/g' conftest.x>conftest.s &&
{ AC_TRY_COMMAND($CC -c -o conftest.$objext conftest.s)
test $ac_status = 0
} ||
star_cv_restfp_fixup=broken
AC_LANG_POP(C)
if test $star_cv_restfp_fixup = broken; then
AC_MSG_WARN([unable to assemble restFP test])
else
# Link this with the C compiler
AC_TRY_COMMAND($CC -o conftest conftest.$ac_objext)
_s_cstatus=$ac_status
# Link this with the Fortran compiler
AC_TRY_COMMAND($FC -o conftest conftest.$ac_objext)
if test $_s_cstatus = 0 -a $ac_status = 0; then
# both compilers can compile it
star_cv_restfp_fixup=no
elif test $_s_cstatus != 0 -a $ac_status != 0; then
# neither compiler can compile it
star_cv_restfp_fixup=no
elif test $_s_cstatus = 0; then
# The C compiler can, but the Fortran cannot
star_cv_restfp_fixup=yes
else
# The C compiler can't compile, but the Fortran can.
# Haven't heard of this case! Don't know what to do.
star_cv_restfp_fixup=broken
fi
fi
# Don't even try linking with -lcc_dynamic. It may work, but
# will be unpredictable:
# http://lists.apple.com/mhonarc/fortran-dev/msg00769.html
if test $star_cv_restfp_fixup = yes; then
AC_TRY_COMMAND($FC -o conftest conftest.$objext -L/usr/lib -lgcc)
if test $ac_status = 0; then
star_cv_restfp_fixup=lgcc
fi
fi
if test $star_cv_restfp_fixup = yes; then
# ooops
AC_MSG_WARN([unable to solve restFP problem])
star_cv_restfp_fixup=broken
fi
rm -f conftest*
else # !gcc
star_cv_restfp_fixup=no
fi
else # !Darwin
star_cv_restfp_fixup=no
fi
])
# Define FCLIBS, just as AC_FC_LIBRARY_LDFLAGS does
if test $star_cv_restfp_fixup = lgcc; then
FCLIBS="$FCLIBS /usr/lib/libgcc.a"
FLIBS="$FLIBS /usr/lib/libgcc.a"
fi
AC_SUBST(FCLIBS)
])

