# PSRCHIVE documentation: vap

## PSRCHIVE user documentation: `vap`

### 1.0 Purpose

The Viewer of Archive Parameters, `vap`, is designed to
interrogate headers and glean simple numerical quantities from the
data contained inside pulsar archives. It is a terminal-based program
that can be combined with the standard utilities awk and grep to
produce large summary listings of multiple archives for the purpose of
organisation, sorting and classification. It does not do any graphical
visualisation or file manipulation.

### 2.0 Usage

`vap` is very simple to use, all commands are of the form

```
vap options filenames
```

where the options can be anything from a large list of available
parameters. For a full list of options, use:

```
vap -h
```

Multiple filenames can be specified on the command line either
individually or using wildcard characters.

### 3.0 Algorithms

`vap` serves primarily as a simple interface between the
user and member functions of various classes in the PSRCHIVE
scheme. It does not (in general) implement any additional algorithms.

### 4.0 Testing and examples

The header parameters in an archive are designed to provide information
about the way the main body of data was observed, recorded and stored.
This includes storage of characteristics like radio frequency, track
duration, calibration status, number of frequency channels, etc. Many
parameters are essential and will always be present in any archive header;
these are listed first in the help text. Certain optional parameters are
only available through the use of Archive or Integration Extension classes,
these parameters are not stored in all archives and are included in a
separate section of the help text.
To access any of these basic storage parameters, a space-separated list
should be passed (in parentheses) to `vap` using the "-c"
argument. For example:

```
vap -c "NAME RAJ DECJ INST PCAL FCAL" *.ar
```

The above command prints a formatted list containing the source name,
sky coordinates, instrument used and calibration status of all
archives with the ".ar" extension. It is possible to alter the formatting
of the output using additional options described in the later section of
the help text. One of the most useful is the "-n" option, which removes
the column headings to make it easier to manipulate the output with awk,
say.
In addition, `vap` can display simple analytical
information (like signal-to-noise ratio), using one or more of the
extended options listed at the end of the help text. For example:

```
vap -r *.ar
```

It is possible (in some cases) to combine these extended options with
header parameters accessed through the "-c" argument. For example, S/N
can be printed alongside observation length using the command:

```
vap -rc *.ar
```

### 5.0 Known bugs and features that require implementation

* None at this time.

Copyright © 2006-2019 Aidan Hotan
Licensed under the Academic Free License version 2.1
