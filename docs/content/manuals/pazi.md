# pazi

### 1.0 Purpose

Pazi is a program developed from psrplot (Willem van Straten) and
pav (Aidan Hotan) to create two main plots (phase vs frequency and
phase vs time) from pulsar data using the PGPLOT graphics
subroutine library. Its functionality can be summed up as an
interactive program combining features from pav and paz using the keyboard and mouse.

The program allows the user to interchange between phase vs time
(default) and phase vs frequency plots with a single key. It also
contains zoom functionality (by increasing the scale on the y-axis)
and allows the user to zap channels or subints, as well as an
additional plot displayed in a separate window graphing the total
frequency, time and polarisation scrunched profile. Files can be
saved at the user's request where a new file filename.rz or
filename.cz is created with the changes.

### 2.0 Usage

The command is in the form:

```
# pazi filename
```

If the 'p' key is pressed, the output of the paz command is
displayed in the terminal as:

```
> paz -z "1 2 3 4 5" -w "5 4 3 2 1" -e rz [filename]
```

Running the command without specifying a filename will print
a short list of commands to the terminal:

```
# pazi

Pazi: A user-interactive program for zapping subints and 
frequency channels.
Usage: pazi [filename]

zoom:                      left click twice
zap:                       right click
zap (multiple):            left click and 'z'
reset zoom:                'r'
frequency:                 'f'
time:                      't'
save ([filename].pazi):    's'
quit:                      'q'
print paz command:         'p'
center pulse:              'c'
undo last:                 'u'
toggle dedispersion:       'd'
binzap:                    'b'
```

Upon program execution, pazi automatically plots a phase vs
time image. The user can then maniupulate archive data and the
image using the mouse and keyboard. The program is designed to
output the equivalent paz command and/or to create a new file
with the channels/subints/bins zapped.

### 2.1 Mouse and Keyboard commands

The list of mouse and keyboard commands as follows:

```
zoom:                       Left click on the selected boundaries.
zap channel:                Right click.
zap multiple channels:      Mark using left click. Move mouse, press 'z'.
rest zoom:                  'r'
quit:                       'q'
print paz command:          'c'
phase vs frequency:         'f'
phase vs time:              't'
save modified file:         's'
print paz command:          'p'
center pulse:				'c'
undo last change:           'u'
toggle dedispersion:        'd'
binzap mod:                 'b'
```

### 3.0 Algorithms

In order to manipulate the data while interchanging between
plots, two instances of the archive are used. The original
archive base\_archive retains each change when
zapping frequency channels and/or sub-integrations. As the
plots have to be frequency and time scrunched according to
the type of plot, each time the plot is redrawn, a copy
of base\_archive is scrunched and then displayed
on the screen.

For each keyboard or mouse command, the program checks
plot\_type to find out if the current plot is phase
vs time or phase vs frequency. This accomodates for the
different methods specific to each plot.

The binzap function calculates the mean (excluding bin/s to be
zapped) across the range of bins specified by the end points
of the current plot. The value is then replaced with:
mean + (random Gaussian number \* variance)
Where the mean is calculated within each channel belonging to
the subint.

### 4.0 Testing and examples

Examples of usage and output have been described in Section 2.0.

### 5.0 Known bugs and features that require implementation

* None at this point in time.

Copyright © 2007 Jonathan Khoo
Licensed under the Academic Free License version 2.1
