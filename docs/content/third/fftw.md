# FFTW: Fastest Fourier Transform in the West

PSRCHIVE requires a single-precision version of
the FFTW library to perform the Fast Fourier Transform. FFTW is freely
available at [fftw.org](http://fftw.org) and licensed under
the [GNU GPL](http://www.gnu.org/copyleft/gpl.md).

### Download

FFTW version 3.1 is available at the [FFTW download](http://fftw.org/download.md) page.

### Install

It is important to configure FFTW to build the single-precision
version of the library using the `--enable-float` option to
the `configure` script.
For example, after downloading the archive, run:

```
gunzip -c fftw-3.1.2.tar.gz | tar xvf -
cd fftw-3.1.2
./configure --enable-float
make
make install
```

### Group Installation

For a group installation, set the installation directory
with the `--prefix` option; e.g.

```
./configure --enable-float --prefix=$PSRHOME/packages/$LOGIN_ARCH
```
