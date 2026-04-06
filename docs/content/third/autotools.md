# PSRCHIVE: GNU Autotools Instructions

## GNU Autotools: autoconf, automake, libtool

To compile the PSRCHIVE source code
from the CVS repository, you will need the GNU Build System
(also called the [GNU Autotools](http://sources.redhat.com/autobook))
including

* [GNU autoconf](http://www.gnu.org/software/autoconf/autoconf.md) [version 2.59 or higher];
* [GNU automake](http://www.gnu.org/software/automake/automake.md) [version 1.9.3 or higher];
* [GNU libtool](http://www.gnu.org/software/libtool/libtool.md) [version 1.5.8 or higher]; and
* [GNU m4](http://www.gnu.org/software/m4/m4.md) [version 1.4 or higher].

### Recommended Instructions

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

### Outdated Instructions

If you choose to follow these instructions, then you'll need to update the version numbers on each package.

#### Download

The GNU autotools can be downloaded using the
[download script](autotools/download.csh). Before running this script,
you may want to edit it to use a GNU mirror. If you don't have
`curl` installed on your system, you could also simply click
the following links:

* [m4-1.4.6.tar.gz](http://ftp.gnu.org/gnu/m4/m4-1.4.6.tar.gz)* [autoconf-2.60.tar.gz](http://ftp.gnu.org/gnu/autoconf/autoconf-2.60.tar.gz)* [automake-1.9.6.tar.gz](http://ftp.gnu.org/gnu/automake/automake-1.9.6.tar.gz)* [libtool-1.5.8.tar.gz](http://ftp.gnu.org/gnu/libtool/libtool-1.5.8.tar.gz)

#### Install

After running the download script, the GNU autotools can be installed
using the [make script](autotools/make.csh).

#### Group Installation

If the PSRHOME environment variable is defined, the make
script will install the GNU autotools in
$PSRHOME/packages/$LOGIN\_ARCH.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
