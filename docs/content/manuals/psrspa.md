# psrspa

## 1.0 Purpose

The PSR Single Pulse Analysis (`psrspa`) is designed to
facilitate various analysis of single pulse data. It consists of
three main parts:

* Scan pulses and find giants. This is what used to be implemented in the
  `spa` program as implemented by Tim Dolley and Aidan Hotan. This parts identifies
  pulses in the data and measures their flux. This can be used to identify giant
  pulses. Distribution of fluxes can be calculated.
* Generate phase resolved histograms of:
  + total intensity flux
  + polarisation degree
  + polarisation angle
* Identify bin with highest flux in chosen phase range(s).

Note that the second and third part require the third party package
[GNU Scientific Library](http://www.gnu.org/software/gsl/).

## 2.0 Usage

### 2.1 Basics

`psrspa` supports several standard options such as preprocessing
of the input archives using the "-j" or "-J" switches, meta file containing
a list of input archives ("-M"), verbosity settings ("-v" and "-V") as
well as "-h" which will print a summary of available options.

### 2.2 Modes

The mode in which `psrspa` is to be run needs to be
chosen, i.e., at least one of the flags "-hf", "-hd", "-ha", "-hl", "-hc", "-fm"
or "-sla" needs to be set. The first two options correspond to the first part of
the program. Please see the [spa](spa.md) documentation for
more information. All of the "-hX" options are used to calculate phase
resolved histograms. Finally, the "-fm" will find maximum amplitudes in chosen
phase range.

### 2.3 Phase Range

The phase range choice is also affecting the phase resolved histograms.
Multiple phase ranges can be specified with the "-pr" option as a list of
comma seperate beginnings and ends of a phase.

### 2.4 Output Files Naming and Format

The resulting files with the chosen phase resolved histograms are stored in
files named in a following way:

```
path/prefix_type_bin.ext
```

All of the above with the exception of type
can be specified by the user (see the section "Output of the histograms" in the
builtin help). The value of type is chosen by `psrspa` as poldeg polang or
flux for the polarisation degree, its angle or
phase resolved flux, respectively.
The format of the output files is following:

```
bin_start bin_end count
```

The histograms are not normalised and all the post-processing has to be
performed by the user.
Naming scheme of the files with maximum flux in the chosen phase range is similar to the previously specified:

```
path/prefix_type.ext
```

with type set to maxamp.
The format of these files is compatible with the [GNU Scientific Library](https://www.gnu.org/software/gsl/):

```
file subint bscrunch_factor phase_min phase_max max_bin max_amplitude
```

where the bscrunch\_factor specifies wheter the
archive was bscrunched before the calculation of the following values when
using automatic looped bscrunching with the "-bl" option.

## 3.0 Algorithms

The part of `psrspa` corresponding to the original `spa` is directly
based on that code.
The phase resolved histograms and search for maximal phase are based on the `PolnProfile`,
`ProfileStats` and `Profile` classes.

## 4.0 Testing and examples

To perform all the calculations with the standard settings simply run:

```
psrspa -hf -hd -ha -hc -hl -fm -sla input_file.ar
```

The "input\_file.ar" can be replaced by multiple files, regular expression or a
metafile containing list of archives.
To calculate only the polarisation degree for pulse profile phases falling
between 0.45 and 0.50 as well as between 0.60 to 0.70 run:

```
psrspa -hd -pr 0.45,0.50,0.60,0.70 input_file.ar
```

Note the format of the phase range specification: each phase range has to be
identified by two numbers, the beginning and end of the phase range. Multiple
ranges can be specified by simply being separated by commas. When no phase
range is chosen, the full range is used.

## 5.0 Known bugs and features that require implementation

* Finding pulses could be extended to include fitting a Gaussian / von Misses distributions (or any other function) to the pulses. This in turn would allow an easy determination of the pulse width.
* Sometimes closing ofstream objects on Intel-based Mac OS X machines can caues a segmentation fault or bus error but the files do not appear to be corrupted in any way.
