# PSRCHIVE: Structure of an Archive

**Structure of an Archive**
The `Pulsar::Archive` class is a data container that holds

* an array of integrations,* common header parameters, and* any additional extensions.

---

The **array of integrations** stores the observed pulsar data, and
is implemented as a vector of pointers to
`Pulsar::Integration` instances, typically ordered as a
function of observed epoch. Each `Pulsar::Integration` is
in turn a two-dimensional container of pointers to
`Pulsar::Profile` instances, typically ordered as a
function of polarization and observed frequency. Each
`Pulsar::Profile` is an array of floating point values,
typically flux densities, stored as function of pulse phase.

The following example demonstrates this structure:

```
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

[...]

  Reference::To<Pulsar::Archive> archive;
  archive = Pulsar::Archive::load( filename );

  for (unsigned isub=0; isub < archive->get_nsubint(); isub++)
  {
    Pulsar::Integration* integration = archive->get_Integration(isub);

    for (unsigned ipol=0; ipol < integration->get_npol(); ipol++)
      for (unsigned ichan=0; ichan < integration->get_nchan(); ichan++)
      {
        Pulsar::Profile* profile = integration->get_Profile (ipol, ichan);
        float* amps = profile->get_amps();

        for (unsigned ibin=0; ibin < profile->get_nbin(); ibin++)
          cerr << amps[ibin] << endl;
      }
  }
```

**Note:** to access the methods of any class, it is necessary to
`#include` the header file that defines that class.
Including only as much header information as is needed to compile the
code reduces the amount of recompilation required when header files
are modified.

---

The **common header parameters** include the minimal set of
information required in order for the `Pulsar::Archive`,
`Pulsar::Integration`, and `Pulsar::Profile`
container classes to perform their functions. They include things
such as the centre frequency of the observation, the dispersion
measure, the epoch, etc.

Like most other class attributes, header parameters may be accessed
via methods that start with `set` and
`get`, as in the following example:

```
  // enable direct access to definitions in the Pulsar namespace ...
  using namespace Pulsar;

  // ... so that, for example, Pulsar::Archive -> Archive
  Reference::To<Archive> archive = Archive::load (filename);

  double RM = archive -> get_rotation_measure ();
  archive -> set_source ("0437-4715");
```

Note that all of the common header parameters defined by an instance
of the `Pulsar::Archive` base class are shared by all of
the `Pulsar::Integration` instances contained by that
`Pulsar::Archive`. For example, it is not possible to mix
`Pulsar::Integration` instances with different centre
frequencies or different source names into one
`Pulsar::Archive`.

---

Any number of **additional extensions** may be used to supplement
the information and/or functionality of the
`Pulsar::Archive` container. Extensions are implemented as
a class that inherits the `Pulsar::Archive::Extension` base
class; they typically provide access to information not included in
the common header parameters and/or implement algorithms not included
in the container class definitions.

An example is the `Pulsar::Receiver` class, which stores
information about the receiver used for the observation, such as its
name and the polarization of its receptors. This class can also
compute the Jones matrix that describes the polarimetric
transformation introduced by the receiver feed. Access to
`Pulsar::Archive::Extension` derived classes is provided by
the `Pulsar::Archive::get<NAME>` template
member function, where `NAME` is the name of the
desired class, as demonstrated in the following example:

```
  using namespace Pulsar;

  Reference::To<Archive> archive = Archive::load (filename);

  Reference::To<Receiver> receiver = archive->get<Receiver>();

  if (receiver->get_basis() == Signal::Circular)
    cerr << "Receptors are circularly polarized" << endl;
  else if (receiver->get_basis() == Signal::Linear)
    cerr << "Receptors are linearly polarized" << endl;
  else
    cerr << "Receptors are elliptically polarized?" << endl;
```

Note that a `Pulsar::Archive` can store only one
instance of a given extension class.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
