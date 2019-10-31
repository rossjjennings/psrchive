dnl @synopsis AC_PYTHON_DEVEL
dnl
dnl Checks for Python and tries to get the include path to 'Python.h'.
dnl It provides the $(PYTHON_CPPFLAGS) and $(PYTHON_LDFLAGS) output
dnl variable.
dnl
dnl Added check for Numpy - 2008/08/27 PBD
dnl
dnl @category InstalledPackages
dnl @author Sebastian Huber <sebastian-huber@web.de>
dnl @author Alan W. Irwin <irwin@beluga.phys.uvic.ca>
dnl @author Rafael Laboissiere <laboissiere@psy.mpg.de>
dnl @author Andrew Collier <colliera@nu.ac.za>
dnl @version 2004-07-14
dnl @license GPLWithACException

AC_DEFUN([AC_PYTHON_DEVEL],[
	#
	# should allow for checking of python version here...
	#
	AC_REQUIRE([AM_PATH_PYTHON])

	# Check for Python include path
	AC_MSG_CHECKING([for Python include path])
	python_path=`echo $PYTHON | sed "s,/bin.*$,,"`
	for i in "$python_path/include/python$PYTHON_VERSION/" "$python_path/include/python/" "$python_path/" ; do
		python_path=`find $i -type f -name Python.h -print | sed "1q"`
		if test -n "$python_path" ; then
			break
		fi
	done
	if test -z "$python_path" ; then
		# do another check for the Python include path.
		AC_MSG_CHECKING([for Python include path (again)])
		python_path=`$PYTHON -c "from sysconfig import get_paths; print(get_paths().get('include'))"`
	fi
	if test -z "$python_path" ; then
		python_path=`echo $python_path | sed "s,/Python.h$,,"`
	fi
	AC_MSG_RESULT([$python_path])
	if test -z "$python_path" ; then
		AC_MSG_ERROR([cannot find Python include path])
	fi

	# Check for numpy
	AC_MSG_CHECKING([for Numpy include path])
	numpy_include=`$PYTHON -c "import numpy; print(numpy.get_include())"`
	AC_MSG_RESULT([$numpy_include])
	if test -z "$numpy_include" ; then
		AC_MSG_ERROR([cannot find Numpy include path])
	fi

	# Output full include path
	AC_SUBST([PYTHON_CPPFLAGS],["-I$python_path -I$numpy_include"])

	# Check for Python library path
	AC_MSG_CHECKING([for Python library path])
	python_path=`$PYTHON -c "import os; from sysconfig import get_paths; print(os.path.dirname(get_paths().get('stdlib')))"`
	AC_MSG_RESULT([$python_path])

	if test -z "$python_path" ; then
		python_path=`echo $PYTHON | sed "s,/bin.*$,,"`
		for i in "$python_path/lib/python$PYTHON_VERSION/config/" "$python_path/lib64/python$PYTHON_VERSION/config/" "$python_path/lib/python$PYTHON_VERSION/" "$python_path/lib/python/config/" "$python_path/lib/python/" "$python_path/" ; do
			python_path=`find $i -name libpython$PYTHON_VERSION.* -print | sed "1q"`
			if test -n "$python_path" ; then
				break
			fi
		done
	fi

	if test -z "$python_path" ; then
		python_path=`echo $python_path | sed "s,/libpython.*$,,"`
	fi

	AC_MSG_RESULT([$python_path])
	if test -z "$python_path" ; then
		AC_MSG_ERROR([cannot find Python library path])
	fi
	python_major_version=`$PYTHON -c "import sys; print(sys.version_info[[0]])"`
	python_lib_name=`echo "python${python_major_version}"`
	AC_MSG_RESULT([$python_lib_name])
	AC_SUBST([PYTHON_LDFLAGS],["-L$python_path -l$python_lib_name"])
	AC_MSG_RESULT([$PYTHON_LDFLAGS])

	python_site=`echo $python_path | sed "s/config/site-packages/"`
	AC_SUBST([PYTHON_SITE_PKG],[$python_site])
	#
	# libraries which must be linked in when embedding
	#
	AC_MSG_CHECKING(python extra libraries)
	PYTHON_EXTRA_LIBS=`$PYTHON -c "import distutils.sysconfig; \
                conf = distutils.sysconfig.get_config_var; \
                print(conf('LOCALMODLIBS')+' '+conf('LIBS'))"`
	AC_MSG_RESULT([$PYTHON_EXTRA_LIBS])
	AC_SUBST(PYTHON_EXTRA_LIBS)
])
