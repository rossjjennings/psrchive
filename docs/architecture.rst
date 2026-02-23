Architecture
============

PSRCHIVE is organized in three tiers, with strict dependency direction
from top to bottom.

.. code-block:: text

   More/       Advanced analysis (libpsrmore.la)
     ├── MEAL/          Measurement equation algebra
     ├── Polarimetry/   Mueller matrices, Stokes calibration
     ├── Timing/        TOA generation, pulsar timing
     ├── RFIMitigation/ RFI removal algorithms
     ├── Plotting/      PGPLOT visualization (conditional)
     └── Applications/  Tools: pam, vap, psrsh, paz, pat
               ↓
   Base/       Core data structures (libpsrbase.la)
     ├── Classes/       Archive, Integration, Profile
     ├── Extensions/    Calibrator, Receiver, Backend metadata
     ├── Formats/       12+ file formats (PSRFITS, WAPP, BPP, Timer, …)
     └── Applications/  Tools: psredit, psrconv
               ↓
   Util/       Low-level utilities (libpsrutil.la)
     ├── epsic/         Math library (git submodule)
     ├── fft/           FFT wrappers (FFTW3, MKL, IPP)
     ├── tempo/tempo2/  Pulsar ephemeris interface
     └── units/         TextInterface dynamic configuration

Core data model
---------------

``Pulsar::Archive``
   Top-level observation container.  Holds metadata (source name,
   coordinates, bandwidth, centre frequency) and a sequence of
   ``Integration`` objects.

``Pulsar::Integration``
   A single time-integrated sub-integration.  Contains a 2-D array of
   ``Profile`` objects indexed by polarization and frequency channel.

``Pulsar::Profile``
   A one-dimensional array of amplitudes as a function of pulse phase
   (the folded pulse profile).

Container and algorithm interfaces
-----------------------------------

All three core classes inherit from ``Pulsar::Container``, which defines
a minimal set of operations: offset, scale, rotate, bscrunch, fold.
Higher-level operations are implemented as ``Pulsar::Algorithm`` objects
that accept containers as input.

File format support
-------------------

``Pulsar::Archive`` is a virtual base class.  Concrete subclasses
implement I/O for specific file formats (PSRFITS, Timer, WAPP, BPP,
etc.).  New instances are created via the ``Pulsar::Archive::load``
static factory, which auto-detects the format.

MEAL library
------------

The Measurement and Error Analysis Library (MEAL) implements non-linear
least-squares fitting and error propagation.  Arbitrary functions are
built by modular construction of elementary functions, each
parameterized by any number of arguments.  MEAL is used extensively by
the Polarimetry module for calibration model fitting.
