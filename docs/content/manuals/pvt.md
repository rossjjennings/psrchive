# pvt

### 1.0 Purpose

One of the foundations of pulsar timing is the assumption that the
mean pulse profile (obtained from several thousand integrated single
pulses) has a stable morphological form in a given frequency range.
Although this is true in the majority of cases, there are notable
exceptions. Some pulsars have two (or possibly more) quasi-stable
modes of emission with different profile morphologies. The dominant
mode can suddenly "switch", causing significant profile evolution in a
very short time. Other pulsars can "turn off" for extended periods of
time (known as "nulling"). Assuming the pulsed emission is
intrinsically stable, there are a number of external influences that
can alter the morphology of the profile. One of the most interesting
is precession of the pulsar rotation axis, which changes our line of
sight to the emission cone (and hence the shape of the observed
profile). Because precession timescales are usually measured in tens
or hundreds of years, it is necessary to search for small changes in
morphology in order to study the effects of precession on pulse
profiles. `pvt` was designed to detect differences
between integrated pulse profiles, using a number of comparison
methods.

### 2.0 Usage

`pvt` has a number of options that govern the type of
analysis performed. In general, it is invoked along with a list of
file names, specifying Archives on disk that contain the Profiles the
user wishes to compare. Most methods of comparison produce graphical
output that can be analysed by eye. The simplest visual method of
comparison is to plot the Profiles in superposition with each other,
after normalisation.

```
pvt -N filenames
```

Pulse amplitude and position angle can be compared in this manner. It
is also possible to zoom in on a particular phase range (with the
`-z` option).

Another useful comparison method is the construction of
"difference profiles", which result from bin-by-bin subtraction of the
amplitudes of one profile from another (again, after
normalisation). To compute difference Profiles, it is necessary to
have a common template Profile against which to compare all other
Profiles. The location of this Profile must also be specified on the
command line as follows:

```
pvt -s template -D filenames
```

For a full list of options, use:

```
pvt -h
```

Multiple filenames can be specified on the command line either
individually or using wildcard characters.

### 3.0 Algorithms

`pvt` uses simple flux normalisation methods and the
Profile::morphological\_difference method. It can also implement more
specific analytical techniques related to the comparison of
individual features in the integrated Profile, as described by Kramer
et al. in the Astrophysical Journal, 520, 324, 1999.

### 4.0 Testing and examples

This program is still somewhat experimental and the algorithms it
employs could change. Often, the program makes hidden assumptions
about the state of the Archives it is reading from (for example, in
some cases it is assumed that full polarimetric information is
available). Users are warned that until the code matures, this program
may require some experimentation and even editing to produce the
desired output.

### 5.0 Known bugs and features that require implementation

* Nothing specific, just lots of polishing.
