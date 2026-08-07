# Quick Start

## Key command-line tools

[`psredit`](content/manuals/psredit.md)
:   Edit or query metadata.

[`psrstat`](content/manuals/psrstat.md)
:   Query metadata and derived statistical quantities.

[`pam`](content/manuals/pam.md)
:   General-purpose processing (dedisperse, integrate, etc.).

[`psrsh`](content/manuals/psrsh.md)
:   Command language interpreter for interactive or scripted processing.

[`paz`](content/manuals/paz.md)
:   Zap (flag) frequency channels or sub-integrations affected by RFI.

[`psradd`](content/manuals/psradd.md)
:   Combine data in various ways.

[`pat`](content/manuals/pat.md)
:   Generate pulse times of arrival (TOAs) for pulsar timing.

See more at [Application Manuals](content/manuals).

### Example script

```bash
# Inspect metadata parameters
psredit -c "name,freq,bw,nchan,nbin,length" observation.ar

# Dedisperse, integrate in frequency and polarization, and overwrite the file
pam -d -F -p -m observation.ar

# Generate an arrival time
pat -f tempo2 -s template.std observation.ar
```

## Software Interface

The primary entry point for most applications that use PSRCHIVE is the
`Pulsar::Archive` class.  Use the `load` factory to open any
supported file format:

### C++ Example

```c++
#include "Pulsar/Archive.h"

auto archive = Pulsar::Archive::load("observation.ar");
archive->fscrunch();       // Integrate over frequency
archive->tscrunch();       // Integrate over time
archive->pscrunch();       // Form total intensity

auto nbin   = archive->get_nbin();
auto nchan  = archive->get_nchan();
auto source = archive->get_source();
```

See more at [Application Programming Interface (API)](api/index).

### Python Example

The above example is equivalent to

```python
import psrchive

archive = psrchive.Archive_load("observation.ar")
archive.fscrunch()
archive.tscrunch()
archive.pscrunch()

nbin   = archive.get_nbin()
nchan  = archive.get_nchan()
source = archive.get_source()
```

See more at [Python Interface](content/manuals/python).