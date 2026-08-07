# TEMPO warnings

PSRCHIVE makes use of tempo to generate
polynomials that approximate the relationship between apparent
topocentric pulse phase and time. The accuracy of this approximation
depends upon the length of time spanned by each polynomial,
`TZNSPAN`, and the number of coefficients,
`TZNCOEF`.

If a PSRCHIVE program issues a warning like

```
Tempo::Predict::generate WARNING rms=0.411525 microseconds
Tempo::Predict::generate https://psrchive.readthedocs.io/en/latest/content/warnings/tempo/
```

then is it is likely that either `TZNSPAN` should be
decreased or `TZNCOEF` should be increased. These
parameters can be set in the input ephemeris file; e.g.

```
PSRJ           1909-3744
RAJ            19:09:47.4379988
DECJ           -37:44:14.31816
F0             339.31569275868954208
F1             -1.6147667945018354534e-15
DM             10.393214975272748697
[...]
TZRMJD         53477.971476731175557       
TZRFRQ         1400
TZNSPAN        120
TZNCOEF        15
```

Note that the span of the polyco is specified in minutes, and the
minimum value of `TZNSPAN` accepted by tempo is 60. The
updated ephemeris can be installed with

```
pam -E updated.eph -e new filename.ar
```
