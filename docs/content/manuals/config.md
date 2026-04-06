# PSRCHIVE documentation: psrchive_config

## PSRCHIVE documentation: **psrchive\_config**

### 1.0 Purpose

Various features of the PSRCHIVE library can be
configured using a simple text file. This page describes how to use
this configuration file and how the **psrchive\_config**
program can be used to generate and update the configuration file.

### 2.0 Usage

PSRCHIVE looks for a configuration file in
the following two locations:

```
$PSRCHIVE_CONFIG
$HOME/.psrchive.cfg
```

For example, the `$PSRCHIVE_CONFIG` environment variable may
be used to set the location of a configuration file that is shared by
a research group, and the `$HOME/.psrchive.cfg` configuration
file can be used by individual researchers.

Configuration files are optional and, if both of the above files are
found, they will be parsed in the above order such that options
configured in the $HOME directory file will over-ride those
found in $PSRCHIVE\_CONFIG.

Configuration options are specified as key = value pairs and
comments preceded by a hash (#) character are ignored.

### 2.1 Creating a configuration file

To create a template configuration file containing every available
configuration option, run **psrchive\_config**; e.g.

```
psrchive_config > $HOME/.psrchive.cfg
```

If this is the first time that the program is run, then all
configuration options will be commented and the file will have no
effect.
To set a configuration option, simply uncomment the line for the
key = value pair and set the value as required.

### 2.1 Updating a configuration file

Over time, new configuration options will be introduced. To update
your configuration file, simply run

```
psrchive_config > $HOME/.psrchive.cfg
```

Note that **psrchive\_config** will preserve all previously
set configuration options from both $PSRCHIVE\_CONFIG and
$HOME/.psrchive.cfg. These will appear in the output
configuration file uncommented.

### 3.0 Algorithms

PSRCHIVE configuration options are declared
using the `Pulsar::Config<>` template class, which is
documented in the [Adding a
Configuration Option](../tutorial/config.md) tutorial.

### 4.0 Testing and examples

To change the default width of the window used to find the off-pulse
baseline, look for the following lines in the template configuration
file

```
###########################################################################
#
# Profile::default_duty_cycle - Duty cycle of baseline [turns]
#
# The default width of the window used to find the off-pulse baseline
# and compute its statistics
#
# Profile::default_duty_cycle = 0.15
```

and change the last line to

```
Profile::default_duty_cycle = new value
```

### 5.0 Known bugs and features that require implementation

The **psrchive\_config** program will always print the
current state of the configuration options. That is, if either
$PSRCHIVE\_CONFIG or $HOME/.psrchive.cfg exist, then
they will always be incorported into the output of
**psrchive\_config**. This makes it impossible to print an
unmodified default template configuration file without first moving
the old configuration files out of the way.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
