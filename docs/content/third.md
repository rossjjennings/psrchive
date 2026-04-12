# Third Party Software

New users should note that if you are installing PSRCHIVE at a
facility that already has the required third party packages, then you can skip this section.

If PSRCHIVE is not finding a package that
is installed on your system, please see the
[Package Detection](third/packages.md) notes.

---

### Necessary

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

---

### Optional

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

---

### For Git Access

If you wish to check out and compile the code from the Git repository,
you will need the following:

* The [Git](https://git-scm.com/) distributed version control system.
* The [GNU autotools](third/autotools.md).

---

### Group Installation Notes

It is often useful to install third party packages in a place that can
be accessed and used by everyone in your research group. At the ATNF
and Swinburne University, pulsar group software is installed in an NFS
mounted disk named by the PSRHOME environment variable. The
LOGIN\_ARCH environment variable is used to further qualify
the location of architecture-dependent executables and data.

As an example, assume that the PSRHOME environment variable is
set to /psr and the LOGIN\_ARCH environment variable is
set to linux. Third party packages should be installed
in the directory /psr/packages/linux.
