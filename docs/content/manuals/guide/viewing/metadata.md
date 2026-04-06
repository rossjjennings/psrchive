# User's Guide: Reading and Writing Metadata

The [`psredit`](../../psredit.md) program is used
to read and write metadata in an archive. To see all of the
single-valued data in an archive; e.g.

```
psredit pulse_473145330.rf.it
Attribute Name   Description                                       Value
------------------------------------------------------------------------
file             Name of the file                                  pulse_473145330.rf.it
nbin             Number of pulse phase bins                        8192
nchan            Number of frequency channels                      8
npol             Number of polarizations                           4
nsubint          Number of sub-integrations                        1
type             Observation type (Pulsar, PolnCal, etc.)          Pulsar
site             Telescope name                                    Hobart
name             Name of the source                                J0835-4510
coord            Coordinates of the source                         00:00:00.000+00:00:00.00
freq             Centre frequency (MHz)                            1440
bw               Bandwidth (MHz)                                   -64
dm               Dispersion measure (pc/cm^3)                      68.1507050520395
[...]
```

This example list is cut short, but is long enough to see that the
coordinates of the source have not been set. To set the coordinates
in the archive:

```
psredit -c coord=08:35:20.6115-45:10:34.875 -m pulse_473145330.rf.it
pulse_473145330.rf.it
pulse_473145330.rf.it updated on disk
```

And to print only the coordinates:

```
psredit -c coord=08:35:20.6115-45:10:34.875 -m pulse_473145330.rf.it
pulse_473145330.rf.it coord=08:35:20.612-45:10:34.88
```

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
