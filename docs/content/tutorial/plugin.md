# PSRCHIVE: Writing a File Format Plugin

**Writing a File Format Plugin**
To add a new file format to the list supported by the PSRCHIVE library, it is necessary to write a new
class that inherits the `Pulsar::Archive` base class. This
class must implement at least:

* constructors, destructors, and methods for copying and cloning;* methods for file loading and unloading; and* an advocate class

---

The **constructors, destructors, and methods for copying and
cloning** handle the initialization of any attributes that are
unique to the new file format, and ensure that these attributes are
properly copied whenever applicable. For an example of a
`Pulsar::Archive` derived plugin class, see the
`Pulsar::ExampleArchive` class in

```
Base/Classes/Pulsar/ExampleArchive.h
Base/Classes/ExampleArchive.C
```

In fact, you can most quickly start writing your new plugin class by
copying this file and editing the copy, replacing all instances of
`ExampleArchive` with the name of your new class.

---

The **methods for file loading and unloading** include the following:

```
  //! Load all header parameters from filename
  void load_header (const char* filename);

  //! Load the specified Integration from filename, returning new instance
  Integration* load_Integration (const char* filename, unsigned subint);

  //! Unload all header and Integration data to filename
  void unload_file (const char* filename) const;
```

Note that `load_Integration` returns a pointer to a newly
constructed instance of a `Pulsar::Integration` derived
class. The most commonly used child is the
`Pulsar::BasicIntegration` class. Also, if you are in a
hurry, you can get away without implementing the
`unload_file` method by writing an empty member function.
If you do not implement it, you simply will not be able to unload any
results of processing in the same file format as was loaded; however,
you could convert to one of the file formats that does have support
for unloading.

---

The **advocate** for your new class tells the PSRCHIVE library when your plugin should be used to
load a file. There are three rules for defining the advocate class:

1. the declaration of the class must be nested inside the definition of
   your plugin class,- the class must be named `Agent`, and- the class must inherit the `Pulsar::Archive::Advocate`
       template base class.

For example, suppose that your plugin class is named
`MyArchive`; the code that defines the advocate class would
look as follows:

```
//! PSRCHIVE plugin for my file format
class MyArchive : public Pulsar::Archive {

  // The main portion of your class definition ...

  protected:

  // Give the Advocate template class access to protected members
  friend class Archive::Advocate<MyArchive>;

  // Nested declaration of the advocate
  class Agent;

};

//! Advocates the use of the MyArchive plugin class
class MyArchive::Agent : public Archive::Advocate<MyArchive> {

  public:

  // This empty constructor is necessary
  Agent () { } 

  //! Return true if filename is in my file format
  bool advocate (const char* filename);

  //! Return the name of the MyArchive plugin
  string get_name () { return "MyArchive"; }
    
  //! Return description of this plugin
  string get_description () { return "My archive loader version 1.0"; }

};
```

The `MyArchive::Agent::advocate` method is
the one that determines when an file should be opened using your
plugin class.

---

The use of dynamic shared object (DSO) files to load plugins was
discontinued in earlier versions of the PSRCHIVE software. Consequently, it is necessary to
modify and recompile the PSRCHIVE library in
order to incorporate your new plugin class. The file to modify is
Base/Formats/Agent\_static.C; in this file you
must add lines like the following:

```
#include "MyArchive.h"

template
Registry::List<Pulsar::Archive::Agent>::Enter<MyArchive::Agent>
Pulsar::Archive::Advocate<MyArchive>::entry;
```

After modifying this file, you will need to recompile the PSRCHIVE code. After recompilation, you can check to
see that your plugin was successfully integrated by running:

```
psrchive_info
```

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
