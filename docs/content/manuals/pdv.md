# pdv

### 1.0 Purpose

The Pulsar Data Viewer, `pdv`, is designed to display some basic
archive data. Including

* Pulse widths and mean flux density
* Polarization summary
* raw profile data as ascii

### 2.0 Usage

`pdv` is very simple to use, it accepts the following arguments.

``` -b ibin ```
:   Select a single phase bin, from 0 to (nbin - 1)

``` -n ichan ```
:   Select a single frequency channel, from 0 to (nchan - 1)

``` -i isub ```
:   Select a single integration, from 0 to (subint - 1)

``` -r phase ```
:   Rotate the profiles by phase before printing

``` -F ```
:   Fscrunch first

``` -T ```
:   TScrunch first

``` -p ```
:   Pscrunch first

``` -C ```
:   Centre first

``` -B factor ```
:   BScrunch by this factor first

``` -x ```
:   Convert to Stokes and also print fraction polarisation

``` -y ```
:   Convert to Stokes and also print fraction linear

``` -z ```
:   Convert to Stokes and also print fraction circular

``` -Z ```
:   Convert to Stokes and also print position angle

``` -c ```
:   Print out calibrator (square wave) parameters

``` -f dcyc ```
:   Show pulse widths and mean flux density (mJy) with baseline width dcyc

``` -I ```
:   Print mean flux density

``` -R ```
:   Do not remove baseline

``` -t ```
:   Print out profiles as ASCII text

``` -A ```
:   Print out profiles as ASCII text ( with per channel headers )

``` -S params ```
:   Print out per subint data ( -h S to list valid params )

``` -H params ```
:   Print out the history table for the archive ( -h S to list valid params )

``` -h arg ```
:   Print out more detailed help for program arguments. Currently lists all available options for H and S

``` -N ```
:   Print the S/N

### 3.0 Testing and examples

#### 3.1 History table

To display the processing command used at each revision of the file and the centre frequencies obtained at each step

```
 pdv -H ctr_freq,proc_cmd *files*
```

#### 3.2 Subint parameters

To display the subint parameters from each subint, showing the tsubint and offs\_sub

```
 pdv -S TSUBINT,offs_sub *files*
```

### 4.0 Known bugs and features that require implementation

* None at this time.
