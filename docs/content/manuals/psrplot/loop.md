# PSRCHIVE documentation: psrplot looping over data dimensions

## PSRCHIVE documentation: **psrplot** looping over data dimensions

Some plots support specification of an index along a certain dimension
of the data to be plotted. For example, the simple `flux`
plot supports specification of the `subint`, `chan`,
or `pol` of the profile to be plotted.

**psrplot** can loop over one or more of these
dimensions, producing a new plot at each step in the loop,
using the `-l index=range` command line option one or
more times. For example,

```
psrplot -p flux -l pol=2,3 -l chan=11-15,63- filename.ar
```

will plot the pulse profile from polarizations 2 and 3 for each of the
frequency channels from 11 to 15 (inclusive) and from 63 to the last
channel. The index specified first will be the most rapidly changing
from plot to plot.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
