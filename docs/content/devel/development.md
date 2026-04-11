# PSRCHIVE developer's guide

### Introduction

The PSRCHIVE software makes use of the GNU
autotools to compile the source code and install the header files,
scripts, libraries and binary executables.
An introduction to GNU autoconf, automake, and libtool is available as
[The Goat Book](http://sources.redhat.com/autobook).
Developers with an interest in writing robust and portable code may
also take interest in the
[GNU coding standards](http://www.gnu.org/prep/standards).
If you have been developing
with the old setup, it would be good to follow the notes on
[switching to the GNU autotools](switching.md).

---

### Check-out, bootstrap, configure, and compile

#### Check-out

Developers may check out the software from the SourceForge.net Git repo with

```
$ git clone ssh://USERNAME@git.code.sf.net/p/psrchive/code psrchive
```

where `USERNAME` is a valid PSRCHIVE sourceforge user account login id.
Upon successful checkout, a single directory, psrchive,
will be created in the current working directory. If this is the
first time you have checked out the software, the psrchive
directory will contain the following files and directories:

```
AUTHORS     config/       Doxyfile     psrchive_cflags.in*   Util/
Base/       configure.in  Makefile.am  psrchive_ldflags.in*
bootstrap*  COPYING       More/        README
ChangeLog   NEWS         upload.csh*
```

The directories shown in white are the **main** directories of the
PSRCHIVE software distribution.

#### Bootstrap

Before configuring and compiling, developers must create the
`configure` script and Makefile.in input files by
running the GNU build tools. This procedure is performed by the
`bootstrap` script, which must be run in the
psrchive directory:

```
cd psrchive
./bootstrap
```

If you encounter problems during bootstrap, please ensure
that your [GNU autotools](../third/autotools.md) are up-to-date.

#### Configure

After running bootstrap, the `configure` script is run to
determine the system configuration and produce the Makefiles.
If you plan to develop PSRCHIVE in parallel with
the software maintained at Swinburne or the ATNF, then please read these
important notes on [choosing an installation
directory](install.md).
If the configure script fails, please refer to the
[Common Problems](../problems.md).

#### Compile

In the top-level build directory (psrchive in most simple
cases), type `make` to compile and `make install` to
copy all products to the **install** directory. A complete list of
the standard make targets can be found in the [Standard Targets for Users](http://www.gnu.org/prep/standards/standards.html#Standard-Targets) section of the GNU coding standards.

In addition to the standard make targets, the following targets have
been defined in the **main** directories:

* libs compile and install only the library defined in the
  **main** directory
* include install only the header files defined in the
  **main** directory

For instructions on how to compile the rest of the Swinburne software
repository, please refer to the
[rest of the repository](switching.shtml.html#rest) notes in switching
to GNU autotools.

---

### Making additions

If you haven't already done so, please review the rules of thumb outlined
in the [design philosophy](design.md).

New code is added to the Makefile.am file in the relevant
sub-directory.

#### Choosing a sub-directory

Sub-directories of the GNU configured and built software tree are compiled
in a specific order. Therefore, it is not possible to compile code that
depends upon header files or libraries that are produced later in the list.
The order in which the sub-directories are compiled is as follows:

```
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

#### Modifying Makefile.am

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
[canonicalized](http://gnu.open-mirror.com/software/automake/manual/html_node/Canonicalization.md).

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
Automake will rebuild the Makefile [automatically](http://gnu.open-mirror.com/software/automake/manual/html_node/Rebuilding.md).

This should be enough information to get started. For further information,
please refer to the
[automake documentation](http://gnu.open-mirror.com/software/automake/manual/html_node).

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
