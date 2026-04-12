# pcm

### 1.0 Purpose

The Polarization Calibration Modeling (`pcm`) program may be
used to estimate the polarimetric response of the observing system, as
described in [van Straten 2004, ApJS 152:129](http://adsabs.harvard.edu/abs/2004ApJS..152..129V),
and [van Straten 2013, ApJS 204:13](http://adsabs.harvard.edu/abs/2013ApJS..204...13V).
There are two different modes of operation:

**Measurement Equation Modeling (MEM)**, described in
van Straten (2004), uses **uncalibrated** observations of

* a single pulsar at multiple parallactic angles,
* an amplitude modulated reference source (linear noise diode), and
* [optionally] an unpolarized flux calibrator, such as Hydra A.

Please read this imporant note about assumptions made to overcome
[measurement equation modeling degeneracy](pcm/degeneracy.md).

**Measurement Equation Template Matching (METM)**, described in
van Straten (2013), uses

* a single well-calibrated observation of a pulsar with high
  signal-to-noise ratio (the template, or standard),
* one or more uncalibrated observations of the same pulsar, and
* [optionally] an amplitude-modulated reference source
  (e.g. a noise diode coupled to the receptors).

It is also possible simultaneously model multiple pulsars
in order to map the two-dimensional response of the instrument.
Please see [Modeling variations of instrumental response across the beam](pcm/beam.md)
for more information.

### 2.0 Usage

#### 2.1 Before running `pcm`

In order to run `pcm` in MEM mode, you will need
Pulsar::Archive files containing:

* full polarization observations of a single pulsar over a wide
  range in parallactic angles
  (`Pulsar::Archive::get_type` must
  return `Signal::Pulsar`).
* one or more observations of the "linear" noise diode (CAL) made
  while pointing at or near the pulsar
  (`Pulsar::Archive::get_type` must
  return `Signal::PolnCal`).
* [optionally] one or more observations of the CAL made while
  pointing at a strong flux calibrator with negligibly small degree of
  circular polarization, such as Hydra A
  (`Pulsar::Archive::get_type` must return
  `Signal::FluxCalOn`).

All observations must have the same observing parameters, such as
centre frequency and bandwidth, as defined by the
`Pulsar::Archive::calibrator_match` method. After
gathering all of the required data:

* Create a calibrator directory and database file:
  1. Copy all calibrator files (.pcal, .fcal) to one directory,
     say $MYCALS. The files can be organized
     into sub-directories if desired (see the `pac` manual).
  2. Run `pac -wp $MYCALS` to create the summary file,
     $MYCALS/database.txt.
* Create a preliminary total integrated calibrated profile:
  1. Perform preliminary calibration using
     `pac -d $MYCALS/database.txt psr*.ar`
  2. Combine the calibrated output archives using
     `psradd -T -o calib.TT psr*.calib`

This last step produces a total integrated profile from which to
choose the best pulse phase bins to be used as model constraints. It
is best to choose these from a high S/N profile, but integration in
both time and frequency will lead to depolarization (primarily due to
the variation of differential phase as a function of frequency, and to
the time-varying projection of the receptors onto the sky). The
preliminary calibration step minimizes this depolarization.

As described in more detail in [Section 4.1 MEM Example](pcm/mem.md), it is possible to either
manually choose individual phase bins or ranges of pulse phase that
make good model constraints, or allow `pcm` to
[automatically choose the best pulse phase bins](pcm/choose.md).
When choosing phase bins manually, it is best
to choose those with the highest polarized flux density and, if
selecting more than one pulse phase bin, to choose states that have
orthogonal polarization vectors in the Poincare space.

As more phase bins are used as constraints, more time will be required
to converge to a solution. For starters, use only four phase bins;
`pcm` will converge more quickly, providing a first order
estimate of the instrumental parameters. More phase bins can then be
added to descrease the experimental uncertainty of the results.

#### 2.2 Running `pcm`

##### 2.2.1 MEM mode

In MEM mode, `pcm` does the following (not necessarily in this order):

* Loads all `PolnCal` calibrator files and solves the
  receiver using one of the basic models (`SingleAxis` or
  `Polar`, depending upon the chosen parameterization,
  `Britton` or `Hamaker`). The weighted mean of
  these solutions is used as the first guess of the instrumental
  response.
* Loads the pulsar archive and prunes the Stokes parameters from
  the specified set of pulse phase bins. These are corrected using the
  best guess of the instrumental response, deparallactified, and added
  to a weighted mean that is used as the first guess of the Stokes
  parameters.
* Loads all `FluxCalOn` flux calibrator files and uses
  the off-pulse Stokes parameters to constrain the mixing of Stokes I
  and V. This breaks the degeneracy described in van Straten (2004).

[An example of MEM usage.](pcm/mem.md)

##### 2.2.2 METM mode

When run with the `-S standard.ar`
option, `pcm` performs measurement equation template
matching, a global fit in which the transformation between the
well-calibrated template in `standard.ar` and the
pulsar observations specified on the command is modellded. Calibrator
observations are also incorporated as constraints.
With the `-1` option, an unique fit if performed on each
input sub-integration and a separate file is output for each solution
(with the extension .mtm).

[An example of METM usage.](pcm/metm.md) Note that this
technique can be used to estimate the differential Faraday rotation
between the template and the observation.

#### 2.3 Diagnostic outputs

**pcm** can optionally produce a number of diagnostic outputs.
These are enabled with the `-D name`  command line option,
where *name* is any one of:

* `prefit`  for each frequency channel, print the initial
  state of the model that is to be fit to the data, including fit flags
  and initial guesses for each parameter
* `report`  print a report of the reduced chisq for each
  Stokes parameter of each pulse phase bin in pcm\_report\_\*.txt.
  Please see [fit report](pcm/report.md) for more details.
* `data` 
  for each pulsar and each frequency channel, print the input data
  (Stokes parameters and errors) and the model evaluated for each datum.
  Please see [data and model](pcm/data.md) for more details.
* `guess`  plot the initial guess of the instrumental
  response in guess\_response.ps, the calibrator Stokes
  parameters in guess\_cal.ps, and the pulsar Stokes parameters
  in guess\_psr.ps.
* `residual`  plot the residual between each measurement
  and the model in channel\_\*.ps.
* `total`  plot the total integrated
  pulse profile in uncalibrated.ps  (before) and
  calibrated.ps  (after).
* `result`  plot the best fit result of the instrumental
  response in result\_response.ps, the calibrator Stokes
  parameters in result\_cal.ps, and the pulsar Stokes parameters
  in result\_psr.ps.

#### 2.4 Modeling time variations

 **Intrinsic variations (including interstellar scintillation)** 

**pcm** can compensate for intrinsic variations in the
intensity of the pulsar signal by normalizing the Stokes parameters of
the pulsar observations by the mean invariant: the square root of the
mean of the invariant intervals (I^2 - p^2) of a constant selection of
on-pulse phase bins.

This feature is enabled with the -s command line option. The
on-pulse phase bins are chosen from either the first archive loaded or
the archive specified using the -c command line option, then
held fixed for all subsequent archives.

When -s is used, the pulsar is put through a signal path with
constant gain set to unity, and the free gain parameter is applied
only to the calibrator signal path.

**Instrumental variations**

**pcm** can model the observations with an instrumental
backend that varies over time. Each of the absolute gain,
differential gain, or differential phase may be independently
represented as either

* a polynomial of arbitrary order; or
* a series of step functions (jumps).

For the series of steps, the epoch of each step is defined by the
first sub-integration of each CAL file encountered. This models the
typical observing pattern with the DFB at Parkes, where a LEVCAL
observation is made to set the signal levels, which remain fixed until
the next LEVCAL.

```
  -u PAR     model PAR with a step at each CAL
  -o PAR:N   model PAR as N degree polyomial
```

Where PAR is a single-character code:

* g = absolute gain
* b = differential gain
* r = differential phase
* a = all of the above

For example, to model all three parameters as a third order
polynomial, use -o a:3; to model absolute and differential gain
variations with a step function at each CAL, add -u g -u b.

#### 2.5 Recommended and useful command-line options

| Option | Description |
| --- | --- |
| `-m bri00e19` | use Equation (19) of Britton 2000 to model the instrumental response Highly recommended: compared to the default, this model has fewer covariances between parameters. |
| `-Q` | model the noise diode as coupled after the orthomode transducer Highly recommended: eliminates the covariances between the noise diode Stokes parameters and the model parameters that describe the front end. The frontend transformation includes the rotation about the line of sight, which can be strongly impacted by ionospheric Faraday rotation. |
| `-s` | normalize the observed Stokes parameters by the phase-integrated invariant interval Highly recommended: pulsar flux density often varies due to interstellar scintillation. |
| `-n 64` | for MEM, use 64 phase bins as model constraintsr; for METM, use 64 harmonics as constraints The default is 16; using more makes pcm slower but can increase the precision of the result. When modelling PSR J0437-4715 observations with METM, I use 200 harmonics. |
| `-a 0` | disable the phase-alignment check Sometimes good data are unnecessarily discarded because of this check; if you are sure that all of your input data files align well in phase (e.g. the ephemeris is good) then disable this check. |
| `-K 3.0` | reject outliers when computing CAL levels This ignores impulsive RFI when computing the mean of the on-pulse and off-pulse of the noise diode observations. |
| `-step 3.0` | detect and model steps in instrumental response When the input data span many separate observations of the pulsar, and the attenuator levels may be updated between observations, this can help to automatically detect such step changes and model them. |
| `-X 2.0` | mask channels with reduced chisq > 2.0 Activate this only after you start getting good model fits. |
| `-N` | do not unload calibrated data files This can speed things up during cycles of development and testing. |

### 3.0 Algorithms

In MEM mode, `pcm` implements the model of reception and the
algorithm for solution described in van Straten, W. 2004,
[Radio Astronomical Polarimetry and Point-Source Calibration](http://adsabs.harvard.edu/abs/2004ApJS..152..129V),
*Astrophys. J. Supp.* **152**, 129-135.
[arXiv:astro-ph/0401536](http://arxiv.org/abs/astro-ph/0401536)

In METM mode, `pcm` implements the measurement equation template matching
algorithm described in van Straten, W. 2013,
[High-fidelity Radio Astronomical Polarimetry Using a Millisecond Pulsar as a Polarized Reference Source](http://adsabs.harvard.edu/abs/2013ApJS..204...13V),
*Astrophys. J. Supp.* **204**, 13.
[arXiv:1212.3446](https://arxiv.org/abs/1212.3446)

### 4.0 Testing and examples

#### [4.1 MEM Example](pcm/mem.md)

#### [4.2 METM Example](pcm/metm.md)

### 5.0 Known bugs and features that require implementation

* Currently, `pcm` can handle data from only one pulsar.
