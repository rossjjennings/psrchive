# PSRCHIVE documentation: rmfit

## PSRCHIVE user documentation: `rmfit`

### 1.0 Purpose

The Faraday Rotation Measure (RM) fitting code (`rmfit`) implements
a number of different algorithms for RM estimation.

### 2.0 Usage

**Prerequisite:** Data should be calibrated.

**Initialization:** Data are tscrunched and dedipsersed.

`rmfit` implements the following three methods of RM estimation

1. Brute force search for peak linear polarization as a
   function of trial RM.- Iterative differential position angle refinement using two
     halves of the total bandwidth- Using the quadratic fitting algorithm presented by
       [Noutsos et al. (2008)](http://adsabs.harvard.edu/abs/2008MNRAS.386.1881N)

#### 2.1 Brute Force Search

The brute force search is executed by specifying the `-t`
command line option. In this case, `rmfit` automatically
chooses the minimum and maximum RM and the RM step size as follows:

* The maximum absolute RM is set to the value at which the position angle
  rotates by one radian in a frequency channel at the centre of the band; i.e.
  `RM_max=0.5/(lambda*lambda)*nu/dnu`, where
  `lambda` is the wavelength in metres, `nu` is
  the centre frequency in Hz, and `dnu` is the channel bandwidth
  in Hz.* `RM_min=-RM_max`* The step size is `RM_max/nchan`, where `nchan`
      is the number of frequency channels in the data.

It is also possible to set minimum and maximum RM and the number of
steps as in the following

```
rmfit -m min_RM,max_RM,num_steps -D -K /xs filename
```

For each trial RM, `rmfit` corrects Faraday rotation and
computes the total linear polarization. The resulting "RM spectrum"
is plotted when the `-D -K /xs` options are used.
`rmfit` fits a Gaussian to the peak of the "RM spectrum" and
uses the centroid of this Gaussian to determine the best RM.
If this fit fails, then the best RM is set to the trial RM at
which the linear polarization reaches its maximum value.

#### 2.2 Iterative differential position angle refinement

The RM-refinement algorithm is executed with the `-r` command
line option.

This algorithm corrects the data for Faraday rotation using the
current RM, splits the bandwidth in two and integrates each half
over frequency, then computes the [weighted
differential polarization angle](rmfit/DeltaPA.pdf), delta-PA, between the two
halves of the band. By default, only those pulse phase bins with
linearly polarized flux that is 3 sigma above the off-pulse noise in
both halves of the band are included in the estimate of differential
position angle. This threshold can be set with the `-T`
command line option.

If the delta-PA estimate is larger than its uncertainty, then the
data are corrected for Faraday rotation with the derived RM and the
delta-PA is estimated again. This process is repeated until
delta-PA is smaller than its uncertainty, at which point the final RM
is reported.

By default, the current RM used in the first iteration is that
stored in the header of the data file. RM-refinement can also be
combined with the brute force search; e.g. as follows:

```
rmfit -t -r -D -K /xs filename
```

In this case, the best RM output by the brute force search is the
first guess used by the RM-refinement algorithm.

#### 2.3 Quadratic Fitting (Noutsos et al. 2008)

The quadratic fitting algorithm is executed with the `-w` command line option and can be run as:

```
rmfit -w start_pulse,end_pulse archive -D -K /xs
```

In this case, the PA is computed per each frequency channel as an average on the pulse window identified by start\_pulse and end\_pulse. The RM is evaluated as a fit on the PAs.

### 3.0 Algorithms

The iterative differential position angle refinement algorithm is based
on the derivation presented [here](rmfit/DeltaPA.pdf).

### 4.0 Testing and examples

The special command-line option `-J` is equivalent to running

```
rmfit -r -u 2000 -U 6 -a 1 -L -D -K rmsearch.ps/cps
```

with the additional constraint that the maximum RM will never be less
than 100. That is, the maximum RM will be set to 6 times the DM
(`-U 6`), but it will never be greater than 2000 (`-u
2000`), and never less than 100.

### 5.0 Known bugs and features that require implementation

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
