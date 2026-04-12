# spa

### 1.0 Purpose

The Single Pulse Analysis program, `spa`, is designed to
search for giant pulses and giant micropulses. It calculates the flux
of individual pulses and plots a histogram of flux distributions. This
can be done for the integrated pulse flux, peak flux, or flux at a
given phase. A plot of the cumulative probability distribution is also
given. A list of individual pulses which are larger than the mean flux
by a given factor is displayed, along with integration numbers, phase,
flux, and the filenames of the archives containing
them. `spa` can also be used to plot the variation in
amplitude in each phase bin. It uses the PGPLOT graphics subroutine
library and any of its associated output devices. It is designed to
extract and display information only, it will not modify archives in
any way. All flux measurements are given in units of mean flux.

### 2.0 Usage

All of `spa`'s commands are of the form:

```
spa [options] filenames
```

with the following options available:
 `-v`                    Verbose mode
 `-V`                    Very verbose mode
 `-h`                    Help
 `-l`                    Logarithmic flux scale
 `-f`                    Save output text as 'spa\_out.txt' - if exists file overwritten
 `-f[filename]`   Save output text as [filename] - if exists file overwritten
 `-g[factor]`      Show giant pulses with flux = [factor]\*[mean flux]. The default value of [factor] is 5
 `-p`                    Use peak flux, not total flux
 `-p[phase]`        Use peak flux, calculated at phase = [phase]. This is useful for off-pulse flux or on-pulse variations
 `-w[width]`        Sets width to sum flux over (in phase units). Use only with the `-p` or `-d` flags
 `-d`                    Plot amplitude variations against pulse phase (1 file only). Can be used with `-p` and `-w` flags to zoom in.
Multiple filenames can be specified on the command line either
individually or using wildcard characters. Not much output is
displayed while files are being loaded and analysed, unless verbose
mode is used, so be patient if loading multiple files.

### 3.0 Algorithms

`spa` uses the Archive and Profile classes to examine
individual pulses. It calculates pulse flux densities and removes the
mean baseline level. In the case of using only amplitudes (-p flag),
the baseline rms is removed. All distribution plots show flux in terms
of the mean flux, which is calculated for each individual file to
compensate for possible changes due to scintillation.

### 4.0 Testing and examples

The flags which can be set with `spa` are explained reasonably
well by using the `-h` option, as below.

```
spa -h
```

The example below displays the log (base 10) of the pulse flux on the
x axis of the flux distribution plot, and only lists pulses with a
flux greater than 10 times the mean flux. The phase shown in the
output text is for the centre of the region with maximum flux.

```
spa -g10 -l  pulse_2109359875.ar
```

The next example uses the peak amplitude of pulses instead of total
flux. It also uses verbose mode, displaying information about the
state of the program during runtime. The phase shown in the output
text is for the peak amplitude.

```
spa -p -v *.ar
```

This time amplitudes are used again instead of total flux, but
amplitudes are calculated at the phase given (phase = 0.649), instead
of peak amplitudes. This is useful for observing off-pulse flux
distributions, normal pulse flux distributions and giant pulse flux
distributions individually.

```
spa -p0.649 -fspa_out.txt *.ar
```

When the `-p` flag is used with the `-w` flag
instead of using the amplitude of individual phase bins, bins are
summed over an area of the width given. In this case over a width of
0.1 (in phase) centred at a pulse phase of 0.65.

```
spa -p0.65 -w0.1 *.ar
```

The example below is sums over a width of 0.1 centred on the peak
amplitude of each pulse period.

```
spa -p -w0.1 *.ar
```

The `-d` flag plots the variation in amplitude in each phase
bin. The value plotted is the number of standard deviations the
maximum value is away from the mean for each phase bin.

```
spa -d  pulsar.ar
```

Finally the `-p` and `-w` flags can be used with
`-d` to zoom in on the area of interest.

```
spa -d -p0.65 -w0.8  pulsar.ar
```

Standard redirection can be used to save the giant pulse information
instead of the `-f`, but the pgplot prompt requesting an
output device will also be sent to the output file and the program
will wait for a response, appearing to have frozen.

### 5.0 Known bugs and features that require implementation

* -d flag only works with one file
