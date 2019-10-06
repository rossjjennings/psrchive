from distutils.core import setup, Extension
# This is an alternate means to install python psrchive in case the makefile does not work.

import numpy

psrchive_module = Extension('_psrchive', sources=['psrchive_wrap.cxx'],libraries=['psrmore','psrbase','psrutil'],include_dirs=[numpy.get_include(),'../../local_include/','.','../..'],extra_compile_args=['-DHAVE_CONFIG_H'])
setup (name = 'psrchive', version = '0.1', ext_modules = [psrchive_module], py_modules = ["psrchive"],)

