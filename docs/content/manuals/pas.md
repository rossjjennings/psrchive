# pas

## 1.0 Purpose

Pulsar timing is done by comparing the phase of each observed pulse
with the fiducial phase of a standard template profile. The absolute
phase of the standard template is not important and it is often
modified so that the pulse appears in the middle or edge of the
profile for ease of analysis. However, if multiple standard template
profiles are required (for example, to span multiple frequency bands
when there is significant evolution of the pulse profile as a function
of frequency) it is essential that they are all aligned to the same
absolute reference phase.

`pas` was designed to edit standard template profiles
in a number of useful ways. Profile objects can be rotated in single
or fractional bin increments (using the Fourier shift theorem),
baseline noise can be zeroed and two different profiles can be
mutually aligned by maximising their cross-correlation function.

## 2.0 Usage

`pas` is slightly different to most other PSRCHIVE applications in that it is an interactive
GUI based on the PGPLOT subroutine library. It is invoked with the
name of the Archive containing the Profile that the user wishes to
edit and all other operations are controlled via the GUI. Occasionally
the user is prompted to enter additional information in the terminal
as well. A typical `pas` session begins as follows:

```
pas filename
```

There are a number of additional command line options related to
verbosity, but the most commonly used option is `-r`, which
allows the user to specify a reference Profile as well as one to be
edited. If this option is used, `pas` plots both profiles in
the same window (though only one can actually be edited) and a number
of additional processing options become available.

```
pas -r reference filename
```

Both the reference Profile and the Profile to be edited are loaded
from Pulsar::Archive objects which can be stored on disk in any of the
supported file formats. Typically, the Archive containing the standard
Profile will be fully scrunched in time, frequency and
polarisation. If this is not the case, `pas` simply
extracts the Profile in the first integration, polarisation and
frequency channel.

## 3.0 Algorithms

`pas` makes use of the various Profile::rotate methods
and the same shift detection routines used to calculate arrival times.
In addition, it allows the user to modify Profile amplitudes in order
to zero out unwanted baseline noise.

## 4.0 Testing and examples

During a typical pas session, the first in a series of standard
templates (representing widely separated frequency bands for example)
is loaded and the phase of the Profile is modified so that the peak is
at a convenient fiducial point. If desired, the baseline noise can be
eradicated as well. Subsequent template Profiles are loaded using the
`-r` option to specify the first one as the
reference. The automatic alignment function is then used to bring each
of the other templates into line with the first, creating a set of
templates with the same reference phase.

## 5.0 Known bugs and features that require implementation

* None at this time.
