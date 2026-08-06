# psrstat

## Selecting, configuring, executing, and querying algorithms

Some [algorithms](algorithms.md) can be selected, configured,
executed, and queried using syntax that is similar to that used for
other variables. (For a complete list,
see [Configurable Algorithms and Options](algorithms.md).)

For example, to see the configuration of
the algorithms used to identify the on- and off-pulse regions

```
psrstat -c on,off filename[s]
```

The algorithms can also be selected and configured using commands of
the following form

```
psrstat -c ALGORITHM=NAME:OPTIONS filename[s]
```

where `ALGORITHM`  is the algorithm to be configured,
`NAME`  is any valid algorithm name
and `OPTIONS`  is a comma-separated list of options. For
example, to set the on-pulse region to all phase bins greater than
five sigma above the mean off-pulse noise:

```
psrstat -c on=above:threshold=5 filename[s]
```

To see all available on-pulse identificaiton algorithms and their
options, type

```
psrstat -c on=help
```

The signal-to-noise ratio estimators typically use their own algorithms to
find the on-pulse and off-pulse regions, and some of the
signal-to-noise algorithms allow these to be configured. For example,
the following psrstat command line does the following:

1. sets the signal-to-noise ratio estimator to the
   `modular` algorithm, which allows both on- and off-pulse
   identification algorithms to be set;
2. sets the on-pulse estimator to the `minimum` algorithm,
   which searches for the minimum in a boxcar smoothed version of the
   profile;
3. configures the on-pulse estimator to search for the maximum
   instead of the minimum (this algorithm is more typically used to
   identify the off-pulse baseline); and
4. sets the duty cycle of the smoothing function used by the
   on-pulse estimator to 5%.

After configuring the signal-to-noise ratio
estimator, `psrstat` then queries the signal-to-noise ratio
and queries the region identified by the on-pulse estimator.

```
psrstat -c 'snr=modular:{on=minimum:{find_min=0,smooth:width=0.05}}' -c snr -c snr:on:found filename[s]
```

Instead of working completely on the command line, it is also possible
to specify a text file of psrstat commands. For example, the
above command line that sets, configures, and queries the
signal-to-noise ratio estimator is equivalent to running the following
script.

```
#!/usr/bin/env psrstat -C
# 
# snr.psh
#

# choose the ModularSNR signal-to-noise ratio estimator
snr=modular

# choose the BaselineWindow on-pulse estimator
snr:on=minimum

# configure BaselineWindow to search for the maximum
snr:on:find_min=0

# set the width of the smoothing window
snr:on:smooth:width=0.05%turn

# execute the algorithm and print the S/N
snr

# query the selected range of pulse phase
snr:on:found
```

Note that
[some flavours of linux do not support command line arguments in the
command passed to `env`](http://unix.stackexchange.com/questions/63979/shebang-line-with-usr-bin-env-command-argument-fails-on-linux). In this case, execute the above
script using

```
psrstat -C snr.psh filename[s]
```

The `snr.psh` script is equivalent to the following C++ code

```
//
// snr.cpp
//

#include "Pulsar/Archive.h"
#include "Pulsar/ModularSNR.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/Smooth.h"

#include 

using namespace std;
int main (int argc, char** argv)
{
  // psrstat loads the archive before executing the script
  Reference::To<Pulsar::Archive> archive;
  archive = Pulsar::Archive::load (argv[1]);

  // choose the ModularSNR signal-to-noise ratio estimator
  Reference::To<Pulsar::ModularSNR> modular = new Pulsar::ModularSNR;

  // choose the BaselineWindow on-pulse estimator
  Reference::To<Pulsar::BaselineWindow> window = new Pulsar::BaselineWindow;
  modular->set_onpulse_estimator( window );

  // configure BaselineWindow to search for the maximum
  window->set_find_minimum( false );

  // set the width of the smoothing window
  window->get_smooth()->set_width (0.05);

  // execute the algorithm and print the S/N
  cout << "snr=" << modular->get_snr( archive->get_Profile(0,0,0) ) << endl;

  // query the selected range of pulse phase
  cout << "snr:on:found=" << window->get_found_range() << endl;

  return 0;
}
```

The above code can be compiled and executed as described at
[How to compile](../../tutorial/compile.md).
