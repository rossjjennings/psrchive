# pav

### 1.0 Purpose

The Pulsar Archive Viewer, `pav`, is designed to visually
display the data stored in an archive. It uses the PGPLOT graphics
subroutine library and any of its associated output devices. Like
`vap`, `pav` is designed to extract and
display information only, it will not modify archives in any way. It
does, however, allow for memory-only pre-processing of an archive. This
means that any processing options specified on the command line are
performed on the archive in memory after it has been read from disk and
are lost once the program exits. To permanently modify an archive on
disk, use `pam`.
The program is built around the PSRCHIVE Plotter class, which contains
a vast and bewildering array of visualisation algorithms. The user can
select which type of plot they desire, specify pre-processing and
display fine-tuning options, then show the result on screen or direct
it to a file according to the drivers installed in the local version
of pgplot.

### 2.0 Usage

`pav` is fairly simple to use, though the wide range of
options available to the user can cause confusion. All commands are of
the form:

```
vap options filenames
```

where the options can be anything from extensive list of available
choices. For a full list of options, use:

```
pav -h
```

Multiple filenames can be specified on the command line either
individually or using wildcard characters. Each file will be displayed
in sequence, one after the other. Alternatively, the view panel can be
partitioned into a grid of smaller panels (using the "-N" option) so
as to fit multiple files in the same window.

### 3.0 Algorithms

The PSRCHIVE Plotter class contains a wide range of visualisation
algorithms. The simplest of these allows single profiles to be extracted
and plotted, more complex algorithms combine multiple dimensions to form
a colour or contour map, or a composite of various visualisation styles.

### 4.0 Testing and examples

`pav` works by setting a flag for each of the available
plot styles and processing options. It will run and exit cleanly even
if no visualisation flags are selected, simply producing no
output. Visualisation flags are (usually) independent of the
pre-processing flags, which allow the user to, say, combine a certain
number of phase bins together in order to clean up a noisy
profile. Multiple visualisation flags can be combined, usually
producing each display in turn. The following examples show various
ways in which `pav` can be used to visualise archives:
To combine all the data in an archive into a single integrated pulse
profile, use:

```
pav -DFTp filename
```

In this case, "D" is the visualisation flag and "FTp" are all pre-
processing flags. To view an integrated profile with polarisation
information, use:

```
pav -SFT filename
```

where the visualisation option "D" has been exchanged for "S". Some of
the plotting options are only relevant to certain file types. For
example, a plot designed to show instrumental phase will only yield
useful information if given a calibrator file. The user must exercise
a certain amount of judgement in these cases. It is also possible to
fine-tune the graphical output. For example, to produce a plot
suitable for publication, it might be necessary to exclude the
filename from the heading and change the colour scheme to better suit
a light background. The umbrella option "--publn" handles the
formatting of labels and headers; there are multiple options for
manipulating colour schemes, depending on the type of plot.

### 5.0 Known bugs and features that require implementation

* None at this time.

Copyright © 2006 Aidan Hotan
Licensed under the Academic Free License version 2.1
