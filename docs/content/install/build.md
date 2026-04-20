# Compilation and Installation

## Installation Directory

The installation directory is the directory to which all build
products (header files, libraries, executables, and runtime data) will
be installed when you type `make install`. The installation
directory is set when you run the `configure` script, and is
stored in the Makefile variable, `$prefix`. When you type
`make install`, build products are installed in

```
$prefix/include
$prefix/lib
$prefix/bin
$prefix/share
```

There are currently three ways in which the installation
directory can be specified:

1. `$PSRHOME` environment variable: if set, then installation 
directory will be set to `$PSRHOME/$LOGIN_ARCH`.

2. Command line: the install directory may be specified using the
`--prefix` argument to the `configure` script; e.g. 
```
./configure --prefix=$HOME/Pulsar
```

3. Default: If the `$PSRHOME` environment variable is not set, and the
`--prefix` argument is not used, then the installation directory
will default to `/usr/local/psrchive`.

## Configure the build

To configure the compilation and installation of PSRCHIVE, run the
configure script; e.g.
```
./configure
```
will configure the compilation of static libraries.  To compile
shared object libraries that can be dynamically loaded at runtime,
```
./configure --enable-shared
```

The configure script may finish with messages like
```
The PGPLOT library was not found:
  PSRCHIVE applications will not be able to plot.
  Run ./packages/pgplot.csh to automatically install.
```
To automatically install any dependencies, simply run the scripts as suggested, set any environment variables as directed, then re-run the configure script, as described in the section on [Automatic Package Installation](third/install.md).

## Compile, run tests, and install

When satisfied that the configure script has found all of the desired dependencies, run
```
make
make check
make install
```

