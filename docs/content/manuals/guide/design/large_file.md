# PSRCHIVE User's Guide : Design

## User's Guide: Large File Support

Some instruments create files that are so large they cannot be loaded
without consuming all physical and virtual memory.

To work around this, the PSRCHIVE library
provides a simple dynamic memory manager that uses the kernel page
swapping algorithm to store virtual memory in a file specified by the
user instead of the default swap device.

To enable this feature, the base name of the file to be used as swap
space must be set in the psrchive configuration file, as shown below.

```
###########################################################################
#
# Profile::swap - Virtual memory swap space filename base
#
# The Profile class can use an alternative virtual memory manager so that
# the kernel swap space is not consumed when processing large files. 
# 
# This configuration parameter should be set to the full path of a file
# to be used for page swapping; e.g. "/tmp/psrchive.swap"
# 
# The filename will have a unique extension added so that multiple
# processes will not conflict.
#

Profile::swap = /tmp/psrchive.swap
```

Note that no two processes will use the same swap file. Each process
will generate a unique filename using the standard C function,
mkstemp. This file is immediately unlinked so that it will
be removed when the process exits, however it exits.

Only the profile amplitudes will be allocated using the alternative
memory manager and paged in and out of the custom swap space. All
other data and metadata will be allocated in the normal way and paged
using kernel swap space as necessary.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
