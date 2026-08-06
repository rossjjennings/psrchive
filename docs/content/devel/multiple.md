# Build Directory

When compiling the PSRCHIVE source code, the
location of the **build** directory is arbitrary. That is, 
when you run `configure` and `make`, your
current working directory need not necessarily be the top of the
source code tree.

This fact enables the PSRCHIVE software to be
compiled on multiple architectures or with different compilers using
the same source code tree. You simply create a separate **build**
directory for each architecture/compiler combination and change to
this directory before running `configure` and `make`.

For example, suppose that the software has been unpacked into the
`$HOME/psrchive` directory, and that you wish to compile
the software twice, once using the GNU compiler collection, and once
using the Intel compilers. Create two separate **build**
directories, e.g. `$HOME/gnu` and `$HOME/intel`.

To compile with GNU:

```
cd $HOME/gnu
$HOME/psrchive/configure --prefix=$PWD
make
make install
```

To compile with the Intel compilers:

```
cd $HOME/intel
setenv CXX icc
setenv CC icc
setenv F77 ifort
$HOME/psrchive/configure --prefix=$PWD
make
make install
```

In each case, the final step installs the PSRCHIVE
software into sub-directories of the current **build** directory.
