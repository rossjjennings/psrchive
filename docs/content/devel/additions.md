
# Making additions

If you haven't already done so, please review the rules of thumb outlined
in the [design philosophy](design.md).

New code is added to the Makefile.am file in the relevant sub-directory.

## Choosing a sub-directory

As described in more detail in [architecture](architecture.md),
sub-directories are compiled in a specific order. Therefore, it is not possible 
to introduce new code that
depends upon header files or libraries that are produced later in the list.
The order in which the sub-directories are compiled is as follows:

```text
Util/units
Util/third
Util/genutil
Util/fft
Util/fitsutil
Util/tempo
Util/pgutil
Util/qtutil

Base/Classes
Base/Extensions
Base/Formats
Base/Applications

More/General
More/MEAL
More/Timing
More/Polarimetry
More/Plotting
More/Applications
More/rhythm
```

## Modifying Makefile.am

The syntax of the `automake` input file, Makefile.am,
is fairly straight-forward. There are basically four main targets:

**header files**: are defined by the include\_HEADERS
macro. If you want your headers to be installed in sub-directories
of the include/ directory, use the nobase prefix,
e.g.

```
nobase_include_HEADERS = Pulsar/Archive.h model_profile.h
```

**libraries**: are defined by the lib\_LTLIBRARIES.
Only the main directories, utils, astro,
and psrchive produce libraries that may be used during
linking. The sub-directories produce convenience libraries.
Therefore, their libraries are defined with the noinst
prefix, e.g.

```
noinst_LTLIBRARIES = libunits.la
```

Notice that the library is given the .la extenstion.
This is used by `libtool`, which will place an appropriate
extension on the output library, depending on the architecture
and on usage of static or dynamic libraries.

In addition to defining the name of the library, it is also necessary
to define its contents. This is done with the SOURCES suffix.
For example:

```
libunits_la_SOURCES = Error.C HeapTracked.C ReferenceAble.C
```

Note how the name of the library has been
[canonicalized](https://www.gnu.org/software/automake/manual/html_node/Canonicalization.html).

**executables**: are divided into two categories.
Programs listed in the bin\_PROGRAMS macro are compiled and installed
by default. Programs listed in the check\_PROGRAMS are compiled
only if the user types `make check` are are never installed.
Only the program name is specifed in a PROGRAMS macro definition,
e.g.

```
bin_PROGRAMS = psradd psredit pac
```

The source code that gets compiled into each program is specified as in
the case of libraries, for example:

```
psradd_SOURCES  = psradd.C
psredit_SOURCES = psredit.C
pac_SOURCES     = pac.C
```

After modifying Makefile.am, simply type `make`.
Automake will rebuild the Makefile [automatically](https://www.gnu.org/software/automake/manual/html_node/Rebuilding.html).

This should be enough information to get started. For further information,
please refer to the [automake documentation](http://www.gnu.com/software/automake/manual/html_node).
