# PSRCHIVE: CFITSIO bug

## CFITSIO Bug

PSRCHIVE makes use the CFITSIO library to read and
write PSRFITS files. There is a major bug in CFITSIO version 3.420 (and
probably most earlier versions of the library) that can lead to memory
violation.

In most cases, PSRCHIVE programs will complete
without any sign of error; however, in some cases, the program will
either fail with a misleading error message or enter an infinite loop
during which the output PSRFITS file grows until the process is killed
or the file system is filled.

The latest version of PSRCHIVE comes with a script
that will patch the bug in CFITSIO. To install the patched CFITSIO library,
run the following

```
cd psrchive
./update
if you build in a different directory, then "cd $buildir"
cd packages
make
./cfitsio.csh
```

When the script has finished installing, configure, compile, and install PSRCHIVE; e.g.

```
cd ..
./configure [plus your favourite arguments]
make clean
make
make install
```

Verify that PSRCHIVE programs have linked against
the newly installed CFITSIO library with something like the following:

```
ldd `which psredit` | grep fits
libcfitsio.so.5 => /home/pulsar/lib/libcfitsio.so.5 (0x00002aaaaae28000)
ls -l /home/pulsar/lib/libcfitsio.so.5
lrwxrwxrwx 1 pulsar pulsar 20 Sep 28 14:32 /home/pulsar/lib/libcfitsio.so.5 -> libcfitsio.so.5.3.42
```

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
