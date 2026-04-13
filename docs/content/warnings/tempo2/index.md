# TEMPO2 Warnings

PSRCHIVE can make use of tempo2 to generate and
use Chebyshev polynomials that predict apparent topocentric pulse
phase as a function of time and radio frequency. This functionality
is optional, and will be compiled into PSRCHIVE
only if the tempo2 library is found when the configure script is run
during installation.

If a PSRCHIVE program issues a warning like

```
Pulsar::FITSArchive does not support tempo2 predictors
Pulsar::FITSArchive https://psrchive.readthedocs.io/en/latest/content/warnings/tempo2/
```

then the PSRFITS file from which data are being loaded contains a
T2PREDICT extension and tempo2 support is required. Please download
tempo2 from the ATNF and install this package, then re-install PSRCHIVE. If the tempo2 package is found, the
following line (among many) will be output by the `configure` 
script:

```
checking for TEMPO2 Predictor library... yes
```

If tempo2 is not found, the following warning will be output upon
completion of the `configure`  script:

```
The TEMPO2 Predictor library was not found:
  PSRCHIVE applications will not be able to use TEMPO2 predictors.
```
