# pac

### 1.0 Purpose

The Pulsar Archive Calibration program, `pac`, performs
both polarimetric and flux calibration of pulsar observations.
Calibration information is provided by calibrator archives, which may
contain either observations of an artificial reference source or a
table of model parameters that describe the instrumental response
(such as that produced by [`pcm`](pcm.md)).
To simplify the task of matching calibrators with observations,
`pac` uses a database and selection criteria.

### 1.1 Supported Models

Currently, `pac` can perform polarimetric calibration
using a number of different models of the instrumental response:

1. `SingleAxis` - This is the most commonly used,
   most rudimentary and, in most cases, incorrect calibration model.
   A single observation of a reference source is used to determine
   the differential gain and phase of two receptors. It is called
   the "Single Axis" model because the transformation matrix can be
   decomposed into a boost along and rotation about a single axis.
   With a linear feed, this axis is the Stokes Q axis; in the case
   of circularly polarized receptors, it is the Stokes V axis. This
   model is based on the assumptions that the receptors are
   perfectly orthogonally polarized, and the reference source is
   100% linearly polarized and illuminates both receptors equally
   and in phase.
2. `Polar` - Introduced in
   van Straten (2002 ApJ: 568, 436), this model is not necessarily
   any more valid than the `SingleAxis` model. It can be
   used to estimate the self-adjoint (or boost) component as well as
   2 out of the 3 degrees of freedom of the unitary (or rotation)
   component of the instrumental transformation. This model is
   based on the assumptions that the off-pulse emission (system plus
   sky) is completely unpolarized, and that the receptors have equal
   and opposite ellipticities.
3. `SingleAxis+Flux` - This model combines
   observations of the reference source made near both the pulsar
   and a standard candle (flux calibrator) to eliminate the
   assumption that the reference source illuminates both receptors
   equally. It is therefore better than `SingleAxis`
   alone.
4. `Reception` - Introduced in
   Ord et al. (2004 MNRAS: 352, 804), this model combines the
   results of two separate models to form the most accurate estimate
   of the instrumental response. First, either a
   `Britton` or `Hamaker` model, as produced
   by [`pcm`](pcm.md), is used to
   completely represent the system response at a fiducial epoch.
   Second, a `SingleAxis` model is used to correct for
   any variations in differential gain or phase since the reference
   epoch. In addition to any assumptions made to overcome the
   [measurement equation
   modeling degeneracy](pcm/degeneracy.md), this model is based on the assumption
   that the receptor orientations and ellipticities, as well as the
   polarization of the reference source, do not vary significantly
   with time.

Ultimately, the model selected by the user will depend upon the
perceived validity of the model assumptions and the availability of
calibration information. Using any of the above calibration models,
the output archive will be normalized in units of the reference flux
density. This important feature enables polarimetric calibration to
be performed independently of flux calibration. Flux densities are
calibrated to an absolute scale in mJy by using a flux calibration
solution of the form produced by the
[`fluxcal`](fluxcal.md) program.

### 2.0 Usage

### 2.1 Creating the calibrator database

Use of the calibrator database not only simplifies the process of
matching calibrator archives with pulsar observations, it also reduces
the amount of time spent re-loading calibrator archives in order to
access the header information.

Before creating the database, all calibrator archives (including flux
calibrator observations and solutions) should be gathered into one
directory. Within this one directory, the archives may be sorted into
sub-directories; `pac` will search the entire directory
tree below the specified base directory.

If a single observation of the reference source extends across
multiple archive files, these should be concatenated into a single
file using `psradd`. It is not necessary to tscrunch
the integrations together, as estimates from multiple integrations
will be used to form a weighted mean. However, if the phases of the
reference source waveforms are aligned, it is better to tscrunch and
produce a single integration with high signal-to-noise ratio.

After gathering the calibrator archives, run `pac -wp
$MYCALS`, where $MYCALS is the path to the base of the
directory tree that contains the calibrator observations. By default,
`pac` will search for calibrator archives with the following
file extensions: cf, pcal, fcal,
pfit, and fluxcal. If the calibrator
archives have different file extensions, these can be specified using
the `-u .myext` command-line option; the
extension may be any value except for `txt`. The dot
preceding the extension name is optional.

When completed, `pac` will output the database in the
text file, $MYCALS/database.txt.
The amount of time required to run will depend upon the number and types
of calibrator archives in the directory.

#### Example: Specify Extension

All of the calibrator archives have been gathered into the directory
/psr/data/calibrators, this is the current working
directory, and all of the files have the extension, .fb:

```
pac -w -u fb 
pac: Generating new calibrator database
pac: Writing database summary file to /psr/data/calibrators/database.txt

pac: Finished all files
```

#### Example: Specify Files

All of the calibrator archives have been gathered into sub-directories
of /psr/data/calibrators, this is the current working
directory, and all of the files are listed in cals.ls:

```
pac -W cals.ls 
pac: Generating new calibrator database
pac: Writing database summary file to /psr/data/calibrators/database.txt

pac: Finished all files
```

### 2.2 Calibrating pulsar observations

By default, `pac` will attempt to calibrate both the
polarization and flux density of each pulsar archive supplied as a
filename on the command line. In order to disable flux calibration,
or to choose the applied polarimetric calibration model, use the
following command line options:

```
  -P       Calibrate polarisations only 
  -S       Use the complete Reception model 
  -s       Use the Polar Model
```

The `SingleAxis` model is the default.

#### Matching Criteria

Calibration information is matched to each pulsar observation using the
following criteria.

1. A calibrator is considered to match a pulsar observation if both
   were observed using the same receiver, instrument, centre frequency, and
   bandwidth.
2. In addition to criterion 1, a flux calibrator is used only if
   it was made within one month before or after the pulsar observation.
3. In addition to criterion 1, a polarimetric reference calibrator
   observation matches only if it was made within 2 hours of the pulsar
   observation, and only if the telescope was pointing within 5 degrees
   of the pulsar.
4. When using the `Reception` model, any
   `Britton` or `Hamaker` solution that satisfies
   criterion 1 will be used (ie. no maximum time constraint); a match for
   the `SingleAxis` component is found using criterion 3.

Any of the above matching criteria can be disabled using the following
command line options:

```
  -c       Do not try to match sky coordinates
  -I       Do not try to match instruments
  -T       Do not try to match times
  -F       Do not try to match frequencies
  -b       Do not try to match bandwidths
  -o       Allow opposite sidebands
```

When multiple matches are found, the calibrator archive with
an epoch closest to the time of the pulsar observation
will be selected. If a match is not found for polarimetric
calibration, an error message will be printed and `pac`
will not produce any output for this file. If a match is not found
for flux calibration, a message will be printed, the output file
will have only its polarization calibrated, and the output filename
will have the letter 'P' appended to it.

#### Output

For each input pulsar archive, a calibrated archive with a similar
filename will be output using the same file format as the input
archive. The extension of the output filename will be *replaced*
with calib. Currently, no
check is made to ensure that the input filename does not also have
this extension. To change the output filename extension, use
the `-e ext` command line option.

If the signal-to-noise ratio appears to be correlated with the
absolute gain of the instrument, then it may be useful to compensate
for any artificial amplification introduced by absolute gain calibration.
Otherwise, sections of data with poor signal-to-noise ratio will contribute
amplified noise to the total integrated profile. To normalize
the profile weight by the absolute gain, use the `-G` command
line option. This option should be used with great care and reservation.

### 3.0 Algorithms

### 3.1 Known Configuration

The PSRFITS file format includes a number of parameters that describe
the known configurations of both the front and backends. For a
complete description, please refer to the
[PSRCHIVE/PSRFITS Basis](../appendices/basis/basis.pdf)
documentation.

### 3.2 Polarimetric Calibration

Polarimetric transformations are represented by Jones matrices. Each
calibration model produces a Jones matrix for each observed frequency
channel (or sub-band).

### 3.3 Flux Calibration

To perform absolute flux calibration, `pac` requires
flux calibration solutions of the form produced by the [`fluxcal`](fluxcal.md) program.

### 4.0 Testing and examples

No test outputs are yet available.

### 4.1 Choice of calibration model

To use the `Reception` model, it will be necessary to run
[`pcm`](pcm.md).

### 5.0 Known bugs and features that require implementation

* The program does not ensure that output filenames do not equal input
  filenames.
* This documentation does not describe the corrections that are
  applied to the pulsar archive based upon the known receiver
  configuration.

---

### Appendix A: Calibrator observations

All of the `pac` calibration models require observations of a
noise diode that is somehow coupled to the receptors, called a CAL,
which ideally presents a 100% linearly polarized reference source.

Observations of the CAL should contain both on and off states, which
can be achieved by modulating the noise diode signal with a square
wave (typically using a 50% duty cycle) and folding the signal at the
modulation period. That is, the mean polarization profile of the CAL
should be formed by integrating the polarization statistics
(e.g. Stokes parameters) as a function of modulation phase and radio
frequency. The CAL should be observed with the same instrumental
configuration as used to observe the pulsar (including centre
frequency, bandwidth, frequency resolution, etc.).

The frequency of the square wave should be a couple of orders of
magnitude less than the smallest bandwidth that you plan to calibrate.
For example, if you plan to retain a frequency resolution of 125 kHz, then
consider a maximum modulation frequency of 512 Hz (11.123 Hz is
used at Parkes). Record for as long as is necessary to achieve a
sufficiently strong integrated signal in each of the frequency
channels.

### A.1 Polarimetric calibrator observations

The calibrator reference source should be observed directly before or
after the pulsar observation. At Parkes, this is usually done while
the antenna is pointing at least a full beam width off source;
however, this isn't really necessary if the pulsar is weak. For this
observation, `psredit -c type filename` should return
`PolnCal`.

### A.2 Flux calibrator observations

Approximately once per day, and for each instrumental configuration
(e.g. centre frequency, bandwidth, receiver, etc.), a set of flux
calibrator observations should be made. These observations are used
to determine the flux of the calibrator reference source with respect
to a "standard candle." At Parkes, the radio galaxy 3C 218, or Hydra
A, is most often used. Two sets of observations should be made:

1. **FluxCalOn** - While the antenna is directed at the standard
   candle, observe the reference source as described above.
   For this observation, `psredit -c type filename`
   should return `FluxCal-On`.
2. **FluxCalOff** - While the antenna is pointing at least a full
   beam width away from the standard candle, observe the reference
   source again.
   For this observation, `psredit -c type
   filename` should return `FluxCal-Off`.

At Parkes, `FluxCal-Off` observations are typically made by
pointing both north and south of Hydra A.
