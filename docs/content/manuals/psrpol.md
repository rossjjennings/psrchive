# psrpol

### 1.0 Purpose

The PSR polarisation code (`psrpol`) is designed to
create phase-resolved spherical histograms of the polarisation vector.
Note that the second and third part requires a third party package
[HEALPix](http://healpix.jpl.nasa.gov/).

### 2.0 Usage

`psrpol` support the standard options such as preprocessing
of the input archives using the "-j" or "-J" switches, meta file containing
a list of input archives ("-M"), verbosity settings ("-v" and "-V") as
well as "-h" which will pritn a summary of available options.
The core functionality of `psrpol` is the calculation of a
phase-resolved spherical histograms of the polarisation vector of the input
data. The user can choose over what phase range these histogram need to be
calculated as well as the resolution of the resulting Stokes sphere pixelization.
The output can be read and plotted with map2tga program contained in the HEALPix
package.

### 3.0 Algorithms

The pixelisation of the Stokes sphere is done using Hierarchical Equal Area
isoLatitude Pixelization of a sphere ([HEALPix](http://healpix.jpl.nasa.gov/)).
This algorithm has been developed for studies of the fluctuations of the cosmic microwave
background and is described in detail in [Górski et al. 2005](http://adsabs.harvard.edu/abs/2005ApJ...622..759G).

### 4.0 Testing and examples

To calculate the phase-resolved spherical histograms of the polarisation vector run:

```
psrpol -pr 0.2,0.8 input_file.ar
```

The "input\_file.ar" can be replaced by multiple files, regular expression or a
metafile containing list of archives.
Note the format of the phase range specification ("-pr" switch): each phase range has to be
identified by two numbers, the beginning and end of the phase range. Multiple
ranges can be specified by simply being separated by commas. When no phase
range is chosen, the full range is used.
Examples of resulting histograms can be seen in Fig. 13 and 14 of
[Osłowski et al. 2014](http://adsabs.harvard.edu/abs/2014arXiv1405.2638O).

### 5.0 Known bugs and features that require implementation

* Limitation of number of phase bins: the output file can contain at most 999 maps (pixels)

Copyright © 2014 Stefan Oslowski
Licensed under the Academic Free License version 2.1
