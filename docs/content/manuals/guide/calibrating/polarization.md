# Polarimetric Calibration

Before starting, please read [Section 1.1:
Supported Models](label-pac-supported) of the `pac` manual.

## Step One: obtain and view calibrator data

To perform the most rudimentary (and in most cases incorrect) form of
calibration, you will need at least one observation of an artificial
calibrator, known as the reference source, or noise diode, or CAL.
Please see [Appendix A: Calibrator observations](label-pac-appendix) 
for more details about the instrumental setup and format of CAL observations.

When calibrator data are obtained, confirm that the observations
look as expected by
[viewing the calibrator data](../viewing/cal/index).

The data files used in the above and following examples
are available here:

* {download}`CAL.ar </_static/CAL.ar>`.
* {download}`PSR.ar </_static/PSR.ar>`.

## Step Two: clean data and prepare a calibrator database

The previous step may have highlighted problems in the data, such
as strong radio frequency interference or regions of the spectrum
corrupted by digitization artifacts.

Corrupted frequency bands can be removed from the calibrator using
[paz](../../paz). For example, in the
[sample data](../viewing/cal/index) from the previous step,
the band edges are clearly depolarized. These channels can be flagged
invalid with a command like

```
paz -Z 0-15 -Z 120-127 -m CAL.ar
Updating CAL.ar ... done
```

Note that when frequency channels are flagged as invalid in a calibrator
file, any pulsar observation calibrated using this calibrator data will
have the same frequency channels flagged as invalid (in addition to any
previously flagged frequency channels).

After all invalid data have been flagged, a calibrator database can be
created as described in the
[pac reference manual](label-pac-database).

## Step Three: calibrate the pulsar observations

For example:

```
pac -d database.txt PSR.ar 
pac: Reading from database summary file

pac: Loaded archive PSR.ar
pac: PolnCalibrator constructed from:
        /full/path/to/CAL.ar
pac: Poln calibration complete
pac: Could not flux calibrate PSR.ar
        no match found
pac: Calibrated archive PSR.calibP unloaded

pac: Finished all files
```

Unless absolute flux calibration is required, the "Could not flux
calibrate ..." warning can be ignored. The calibrated pulse profile
can be plotted with

```
psrplot -pS -jF PSR.calibP
```
