
# Version Control

To use the current stable version of the PSRCHIVE
system installation, simply run

```sh
source $PSRCHIVE_SYSTEM/stable/share/psrchive_version
```

This script will modify your PATH environment variable
and setup an alias command for version selection.

Continuing the example above, you might add the following to
your .cshrc file:

```sh
setenv PSRCHIVE_SYSTEM /nfs/pulsar/linux/psrchive
if (-f $PSRCHIVE_SYSTEM/stable/share/psrchive_version) then
  source $PSRCHIVE_SYSTEM/stable/share/psrchive_version
endif
```

After sourcing the psrchive\_version script, a new alias
of the same name will be available. With this command, it is
possible to query the system version; e.g.

```sh
psrchive_version
PSRCHIVE System: /nfs/pulsar/linux/psrchive Version: stable
```

and set the system version; e.g

```sh
psrchive_version 6.1
```

After setting the system version, the PATH environment
variable will be set accordingly.
