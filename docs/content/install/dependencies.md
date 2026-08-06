# Dependencies

If PSRCHIVE is not finding a package that is installed on your system, please see the [Package Detection](third/packages.md) notes.

To automatically install any dependencies, see [Automatic Package Installation](third/install.md).

```{toctree}
:hidden:
:maxdepth: 2

third/packages.md
third/install.md
```

---

## Necessary

These packages provide basic functionality and must be installed
before attempting to compile or install PSRCHIVE.

* A Fortran, C, and C++ compiler; for example, the
  [GNU Compiler Collection](http://gcc.gnu.org/) (GCC).
  PSRCHIVE currently compiles using
  GCC release series 3 and 4.
* A Fast Fourier Transform (FFT) library, such as a single-precision version
  of the
  [Fastest Fourier Transform in the West](http://www.fftw.org/)
  (FFTW).
  Please see our [Download and Installation](third/fftw.md) instructions.
  (PSRCHIVE can also make use of several
  vendor-supplied FFT libraries.)
* **Mac users only** may need to install some
  [additional packages](third/mac.md).
* There are also some notes on installing PSRCHIVE on [Ubuntu running on Windows 10](https://docs.google.com/document/d/1k61ya7rOvUKfWppfZib3Sed1zOvL6_2MeHp6j9LdWog/edit?usp=sharing).

```{toctree}
:hidden:
:maxdepth: 2

third/fftw.md
third/mac.md
```
---

## Optional

PSRCHIVE can compile without these packages,
but its subsequent use may be limited.

* [TEMPO2](http://www.atnf.csiro.au/research/pulsar/tempo2)
  pulsar timing data analysis software, maintained by George Hobbs
  at the Australia Telescope National Facility.
  PSRCHIVE may require TEMPO2 in order to
  handle PSRFITS files that make use of the TEMPO2 Predictor
  to model pulse phase.
  Please see our [Download and Installation](third/tempo2.md) instructions.
* [TEMPO](http://pulsar.princeton.edu/tempo/index.md)
  pulsar timing data analysis software, maintained at Princeton University
  and the Australia Telescope National Facility.
  PSRCHIVE utilizes TEMPO to generate the
  polynomials used to predict pulse phase.
* The [NASA CFITSIO](http://heasarc.gsfc.nasa.gov/docs/software/fitsio/fitsio.md) subroutine library. This is necessary to
  compile the PSRFITS plugin. PSRFITS is the
  format used by the ATNF Wideband Correlator at Parkes.
* PGPLOT Graphics Subroutine Library. PGPLOT is used to prepare
  publication-quality plots. Please see our [Download
  and Installation](third/pgplot.md) instructions.

```{toctree}
:hidden:
:maxdepth: 2

third/tempo2.md
third/pgplot.md
```

---

## Build tools

If you wish to check out and compile the code from the Git repository,
you will need the following:

* The [Git](https://git-scm.com/) distributed version control system.
* The [GNU autotools](third/autotools.md).

```{toctree}
:hidden:
:maxdepth: 2

third/autotools.md
```