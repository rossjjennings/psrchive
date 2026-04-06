# Quick Start

## Loading an archive

The primary entry point for most work with PSRCHIVE is the
`Pulsar::Archive` class.  Use the static `load` factory to open any
supported file format:

```c++
#include "Pulsar/Archive.h"

auto archive = Pulsar::Archive::load("observation.ar");
archive->fscrunch();       // Integrate over frequency
archive->tscrunch();       // Integrate over time
archive->pscrunch();       // Form total intensity
```

## Key command-line tools

`pam`
:   General-purpose archive manipulation (scrunch, calibrate, dedisperse).

`vap`
:   Print archive header parameters in tabular form.

`psrsh`
:   Interactive scripting shell for PSRCHIVE operations.

`paz`
:   Zap (remove) frequency channels or sub-integrations affected by RFI.

`pat`
:   Generate pulse times of arrival (TOAs) for pulsar timing.

`psredit`
:   Edit or query archive metadata.

`psrconv`
:   Convert between archive file formats.

## Example workflow

```bash
# Inspect archive parameters
vap -c "name,freq,bw,nchan,nbin,length" observation.ar

# Dedisperse, scrunch in frequency and polarization
pam -d -F -p observation.ar

# Generate a TOA
pat -f tempo2 -s template.std observation.ar
```

## Python interface

When built with `--enable-shared` and SWIG, PSRCHIVE provides Python
bindings:

```python
import psrchive

archive = psrchive.Archive_load("observation.ar")
archive.fscrunch()
archive.tscrunch()

nbin   = archive.get_nbin()
nchan  = archive.get_nchan()
source = archive.get_source()
```
