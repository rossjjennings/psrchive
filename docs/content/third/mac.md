# PSRCHIVE: Instructions for Mac users

## Mac OS X Users

PSRCHIVE compiles on Mac OS X using Apple's
Xcode Command Line Tools and a compatible version of gfortran.
To compile PGPLOT and use the X11 graphics device, you will also require X11.
Alternatively, you can use the XQuartz-aware version of PGPLOT that
can be installed using [fink](http://www.finkproject.org) or
[macports](http://www.macports.org).

The various dependencies can be installed as follows:

* Install X11 and Xcode Tools:
  1. insert Mac OS X Install Disk 1* click Finder (blue happy face)* search for "X11User.pkg" and install it* search for "XcodeTools.mpkg" and install it
* **Mac OS X 10.5 (Leopard) and higher**:
  Install [the latest release of X11.pkg from XQuartz](http://xquartz.macosforge.org).
* Install the appropriate version of gfortran. Links to
  compiled binaries are maintained at
  [gcc.gnu.org](http://gcc.gnu.org/wiki/GFortranBinaries#MacOS).
  (Most installers put the executable in /usr/local/bin, so you
  will have to add this to your PATH.)
* **Mac OS X 10.8 (Mountain Lion) and higher**:
  Install [Homebrew](http://mxcl.github.com/homebrew/)
  and use it to install gfortran. Note that the "Install Homebrew"
  ruby command provided near the bottom of the Homebrew web-page
  works only in bash (it fails in tcsh).
* The third party packages, FFTW and CFITSIO, can be installed on a Mac
  as normal.
* If not already installed using fink or macports, it is recommended
  to install PGPLOT using the packages/pgplot.csh  script that
  is created when you first run the PSRCHIVE configure script.

  If you prefer to install PGPLOT by hand, note that the standard
  source package will work if you use the linux g77\_gcc\_aout
  architecture, but you will need to edit the makefile, e.g. changing
  g77  to gfortran .
  For detailed help, please follow these
  [PGPLOT installation instructions](pgplot.md).

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
