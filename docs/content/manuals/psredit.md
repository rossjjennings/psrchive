# psredit

### 1.0 Purpose

The Pulsar Archive Editor, **psredit**, enables one to
query, modify, add or remove the data in a Pulsar Archive.

### 2.0 Usage

**psredit** commands are of the form:

```
psredit -c command[s] [output options] filename[s]
```

where `command[s]` is a string of one or more commands,
separated by commas. (Multiple commands may also be specified by
using multiple `-c` options.)
Each command either gets, sets, or adds a parameter value.

#### 2.1 Query (get)

A get command is simply the name of the paramter to be queried; e.g.

```
psredit -c bw,freq filename[s]
```

will query the bandwidth and centre frequency of the specified archives.

To print a list of every parameter in the data file, including a short description
of each parameter and its current value,

```
psredit filename[s]
```

Note that some parameters may be either vectors or attributes of the elements of a vector.
For example, when listing every parameter in a data file, the attribute that provides access
to the vector of sub-integrations is output as

```
int*:@           int:help for attribute list
```

The asterisk indicates that the file contains a vector of integrations, and the ampersand
indicates that each element of this vector has its own list of attributes that can be
listed by querying the help; e.g.

```
psredit -c int:help file.ar 
nchan            Number of frequency channels
npol             Number of polarizations
nbin             Number of pulse phase bins
mjd              Epoch (MJD)
duration         Integration length (s)
period           Folding period (s)
gate             Gate duty cycle (turns)
freq*            Channel centre frequency (MHz)
wt*              Channel weight
ext              list all extensions, or add/remove extension
point:lst        Local sidereal time (seconds)
point:gb         Galactic longitude (deg)
point:gl         Galactic latitude (deg)
point:fa         Feed angle (deg)
point:pa         Position angle of the feed (deg)
point:va         Vertical (parallactic) angle (deg)
point:az         Telescope azimuth
point:zen        Telescope zenith
point:info       list additional info, or add/remove info
```

In the above list, `freq*` and `wt*` are vectors of frequencies and weights for each frequency channel
of the sub-integration. The `point` structure contains attributes with additional information about the
orientation of the telescope. These attributes are specific to a horizon-mounted (alt-az) telescope, but can
be extended as described in the following section.

For vector attributes, if only the parameter name is specified, then the query will return all elements of the vector.
Multiple output values are separated by commas, as in the following example:

```
psredit -c int:mjd file.ar
file.ar int:mjd=52678.077,52751.281,52794.848,52795.401,52795.688,52804.146,52812.626,52812.695,52839.118
```

Here, the epoch (MJD) of each sub-integration in `file.ar` is output as a comma-separated list.

It is also possible to specify an element or range of elements, using simple array syntax; e.g.

```
psredit -c 'int[2,5-]:duration' file.ar
file.ar int[2,5-]:duration=8686.6640625,8686.6640625,8686.6640625,8686.6640625,8686.6640625
```

Here, the integration length, or duration, of the sub-integrations
indexed by 2 and 5 through 8 (there are 9 sub-integrations) are output
as a comma-separated list.

**NOTE:** the single quotation marks in the above command-line example
are necessary to protect the square brackets from interpretation by the shell.

#### Formatting output

With the `-q` command line option, `psredit` will
not output the name of the file from which the parameters are queried; e.g.

```
psredit -qc freq,bw filename.ar
freq=1420 bw=-64
```

With the `-Q` command line option, `psredit` will
not output the names of the queried queried; e.g.

```
psredit -Qqc freq,bw filename.ar
 1420 -64
```

To change the precision with which values are printed append a `%<digits>` to the parameter name; e.g.

```
psredit -Qqc int:mjd%30 filename.ar
 55915.5135737132842719265823916
```

#### 2.2 Modify (set)

A set command is an assignment expression with the parameter name on the left; e.g.

```
psredit -c dm=10.35 -e fix filename[s]
```

Here, the command `dm=10.35` sets the dispersion measure
attribute and `-e fix` is one of the
[standard output options](guide/design/options.shtml.html#output)
for naming the edited output file; in this case, the output file is written
with the extension replaced by `fix`.

As for queries, array notation can be used to set specific values in a vector quantity.
If only the parameter name is specified, then all elements of the vector will be set to the specified value.
For example, to zero the weight of channel zero in every sub-integration,

```
psredit -c 'int:wt[0]=0.0' -e zap file.ar
```

To zero the weight of every channel in only the first sub-integration

```
psredit -c 'int[0]:wt=0.0' -e zap file.ar
```

#### 2.3 Add sub-integration pointing parameters

The PSRFITS file format supports extending the `point` attribute of every sub-integration with additional parameters.
For example, to add a new parameter named `ha` (hour angle) to every sub-integration,
and write out a new file with the extension `wha`

```
psredit -c 'int:point:info=+ha' -c int:point:ha:desc="Hour angle in radians" -e wha file.ar
```

Subsequent queries of the sub-integration information will show the new parameter as an element with its own attributes

```
psredit -c int:help file.wha 
point:lst        Local sidereal time (seconds)
point:gb         Galactic longitude (deg)
point:gl         Galactic latitude (deg)
point:fa         Feed angle (deg)
point:pa         Position angle of the feed (deg)
point:va         Vertical (parallactic) angle (deg)
point:az         Telescope azimuth
point:zen        Telescope zenith
point:info       list additional info, or add/remove info
point:ha[:@]     Hour angle in radians
```

The attributes of the new hour angle parameter can be queried with `help`; e.g.

```
psredit -c int:point:ha:help file.wha 
name             name of the information
unit             physical unit of the information
desc             description of the information
value            numerical value of the information
```

The `name` and `desc` were set when the parameter was added.
The new parameter can be queried and modified like any other; e.g. to set the hour angle in every
sub-integration equal to 1.0

```
psredit -c int:point:ha=1.0 -m file.wha
```

Here, `-m` is one of the
[standard output options](guide/design/options.shtml.html#output)
for naming the edited output file; in this case, the output file overwrites
the input file.

#### 2.4 Add sub-integration extensions

To view the list of extensions that can be added to each sub-integration in a file, run

```
psredit -c int:ext=+help file.ar
Options:

aux
    dm               Auxiliary dispersion measure   0
    rm               Auxiliary rotation measure     0
```

Currently, there is only the `aux` extension, which can be used to store a unique
auxiliary dispersion measure and rotation measure for each sub-integration.
See [User's Guide: Cold Plasma Effects](guide/design/cold_plasma.md)
for more information.
To add an `aux` extension to each sub-integration in a file and set the auxiliary RM
in all sub-integrations to 0.5

```
psredit -c int:ext=+aux -c int:aux:rm=0.5 -e wrm file.ar
```

#### 2.5 Add archive extensions

To view the list of extensions that can be added to a file, run

```
psredit -a help file.ar
Options:

itrf
   ant_x            ITRF X coordinate.   0
   ant_y            ITRF Y coordinate.   0
   ant_z            ITRF Z coordinate.   0

aux
   dm_model         Auxiliary dispersion model
   dmc              Auxiliary dispersion corrected      0
   rm_model         Auxiliary birefringence model
   rmc              Auxiliary birefringence corrected   0
```

Currently, there are only the `itrf` and `aux` extensions.
The `itrf` extension can be used to store the ITRF coordinates of the observatory
and the `aux` extensions can be used to store information about the
auxiliary dispersion measure and rotation measure.
To add an `aux` extension to a file and set the auxiliary RM corrected flag,

```
psredit -a aux -c aux:rmc=1 -e wrm file.ar
```

#### 2.6 Remove archive extensions

The `-r` option is used to remove an extension from an archive by name; e.g.
to remove the auxiliary cold plasma extension from a file

```
psredit -r aux -e noaux file.ar
```

#### 2.7 Editing scripts

Multiple `psredit` commands can be combined into a single text file
and passed as a list of instructions using the `-C` command-line option;
e.g. to add the hour angle attribute to every sub-integration and set unique
values for each sub-integration

```
int:point:info=+ha
int:point:ha:desc="Hour angle in radians"
int[0]:point:ha=-1.0
int[1]:point:ha=-0.5
int[2]:point:ha=0.0
int[3]:point:ha=0.5
int[4]:point:ha=1.0
```

If the above text was saved to a file named set\_ha.ped, then the commands could be executed with

```
psredit -C set_ha.ped -e wha file.ar
```

### 3.0 Algorithms

For a description of the `TextInterface::Class`, please set
the [Building a text interface](../tutorial/interface.md)
tutorial.

### 4.0 Testing and examples

To add auxiliary cold plasma measures to every sub-integration and set unique
values for each of the 3 sub-integrations:

```
int:ext=+aux
int[0]:aux:rm=-1.0
int[0]:aux:dm=1.0e-4
int[1]:aux:rm=-0.5
int[1]:aux:dm=1.5e-4
int[2]:aux:rm=0.5
int[2]:aux:dm=2.0e-4
```

### 5.0 Known bugs and features that require implementation

* None at this time.
