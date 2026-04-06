# fluxcal

### 1.0 Purpose

The `fluxcal` program uses observations of the reference
source and a standard candle in order to determine both the system
temperature and the flux density of the reference source. The
solutions produced by `fluxcal` are used by [`pac`](manuals/fluxcal/pac.md) to perform absolute flux
calibration of pulsar observations. Absolute flux calibration must be
preceded by a stage of polarimetric calibration that normalizes the
observed flux density by the flux density of the reference source.

### 2.0 Usage

### 2.1 Before running `fluxcal`

#### The Observations

In order to run `fluxcal`, you will need sets of
`Pulsar::Archive` files containing observations of the
reference source (as described in Appendix A of the  [`pac`](manuals/fluxcal/pac.md) documentation). A set of flux
calibrator observations must include at least one of each of the
following:

1. `FluxCalOn` - An observation of the
   reference source made while the antenna is directed at a standard
   candle. (`Pulsar::Archive::get_type` must
   return `Signal::FluxCalOn`).- `FluxCalOff` - An observation of the reference
     source made while the antenna is pointing at least a full beam
     width away from the standard candle.
     (`Pulsar::Archive::get_type` must return
     `Signal::FluxCalOff`).

The observations in each set must have the same observing parameters,
such as centre frequency and bandwidth, as determined by the
`Pulsar::Archive::calibrator_match` method.

#### The Standard Candles

The `fluxcal` program also requires information about the
standard candle(s) to be used for absolute flux calibration. This
information is parsed from a text file (by default,
$prefix/share/fluxcal.cfg) which includes a row
for each flux calibrator. The columns in this file are:

1. source name- right ascension- declination- reference frequency (MHz)- reference flux density (Jy)- spectral index

Following each row, any number of aliases for the source name may be
specified using the aka keyword. An example
input file is:

```
# NAME     R.Ascension  Declination     Freq    Flux    spectral
#          (J2000)      (J2000)         (MHz)   (Jy)    index

0407-658   04:08:20.380 -65:45:09.078   1400.0  14.4    1.189

3C353      17:20:28.15  -00:58:46.8     1400.0  57.3    0.76

3C274      12:30:49.423 +12:23:28.043   1400.0  200.    0.81
aka Virgo
aka B1228+126

3C218      09:18:05.669 -12:05:43.95    1400.0  43.1    0.91
aka Hydra
aka J0918-1205
aka CAL
```

### 2.2 Running `fluxcal`

#### Input

There are two modes of operation, as determined by the input method:

1. From File: In this mode of operation, flux calibrator observation
   filenames are specified on the command line,
   `fluxcal` sorts the files by name and, as they are
   loaded, breaks the list up into sets. In order for the solutions
   to be used, the calibrator database text file must later be
   regenerated using [`pac`](manuals/fluxcal/pac.md).- With Database: In this mode of operation, a calibrator database
     text file, as produced by [`pac`](manuals/fluxcal/pac.md),
     is provided using the -d filename command
     line option; `fluxcal` sorts the observations by
     the MJD of each database entry and, as they are loaded, breaks
     the list up into sets. For each new flux calibration solution
     successfully written to disk, `fluxcal` will add a
     new entry to the database. The updated database text file will
     be written at the end of operation.

A flux calibrator set is defined by the time elapsed between the end
of the last observation in the set and the start of the first
observation in the next set. This separation time is 5 minutes by
default and can be changed using the `-i` command line
option.

#### Output

For each set, `fluxcal` solves for the system temperature and
reference flux density, and outputs these values as a function of
radio frequency into the FLUXCAL binary table extension of a PSRFITS
file. If another file format can be used to store the
`FluxCalibratorExtension` data, this file format can
optionally be specified using the `-a` command line option.

The solution is written to a file with the same name as the first
`FluxCalOn` observation in the set, with
the extension replaced by .fluxcal. To change
the extension, use the `-e` command line option.

The solutions produced by `fluxcal` may be plotted by
running `pacv filename.fluxcal`.

### 3.0 Algorithms

The solution implemented by `fluxcal` is described in
the [appendix](../appendices/fluxcal/fluxcal.pdf).

### 4.0 Testing and examples

The following simple example shows a successful run. Notice that the
output filename need not necessarily be based upon the first file
loaded. Rather, it is based on the first `FluxCalOn` observation:

```
> fluxcal w030814_*.cf

Pulsar::Archive::Agent::init 
fluxcal: loading w030814_021414.cf
fluxcal: starting new FluxCalibrator
fluxcal: loading w030814_021709.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w030814_022009.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w030814_022310.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w030814_022610.cf
fluxcal: observation added to FluxCalibrator
fluxcal: creating PSRFITS Archive
fluxcal: unloading w030814_021709.fluxcal
```

The following example illustrates the automatic division of observations
into sets, and includes an error condition that arises when an incomplete
set is encountered:

```
> fluxcal w031029_*.cf

Pulsar::Archive::Agent::init 
fluxcal: loading w031029_232004.cf
fluxcal: starting new FluxCalibrator
fluxcal: loading w031029_232252.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w031029_232532.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w031029_232816.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w031029_233106.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w031029_234326.cf
fluxcal: gap=10.3min > interval=5.0min
fluxcal: creating PSRFITS Archive
fluxcal: unloading w031029_232252.fluxcal
fluxcal: starting new FluxCalibrator
fluxcal: loading w031029_235143.cf
fluxcal: gap=6.2min > interval=5.0min
fluxcal: creating PSRFITS Archive
fluxcal: error unloading solution
        no FluxCal-On data
fluxcal: starting new FluxCalibrator
fluxcal: creating PSRFITS Archive
fluxcal: error unloading solution
        no FluxCal-On data
```

In the following example, the square wave form of the pulsed reference
source (e.g. linear noise diode) is not detected.

```
> fluxcal w040219_*.cf

Pulsar::Archive::Agent::init 
fluxcal: loading w040219_164032.cf
fluxcal: starting new FluxCalibrator
fluxcal: loading w040219_164326.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040219_164624.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040219_164927.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040219_165227.cf
fluxcal: observation added to FluxCalibrator
fluxcal: creating PSRFITS Archive
fluxcal: error unloading solution
        Flux calibrator not detected
```

In the following example, a new set of flux calibrator observations
follows the previous set within the five minute separation interval,
but with a different configuration. The change in configuration is
noticed, the first solution is written to disk, and a new flux
calibrator set is started.

```
> fluxcal w040509_*.cf

Pulsar::Archive::Agent::init 
fluxcal: loading w040509_085200.cf
fluxcal: starting new FluxCalibrator
fluxcal: loading w040509_085515.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040509_085838.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040509_090157.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040509_090518.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040509_091148.cf
fluxcal: failed to add observation
        mismatch between
        w040509_085515.cf and
        w040509_091148.cf
        centre frequency mismatch: 1433.000000 and 3100.000000
        bandwidth mismatch: -256.000000 and -512.000000
fluxcal: creating PSRFITS Archive 
fluxcal: unloading w040509_085515.fluxcal
fluxcal: starting new FluxCalibrator
fluxcal: loading w040509_091501.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040509_091814.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040509_092135.cf
fluxcal: observation added to FluxCalibrator
fluxcal: loading w040509_092457.cf
fluxcal: observation added to FluxCalibrator
fluxcal: creating PSRFITS Archive
fluxcal: unloading w040509_091501.fluxcal
```

### 5.0 Known bugs and features that require implementation

None at this time.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
