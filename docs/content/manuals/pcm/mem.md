# PSRCHIVE documentation: pcm

## PSRCHIVE user documentation: `pcm`

### Measurement Equation Modeling Example

For this example, suppose that all of the pulsar observations are
located in the current working directory and have the extension
.ar and that all of your calibrator archives are summarized
in the data base, $MYCALS/database.txt.

The simplest way to run `pcm` is to let it choose the optimal
phase bins to use as model constraints. Assuming that calib.TT
has been created as described in Section 2.1, run

```
pcm -d $MYCALS/database.txt -c calib.TT -s -t2 *.ar
```

Here, the `-s` option specifies that the Stokes parameters
should be normalized to compensate for temporal flux variations and
the `-t2` option specifies to use two processing threads
(not useful if your machine has only one single-core CPU).

### Manaual selection of phase bins

Alternatively, it is also possible to specify the phase bins to be
used as constraints. When plotting data to support phase bin selection,
be sure to dedisperse and centre the data; e.g.

```
psrplot -j "dedisperse, centre 0" -p S -jF calib.TT
```

Suppose that the Stokes parameters at pulse phases equal to 0.45
and 0.51 look like they will make good constraints. If there are 2048
phase bins, then bin numbers 922 and 1044 are to be used. The command
line is:

```
pcm -b 922 -b 1044 -d $MYCALS/database.txt *.ar
```

Alternatively, suppose that you would like to select 8 bins between
pulse phases 0.39 and 0.51. The command line is:

```
pcm -p 0.39,0.51 -n 8 -d $MYCALS/database.txt *.ar
```

---

In the latter example, `pcm` will begin with something like:

```
Pulsar::Archive::Agent::init 
pcm: selecting input states from 0.39 to 0.51
pcm: selecting a maximum of 8 bins
pcm: assuming that System + Hydra A Stokes V = 0
pcm: allowing CAL Stokes Q to vary
pcm: normalizing Stokes parameters by invariant interval
pcm: constructing Calibration::Database from
     /export/home/user/pcm/cals/database.txt
pcm: searching for calibrator observations within 24 hours of midtime
pcm: midtime = 2003-08-29-19:02:00
pcm: adding n2003241194914.pcal
pcm: adding n2003241223100.fcal
pcm: adding n2003241222635.fcal
pcm: set calibrators
pcm: loading archives
pcm: adding phase bin 799
[...]
pcm: adding phase bin 1044
pcm: 8 states
Pulsar::ReceptionCalibrator::load_calibrators 
  loading /export/home/user/pcm/cals/n2003241194914.pcal
```

Note that `pcm` begins by loading the calibrators. At this
stage, you may see error messages like:

```
Pulsar::ReceptionCalibrator::add_calibrator error adding ichan=0
Error::stack
        Calibration::ReceptionModel::add_data
Error::InvalidParam
Error::message
        source_index=0 ipol=3 variance=0.000000 < 0
```

Or also:

```
Pulsar::ReceptionCalibrator::add_data discarding ichan=0 ibin=1044
```

Unless all of the data are getting discarded, or appear to be
corrupted, these error messages may be ignored; `pcm`
can handle and flag a variety of problems in the data.

After the calibrators have been loaded, `pcm` will
output on stderr lines such as:

```
Setting 128 channel receiver
Pulsar::ReceptionCalibrator::precalibrate
pcm: loaded archive: n2003200181319.cfb
```

These mean that `pcm` is calibrating the input archives
with its current best guess of the receiver before adding their data
to the best guess of the input polarizations.

After all the data is loaded, `pcm` will output something like:

```
pcm: plotting initial guess of receiver
Pulsar::ReceptionCalibrator::solve information:
  Parallactic angle ranges from -102.621536254883 to 98.1568908691406 degrees
Pulsar::ReceptionCalibrator::solve reference epoch: 52839.945158016725522
pcm: plotting uncalibrated pulsar total stokes
pcm: plotting uncalibrated CAL
pcm: plotting pulsar constraints
pcm: nstate=1
ichan=1 istate=1
pcm: nstate=1
ichan=11 istate=1
pcm: nstate=1
ichan=21 istate=1

[...]
```

At this stage, `pcm` creates a number of postscript
files: guess.ps and
channel\_\*.ps.
In guess.ps is plotted the first guess of the
instrumental response and the Stokes parameters of both the calibrator
the pulsar. In channel\_\*.ps are plotted the
variations of the observed Stokes parameters as a function of
parallactic angle in a selection of up to 13 frequency channels, which
are evenly spaced across the band.

Next, `pcm` will output lines like:

```
pcm: solving model
Pulsar::ReceptionCalibrator::solve ichan=0
Pulsar::ReceptionCalibrator::solve failure ichan=0
Error::stack
        Calibration::ReceptionModel::solve
Error::InvalidRange
Error::message
        input source 0 with free parameter(s) not observed

Pulsar::ReceptionCalibrator::solve ichan=1
Calibration::ReceptionModel::solve converged in 53 iterations. 
  chi_sq=785.590026855469/406=1.93495082855225
Pulsar::ReceptionCalibrator::solve ichan=2
Calibration::ReceptionModel::solve converged in 40 iterations.
  chi_sq=739.773132324219/406=1.822101354599
Pulsar::ReceptionCalibrator::solve ichan=3
Calibration::ReceptionModel::solve converged in 30 iterations.
  chi_sq=737.575866699219/406=1.81668937206268

[...]

Pulsar::ReceptionCalibrator::solve ichan=36
Calibration::ReceptionModel::solve converged in 5 iterations.
  chi_sq=593.349975585938/406=1.4614531993866
Pulsar::ReceptionCalibrator::solve ichan=37
Calibration::ReceptionModel::solve converged in 6 iterations.
  chi_sq=626.255004882812/406=1.54250001907349
Pulsar::ReceptionCalibrator::solve ichan=38
Calibration::ReceptionModel::solve converged in 5 iterations.
  chi_sq=614.626037597656/406=1.51385724544525
```

Note the error message that arises because channel 0 was corrupted in
all of the calibrator archives (the PolnCal Stokes parameters are
assigned an input source index of zero). Note also that corruption
due to aliasing during downconversion causes slower convergence and a
larger reduced chi squared near the leading edge of the passband
(which corresponds to the lowest values of ichan shown above). On a
1.2 GHz Pentium III, the solution takes approximately:

* less than a minute with one pulse phase bin* about an hour with 16 pulse phase bins* about 19 hours with 64 pulse phase bins

- When all of the frequency channels have been fitted,
  `pcm` will output the solution in the FEEDPAR HDU of a
  PSRFITS file named pcm.fits. It will also
  over-write channel\_\*.ps by plotting the same
  data with the best fit model curves drawn through the data points. It
  will also output a new file, result.ps, which
  plots the best-fit model parameters that describe the instrumental
  response and the Stokes parameters of the flux calibrator and noise
  diode.
  `pcm` then re-calibrates all of the Pulsar, PolnCal, and
  FluxCalOn files, producing new files with a .calib
  extension. Finally, the mean of the calibrated pulsar data is output
  in the file, total.ar and the fscrunched and tscrunched
  calibrated pulse profile is plotted in calibrated.ps.

  Copyright © 2006-2019 Willem van Straten
  Licensed under the Academic Free License version 2.1
