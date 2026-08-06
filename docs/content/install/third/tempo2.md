# TEMPO2 Pulsar Timing Package

PSRCHIVE can make use of the TEMPO2 Pulsar Timing
Package to model pulse phase.

## TEMPO2 Environment Variable

Before installing tempo2, it is useful to choose an installation location
and set the TEMPO2 environment variable accordingly.
For example, in your .bashrc file:

```
export PSRHOME=$HOME/Pulsar
export TEMPO2=$PSRHOME/tempo2
export PATH=${PATH}:$TEMPO2/bin
```

## Manual Download and Installation

You can download and install TEMPO2 as described at the
[TEMPO2 home page](http://www.atnf.csiro.au/research/pulsar/tempo2).

## Automatic Installation

Alternatively, you can run the TEMPO2 installation script that is
distributed with the PSRCHIVE source code. From the top-level build
directory (where the `configure`  script was run) simply type

```
packages/tempo2.csh
```
