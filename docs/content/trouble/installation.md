# Installation problems

The PSRCHIVE code makes use of a number of
third-party packages. The `configure` script will do its best
to detect the location of these packages and configure the software to
use them. However, as directory structures vary widely from place to
place, the most commonly encountered problems are usually related to
third-party dependencies.

As always, if you have any difficulties with compiling the PSRCHIVE software, please do not hesitate to contact
us.

---

## Configuration

The `configure` script can fail for a number of reasons:

### Compiler does not support C++17

If the configure script aborts with an error message like the following:

```
configure: error: *** A compiler with support for C++17 language features is required.
```

then double-check that the C++ standard is not hard-coded in your CXXFLAGS environment variable; e.g.,

```
$ echo $CXXFLAGS
-mtune=native -O2 -ffast-math -pthread -fPIC -std=c++14
```

### Third-party package not found

In addition to the standard installation directories
(e.g. /, /usr, /usr/local) the
`configure` script will also search for third-party
packages in $PSRHOME/packages/$LOGIN\_ARCH. If your
third-party software is located in a different directory, this can
be specified using the relevant command line option, e.g.
`--with-cfitsio-dir=DIR`. Type `configure --help`
for the full list of options.

### Different C++ compiler used to compile third-party package

Because different C++ compilers (including different versions of
`g++`) mangle names differently, you must ensure that the same C++
compiler is used to compile PSRCHIVE
that was used to compile Qt, for example. To specify the C++
compiler, set the CXX environment variable before
running `configure`.

### Different Fortran compiler used to compile a third-party package

Because different Fortran compilers make use of different
libraries during linking, you must ensure that the same Fortran
compiler is used to compile PSRCHIVE that was used to compile, for
example, SLALIB. To specify the Fortran compiler, set the
F77 environment variable before running
`configure`.

### SIGPROC `find` is first in `$PATH`

If you have the SIGPROC (Pulsar) Signal Processing Programs
installed on your system, then it is possible that the
`find` that searches for files in a directory hierarchy is
over-shadowed by SIGPROC `find`, which searches for pulsars
in observed timeseries. Please ensure that your PATH
environment variable is set so that the unix `find` program
is found first.

---

## Compilation

`make` can fail for a number of reasons:

### Undefined references to std c++ objects like `cerr` and `endl`

If the version of `g77` does not match that of `g++`
then the flags added in order to link against the Fortran
libraries will mess up `g++` during the linking stage.
Please ensure that `g77` and `g++` have the same
version number.

