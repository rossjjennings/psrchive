# pat

### 1.0 Purpose

`pat` is used to determine the time of arrival (TOA) of
each integrated Profile in an Archive. It can use a variety of methods
to determine the phase shift between each Profile and a standard
template. This shift is added to the reference epoch of the
Integration in which the Profile resides to give the representative
site arrival time. These TOAs can then be analysed with TEMPO or
TEMPO2.

### 2.0 Usage

`pat` is very easy to use, all commands are of the form

```
pat options filenames
```

where the options allow the user to specify a standard template
profile, the fitting algorithm to use, the output format and several
other small configuration parameters. For a full list of options, use

```
pat -h
```

.
Multiple filenames can be specified on the command line either
individually or using wildcard characters.

### 3.0 Algorithms

At the present time, `pat` can use any one of five
algorithms to determine the phase shift between the standard template
and the observed Profile.

* **Fourier Phase Gradient (PGS)** This algorithm was described
  By J. Taylor in the Philosophical Transactions of the Royal Society of
  London A, 1992, 341, 117. It takes advantage of a property of the
  Fourier transform known as the "shift theorem", which states that the
  Fourier transform of a function and a shifted copy of the function
  differ only by a linear phase gradient. Fitting for this gradient in
  the Fourier domain can determine the shift between two similar
  Profiles. This algorithm is very precise when the S/N of the Profile
  is high.
* **Gaussian Interpolation Shift (GIS)** This algorithm was
  described by Hotan, Bailes and Ord in the Monthly Notices of the Royal
  Astronomical Society, 362, 1267. `pat` calculates the
  discrete cross correlation function of the Profile with the template
  and a Gaussian curve is then fitted to the resulting points to allow
  interpolation between each phase bin. In this manner, TOAs can be
  determined to within approximately 1/10 of the width of an individual
  phase bin. This algorithm is less susceptible to noise contamination
  than the PGS method, but is less precise when the S/N is high.
* **Parabolic Interpolation Shift (PIS)** This is the oldest
  method available. It is very similar to the GIS method, but uses only
  the peak bin of the cross correlation function and one bin on either
  side to define a parabola that is used for interpolation.
* **Zero Pad Fitting (ZPF)**This method interpolates the cross
  correlation function by Fourier transforming, padding the result with
  zeroes and transforming back to the time domain. It is somewhat
  experimental and the error estimate it returns is not reliable.
* **Sinc Interpolation Shift (SIS)** This algorithm is similar in
  effect to the ZPF method.

All of the above methods fit only the Stokes I profile. The user can
also choose to fit the full polarimetric profile in the Fourier domain
using an algorithm described by van Straten in the Astrophysical
Journal (in press).
Normally, the standard template profile is loaded from a
Pulsar::Archive, but it is also possible to use an analytic standard
template constructed from Gaussian components.
`pat` is compatible with multiple TOA output formats,
including parkes, itoa, princeton and the more modern tempo2 format.
Additional flags can be added to the default output when using the
tempo2 format, allowing the user to carry extra information (like the
name of the instrument used to record the data) along with the TOAs.

### 4.0 Testing and examples

Typically, `pat` will be invoked in the following
fashion:

```
pat -A GIS -s std.rf *.rf
```

The above command prints a formatted list containing the TOAs and
associated information like frequency and uncertainty. If no algorithm
is specified (with the -A option), the PGS method is used.

### 5.0 Known bugs and features that require implementation

* None at this time.
