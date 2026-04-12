# PSRCHIVE Design Philosophy

Modularity and extensibility are desireable software design features;
but what does this mean, on a *practical* level, when
developing PSRCHIVE? The following handy rules
of thumb can help when making design decisions:

1. [Avoid adding methods](design.shtml.html#methods) to the container classes
   (Archive, Integration, Profile)
2. [Implement algorithms as classes](design.shtml.html#classes)
3. [Use extensions](design.shtml.html#extensions) to customize functionality

---

### Avoid adding new methods to container classes

As much as possible, avoid the urge to add new methods to the
container classes.
Container classes include `Pulsar::Archive`,
`Pulsar::Integration`, and
`Pulsar::Profile`.
In an [ideal world](../disclaimer.md), these containers would
implement only the methods required to manage their data (e.g. resize, load,
unload, append). However, many commonly used algorithms have already been
implemented as member functions
(e.g. `Archive::toas`,
`Integration::dedisperse`, and
`Profile::stats`).

The imprudent addition of member functions causes a class to bloat
over time, making it harder to understand its purpose or relationship
with the rest of the software. It also results in much more time
spent compiling the software, since all code that uses a class will
need to be recompiled when it is modified.

If your new algorithm must be declared as a friend, or if it requires
access to private members of the container classes, then it may be best
implemented as a member function. This is generally true of any method
that must resize the container.

---

### Implement algorithms as classes

There are a number of reasons to implement an algorithm as a class:

**1) Clarity of code:** Many well-designed algorithms support a
variety of options for customization. These options can either be
passed as arguments to a function or be set as attributes of a class.
The following example demonstrates the descriptive power of the latter
option:

```
// Call a function to compute S/N

double SNR = snr (archive, 3.0, 0.1, false);
```

versus

```
// Use a well-designed class to compute S/N

Pulsar::SignalToNoise snr;

snr.set_threshold( 3.0 );
snr.set_duty_cycle( 0.1 );
snr.set_use_weights( false );

double SNR = snr.get_snr( archive );
```

Although the function requires only one line of code, the meaning of
that line is not at all clear until reference is made to the manual.
A good programmer would have documented the meanings of each of the
arguments using internal comments; but we are not always good.

The interface to the well-designed class introduces a form of
self-documentation. Although it required a little more typing to
achieve the same result, the meaning of your code will become a little
more clear to everyone else (including yourself in a few months time).

**2) Inheritance:** Even if you don't realize it at the time, your
algorithm may belong to a family of algorithms. By implementing it as
a class, you open up the possibility of having your class inherited
and/or incorporated into a family tree of similar classes with a
similar purpose. This increases the opportunity for code re-use, and
can often help others to better understand where your code fits into
the big picture.

**3) Optimization:** Each time you call a function, it must perform
all of its computations given the information provided as arguments.
Each time you call the member function of a class, it can make use of
the results of computations already stored in its attributes. A
simple example of this principle is found in the family of
`Pulsar::Calibrator` classes. These construct an array of
polarimetric transformations for each frequency channel and store the
results internally. These transformations can be used on multiple
`Pulsar::Archive` instances without the need for
recomputation.

---

### Use extensions to customize functionality

If an algorithm must be adapted according to the type of the
container, then first consider adding an Extension class to implement
this change. For example, the `FITSArchive` class can
store its integrations as a function of binary phase. Rather than
over-riding the `Archive::append` method, the functionality
required to integrate profiles as an arbitrary function is
encapsulated in the `IntegrationOrder` extension class.
This class can also be used by other container class that can support
this feature.

The use of Extension classes enables the modular composition of
different behaviours into a class. Without this design technique, it
would be necessary to either duplicate the code or multiply inherit
from different base classes that provide the different pieces of
functionality.
