# PSRCHIVE: switching to SourceForge

### Introduction

This web-page is intended to help those with Swinburne CVS access to
`soft_swin` continue to develop in parallel with the PSRCHIVE software hosted at SourceForge. Please read
this page completely before following its instructions.

---

### Step One: Don't Panic

The Standard Makefile Scheme is once again used to compile the
software in the Swinburne CVS repository. As a result, it will be
necessary to clean out the old GNU autotools files from
soft\_swin. It may also be necessary to merge your changes
from soft\_swin with the SourceForge distribution. These
instructions will help.

---

### Step Two: Install the New

Most pulsar software developers at Swinburne and ATNF will have set the
$CVSHOME and $LOGIN\_ARCH enviroment variables.
It is strongly recommended that you:

1. Check out the new PSRCHIVE software into your
   $CVSHOME directory; and- Set the $CONFIG\_SITE environment variable to ensure that
     PSRCHIVE is installed in $CVSHOME.

In short, type

```
cd $CVSHOME
setenv CONFIG_SITE $CVSHOME/psrchive/config/cvshome
```

then checkout and build PSRCHIVE as described
in the [CVS Access Instructions](../cvs.md).

If you plan to continue doing most of your development in
$CVSHOME, then you should set the $CONFIG\_SITE
environment variable in your .cshrc file:

For more information on choosing and setting the installation directory,
please read the [Installation
Directory](install.md) notes.

---

### Step Three: Clean up the Old

To clean up the old Swinburne repository and continue developing
where you left off, run the following commands:

```
cd $CVSHOME/soft_swin
make distclean
cvs update autoclean sfmerge
./autoclean
cvs update -AP |& tee update.log
```

The `make distclean` step may fail if you have previously run
`cvs update` in soft\_swin. Don't worry about it;
the `autoclean` script will clean up whatever
autotools-generated files are left behind.

---

### Step Four: Merge Any Modifications

The `cvs update` performed in the previous step may have produced
some conflicts. For example:

```
grep conflict update.log

cvs update: conflict: `psrchive/SquareWave.C' is modified but no longer in the repository
C psrchive/SquareWave.C
```

This message means that a modified file has been moved to SourceForge.
Continuing the example, run

```
cvs diff psrchive/SquareWave.C
```

to determine the nature of the modifications and decide if they should
be merged with the SourceForge distribution. To merge the changes, use
the `sfmerge` script, as in the following example:

```
./sfmerge $CVSHOME/psrchive psrchive/SquareWave.C

Merging psrchive/SquareWave.C into /psr/cvshome/wvanstra/psrchive/More/General/SquareWave.C
```

After merging any modifications, you may need to rebuild PSRCHIVE.

---

### Step Five: Compile

Just run `make`.

If you have any difficulties with compiling, please contact Willem.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
