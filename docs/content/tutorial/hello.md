# Hello, pulsar!

In the tradition of computing language tutorials, the following
example provides a simple introduction to the PSRCHIVE library.

```
#include "Pulsar/Archive.h"

#include <iostream>

using namespace std;

int main (int argc, char** argv)
{
  if (argc < 2) {
    cerr << "Please specify a filename" << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> archive;

  archive = Pulsar::Archive::load (argv[1]);

  cout << "Hello, PSR " << archive->get_source() << "!" << endl;

  return 0;
}
```

Although this is one of the least useful programs that can be written, it
includes the basic components of every PSRCHIVE program:

**#include "Pulsar/Archive.h"**
:   This header file defines the `Pulsar::Archive`
    class; it also includes the headers of all other class definitions required
    in order to define this class.

**Reference::To<Pulsar::Archive> archive;**
:   The `Reference::To` template class is a smart
    pointer. It will automatically destroy an object if it is the last
    smart pointer that refers to that object when it is reset. Also, if
    the object to which a `Reference::To` points is
    deleted, the reference will be automatically invalidated.
    Note that it is not necessary to delete an object to which a
    `Reference::To` smart pointer points.

**archive = Pulsar::Archive::load (argv[1]);**
:   This line loads the data from the filename specified by the
    first argument to the program. `Pulsar::Archive::load`
    is known as a *factory*; it returns a newly constructed instance
    of the appropriate derived class (e.g.
    `Pulsar::FITSArchive`,
    `Pulsar::TimerArchive`, or
    `Pulsar::EPNArchive`) according to the
    format of the archive data in the specified file.

**cout << "Hello, PSR " << archive->get\_source() << "!" << endl;**
:   This line prints the name of the source loaded from the
    specified file. Notice that the program need know neither what type of file
    was loaded, nor what this attribute would have been named in the header
    of this file. The `Pulsar::Archive` base class
    defines a common interface to all file formats.
