dnl @synopsis SWIN_BEST_PARTIAL_SPECIALIZATION
dnl
dnl If the compiler can find the best partial specialization of a
dnl template with template arguments, define
dnl HAVE_BEST_PARTIAL_SPECIALIZATION.
dnl
dnl @category Cxx
dnl @author Willem van Straten <straten@astron.nl>
dnl @version 2005-02-12
dnl @license AllPermissive

AC_DEFUN([SWIN_BEST_PARTIAL_SPECIALIZATION],
[AC_CACHE_CHECK(if compiler finds best partial specialization,
ac_cv_cxx_best_partial_specialization,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
template<typename A, typename B> class X { };
template<typename A, typename B, template<class> class C>
class X< C<A>, C<B> > { };
template<typename A, typename B, template<class> class C>
class X< C<A>, B > { };
template<class T> class Test1 { };
],[X< Test1<int>, Test1<double> > test1;],
 ac_cv_cxx_best_partial_specialization=yes,
 ac_cv_cxx_best_partial_specialization=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_best_partial_specialization" = yes; then
  AC_DEFINE(HAVE_BEST_PARTIAL_SPECIALIZATION,,
            [define if the compiler finds best partial specialization])
fi
])

