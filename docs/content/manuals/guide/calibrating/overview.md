# PSRCHIVE User's Guide: Overview of Calibration

The progam [pac](../../pac.md) calibrates pulsar data using
available combinations of:

1. polarimetric calibrator observations as described in
   [Appendix A](../../pac.html#observations) of the
   [pac manual](../../pac.md).- flux calibration solutions as output by
     [fluxcal](../../fluxcal.md);- a reception model (including cross-coupling parameters) as output by
       [pcm](../../pcm.md) [required only when the `pac -S`
       command line option is used]

`pcm` is used to produce a reception model, but it will also
output calibrated versions of its input files (which should be
uncalibrated). These calibrated outputs are useful as diagnostics,
but they will also be calibrated using a model that includes any time
variations (e.g. if the `pcm -u` or `-o` options are
used). If these options are used, and a better fit is obtained, then
the calibrated output from `pcm` will be better than that
output by `pac`, but only for the data set used as input; the
time variations do not apply to any other pulsar and they are not
stored in the solution output by pcm (a PSRFITS file named
pcm.fits).

If a pcm.fits solution has been derived from other data,
and it has the same instrumental parameters listed in
database.txt, then `pac` can apply the model
stored in pcm.fits to calibrate the pulsar data. It is not
necessary to use pcm **to calibrate** data (unless time variations
produce a better fit to the data, and the data to be calibrated are
those used as input to pcm, as mentioned above).

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
