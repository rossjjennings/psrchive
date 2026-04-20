# GNU Autotools

To compile the PSRCHIVE source code after cloning the Git repository, you will need the GNU Build System (also called the [GNU Autotools](http://sources.redhat.com/autobook)) including

* [GNU autoconf](http://www.gnu.org/software/autoconf/autoconf.md) [version 2.59 or higher];
* [GNU automake](http://www.gnu.org/software/automake/automake.md) [version 1.9.3 or higher];
* [GNU libtool](http://www.gnu.org/software/libtool/libtool.md) [version 1.5.8 or higher]; and
* [GNU m4](http://www.gnu.org/software/m4/m4.md) [version 1.4 or higher].

## Recommended Instructions

Install third party software using the package manager for your version of Linux/BSD/Unix; e.g. on Ubuntu

```
sudo apt update
# install the GNU build system
sudo apt install -y autotools-dev autoconf libtool make 
# install the GNU compiler collection
sudo apt install g++ gfortran
# install some other third party packages used by psrchive
sudo apt install libfftw3-dev pgplot5 libcfitsio-dev
```

## Outdated Instructions

If you choose to follow these instructions, then you'll need to update the version numbers on each package.

### Download

The GNU autotools can be downloaded using the following script.

```
#!/bin/csh

setenv mirror http://ftp.gnu.org/gnu

echo downloading from $mirror

echo downloading m4 ...
curl -O -f -L $mirror/m4/m4-1.4.6.tar.gz

echo downloading autoconf ...
curl -O -f -L $mirror/autoconf/autoconf-2.60.tar.gz

echo downloading automake ...
curl -O -f -L $mirror/automake/automake-1.9.6.tar.gz

echo downloading libtool ...
curl -O -f -L $mirror/libtool/libtool-1.5.8.tar.gz
```
Before running this script,
you may want to edit it to use a GNU mirror, or use `wget` instead of `curl`.You could also simply click the following links:

* [m4-1.4.6.tar.gz](http://ftp.gnu.org/gnu/m4/m4-1.4.6.tar.gz)
* [autoconf-2.60.tar.gz](http://ftp.gnu.org/gnu/autoconf/autoconf-2.60.tar.gz)
* [automake-1.9.6.tar.gz](http://ftp.gnu.org/gnu/automake/automake-1.9.6.tar.gz)
* [libtool-1.5.8.tar.gz](http://ftp.gnu.org/gnu/libtool/libtool-1.5.8.tar.gz)

### Install

After running the download script, the GNU autotools can be installed
using the following script.

```
#! /bin/csh -ef

if ( $?PSRHOME ) then
  setenv prefix $PSRHOME/packages/$LOGIN_ARCH
else
  setenv prefix /usr/local
endif

alias MAKE "./configure --prefix=$prefix; make; make install; rehash; cd .."

echo installing in $prefix

setenv PATH ${prefix}:${PATH}

echo unpacking m4 ...
gunzip -c m4-1.4.6.tar.gz | tar xf -
cd m4-1.4.6
MAKE

echo unpacking autoconf ...
gunzip -c autoconf-2.60.tar.gz | tar xf -
cd autoconf-2.60
MAKE

gunzip -c automake-1.9.6.tar.gz | tar xf -
cd automake-1.9.6
MAKE

gunzip -c libtool-1.5.8.tar.gz | tar xf -
cd libtool-1.5.8
MAKE
```