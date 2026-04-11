# config

**Adding a Configuration Option**
A number of PSRCHIVE classes have default
behaviours that can be configured using a configuration file, as
described in the [psrchive\_config
manual](../manuals/config.md).

This tutorial describes how to add new configuration options using
the `Pulsar::Config<>` template class.

Begin by choosing a name for the new configuration option, which
cannot be equal to the name of an existing option. There are three
ways to add the new option:

1. direct use of the `Pulsar::Config<>` template
2. indirect use of the `Pulsar::Config<>` template via a pointer
3. indirect use of the `Pulsar::Config<>` template via a `CommandParser`.

---

# Direct Use

To serve as an example, consider adding a new configuration option to
set the default value of `threshold`, a floating point
attribute of a class named `Prune`. In the declaration of
the `Prune` class:

```
class Prune
{
  public:

    // 1. Declare a static member, default_threshold
    static Pulsar::Option<float> default_threshold;

    // 2. Set threshold to default in constructor
    Prune ()  { threshold = default_threshold; }

  protected:

    // 3. Declare threshold attributed
    float threshold;
};
```

The static member, `Prune::default_threshold` must be defined
in a separate source code file; e.g. in Prune.C.

```
#include "Pulsar/Config.h"

Pulsar::Option<float> Prune:default_threshold
(
 "Prune::default_threshold", 3.0,

 "Threshold above which branches are pruned [metres]",

 "The default maximum length of a branch, beyond which the branch will\n"
 "be pruned using the Prune algorithm"
);
```

---

### Indirect Use via Pointer

The above solution may be too invasive for some applications. For example,
it may be desireable to set the default value of a variable belonging to
a third party library or some other code that knows nothing about
`Pulsar/Config.h`. It is still possible to set such variables
using pointers, as in the following example:

```
#include "Pulsar/Config.h"

Pulsar::Option<std::string> third_log_filename_config
(
 &third_log_filename,

 "third_log_filename", "$HOME/third.log",

 "Default name of third party library log file"
);
```

---

### Indirect Use via CommandParser

not finished

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
