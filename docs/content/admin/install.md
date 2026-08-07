
# System Installation

At sites where some level of stability is important, it is useful to
maintain previous working versions of compiled binaries and revert to
these if the development version is corrupted or lost.

The PSRCHIVE distribution comes with two
useful scripts to aid in the management of a system installation
directory. These are found in

```
Management/psrchive_install
Management/psrchive_version
```

Both of these scripts make use of the PSRCHIVE\_SYSTEM
environment variable, which must be set to the root directory
in which you wish to install the version control system. For
example:

```
setenv PSRCHIVE_SYSTEM /nfs/pulsar/linux/psrchive
```

After setting this environment variable, a PSRCHIVE
Release may be installed with the command

```
psrchive_install VER
```

where *VER* is the SourceForge Release Name (version number).
The `psrchive_install` script will

1. download psrchive-VER.tar.gz from the nearest SourceForge mirror;
2. unpack and run `configure`, `make`, `make check`, and `make install`;
3. install a read-only copy of the software in $PSRCHIVE\_SYSTEM/$VER;
4. update the stable symbolic link to point to the new installation; and
5. if the PSRCHIVE\_SYSGRP environment variable is set, then
   the group ownership of the newly installed files will be set to it.

The `psrchive_install` script uses `curl` to download
the tarball. If this step fails, you may have to set the http\_proxy
environment variable.
