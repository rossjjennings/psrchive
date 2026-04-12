# Package Detection

PSRCHIVE can be directed where to find third-party
software packages using one or more of the following environment variables:

* PKG\_CONFIG\_PATH : a colon-separated list of paths searched
  by [`pkg-config`](https://www.freedesktop.org/wiki/Software/pkg-config/),
  which is used to find the compiler and linker flags for third-party dependencies
  including armadillo, cfitsio, fftw, healpix, and yaml-cpp.
* PGPLOT\_DIR : full path to the directory in which the
  PGPLOT libraries can be found
* TEMPO2 : full path to the directory in which the
  TEMPO2 libraries can be found
* PACKAGES : a space-separated list of absolute paths to base directories
  containing third-party packages that do not publish compilation information using `pkg-config`,
  including eigen, ipp, mkl, and splinter.

---

### Using the PACKAGES environment variable

You can specify one or more root directories in which to search for
packages using the PACKAGES environment variable.
PSRCHIVE will search the sub-directories
for the required pacakges.
For example, suppose that you have

* Splinter installed in /home/pulsar/packages
* Intel Performance Primitives installed in /usr/local/intel/ipp/5.1/em64t

Simply

```
setenv PACKAGES "/home/pulsar/packages /usr/local/intel"
```

before running the `configure` script.

---

### Using `configure` script options

The above example could also have been handled with

```
./configure --with-splinter-dir=/home/pulsar/packages \
  --with-ipp-dir=/usr/local/intel/ipp/5.1/em64t
```

For a complete list of `--with`  options, type
`configure --help`
