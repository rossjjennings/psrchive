Installation
============

Prerequisites
-------------

**Required:**

- C++11 compiler (GCC, Clang)
- FFT library: FFTW3, Intel MKL, or Intel IPP

**Optional:**

- CFITSIO (for PSRFITS file format support)
- TEMPO2 (for pulsar timing)
- PGPLOT (for plotting)
- SWIG + Python development headers (for Python bindings)

Building from source
--------------------

.. code-block:: bash

   git clone https://gitlab.com/pulsar-astronomy/psrchive.git
   cd psrchive
   git submodule update --init      # Initialize epsic math library
   ./bootstrap
   ./configure --prefix=/usr/local
   make
   make check                       # Run tests
   make install

Configure options
-----------------

``--enable-lean``
   Disables MEAL, Polarimetry, Timing, RFIMitigation, and Plotting modules.
   Useful for minimal installations that only need core I/O functionality.

``--enable-shared``
   Required for building Python bindings via SWIG.

``--prefix=<path>``
   Installation prefix (default ``/usr/local``).

Dependency auto-install
-----------------------

Scripts in the ``packages/`` directory can automatically download and build
common dependencies:

.. code-block:: bash

   cd packages
   ./fftw3.csh         # FFTW3
   ./cfitsio.csh       # CFITSIO
   ./tempo2.csh        # TEMPO2

Running tests
-------------

.. code-block:: bash

   make check                       # All tests
   cd More/Tests && make check      # Analysis tests only
   cd Base/Classes && make check    # Core class tests
