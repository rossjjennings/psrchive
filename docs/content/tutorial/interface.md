# PSRCHIVE: Building a text interface

**Building a text interface**
Sometimes it is useful to enable the configuration of a class
through a text interface. For example, text parsed from the command
line may be used to set the attributes of a class; also, a configuration
file may be used to set default values or load a set of commonly used
parameters. Finally, by making use of the same text interface,
a graphical user interface can be built in a modular fashion.

To aid in the construction of text interfaces, a template class named
`TextInterface::To<T>` has been developed. Here,
`T` is the name of the class for which you want to build an
interface, e.g. `MyClass`. Begin by creating a new class
that inherits `TextInterface::To<T>`; e.g.

```
  class MyClassInterface : public TextInterface::To<MyClass> {

  public:
    //! Default constructor
    MyClassInterface (MyClass* instance = 0);

  };
```

This is probably all that you will ever need to declare in the
interface class. All of the work is done in the constructor; here,
new attribute interfaces can be added, and text interfaces from other
classes can be imported. It is not mandatory to accept a pointer to
`MyClass` in the argument to the constructor; the
usefulness of this feature will be demonstrated later.

---

### Adding Attributes

To add an interface to an attribute of a class, there must be separate
methods that get and (optionally) set the value of this attribute.
Continuing with the preceding example, suppose that
`MyClass` has three attributes, which are accessed via the
following methods:

```
  std::string get_name () const;
  void set_name (const std::string&);

  double get_max () const;

  int get_code () const;
  void set_code (int);
```

Notice that there is no `set_max` method; this attribute is
read only. To create the interfaces to these attributes, the
`MyClassInterface` constructor would look something like:

```
MyClassInterface::MyClassInterface (MyClass* instance)
{
  add( &MyClass::get_name,
       &MyClass::set_name,
       "name", "Name of the source of data" );

  add( &MyClass::get_max,
       "max", "Maximum value");

  add( &MyClass::get_code,
       &MyClass::set_code,
       "code", "Observation code" );

  if (instance)
    set_instance (instance);
}
```

The arguments to the
`TextInterface::To<MyClass>::add` method are pointers
to member functions of `MyClass`, followed by the text name
of the attribute and (optionally) a short description of the
attribute. Two important things to note:

* Methods that get the value of an attribute receive no arguments and
  must be `const`; ie.

  ```
    T get_attribute () const;
  ```

  * Methods that set the value of an attribute return no value, are
    not `const`, and accept a single argument
    either by value or by `const` reference, ie.

    ```
      void set_attribute (T);          // by value
      void set_attribute (const T&);   // by const reference
    ```

---

### Arrays of Values

To add an interface to an array of values managed by a class, there
must be separate methods that report the size of the array, as well as
get, and (optionally) set the values of the elements in this array.
For example, suppose that `MyClass` has an array of
frequencies:

```
  double get_frequency (unsigned ichan) const;
  void set_frequency (unsigned ichan, double);
  unsigned get_nchan () const;
```

To create the interface to this array, the
`MyClassInterface` constructor would include something like:

```
  VGenerator<double> generator;
  add(generator( "freq", "Frequency of each channel",
		 &MyClass::get_frequency,
		 &MyClass::set_frequency,
		 &MyClass::get_nchan ));
```

Note that `VGenerator` is a template with its argument set
to the return type of the `get_frequency` method.

---

### Importing Text Interfaces

When the class for which an interface is being constructed inherits
another class and/or is composed of one or more other classes, it
proves useful to incorporate the text interfaces to the parent and/or
other classes into the new class interface. This is done in the
constructor of the new class interface using the
`TextInterface::To<MyClass>::import` method. It is
currently possible to import the text interface of:

* a [parent](interface.shtml.html#parent),* a [member](interface.shtml.html#member),* the elements of a [vector](interface.shtml.html#vector), or* the data in a [map](interface.shtml.html#map).

**Inheriting the Parent interface**

For example, suppose that a class named `Child` inherits a
class named `Parent`, and that `Parent` has
defined its text interface in a nested sub-class named
`Parent::Interface`; that is:

```
  class Parent : public Reference::Able {
  public:
    class Interface;
  };

  class Parent::Interface : public TextInterface::To<Parent> {
  public:
    //! Default constructor
    Interface (Parent* instance = 0);
  };
```

Now suppose that the `Child` class follows the example set
by its parent and defines its text interface exactly as above, with a
nested sub-class named `Child::Interface`. In the
constructor for `Child::Interface`, the
`Parent::Interface` is imported as simply as:

```
Child:::Interface::Interface (Child* instance)
{
  import ( Parent::Interface() );

  if (instance)
    set_instance (instance);
}
```

Note that it is not mandatory to use nested sub-classes in order to
import the functionality of a parent text interface into the text
interfaces of its derived types. However, this convention proves
useful when the text interface requires access to protected methods,
and could also enable template codes that find the text interface
to a class in this way, e.g.:

```
template<class T>
void quick_set (T* instance, std::string& key, std::string& value)
{
  typename T::Interface interface (instance);
  interface.set_value (key, value);
}
```

**Importing a member interface**

For example, suppose that a class named `Composite` has an
attribute of type `Component`, and that the text interface
to `Component` is defined by a class named
`ComponentTI`. The `Composite` class must
define access to its `Component` attribute through a
method, e.g.

```
class Composite : public Reference::Able {
public:
  Component* get_member ();
};
```

In the constructor for the `Composite` text
interface, the `Component` text interface is
imported as in the following example:

```
CompositeTI::CompositeTI (Composite* instance)
{
  import ( "member", ComponentTI(), &Composite::get_member );

  if (instance)
    set_instance (instance);
}
```

It is necessary to name the component interface. The names of the
attributes in `ComponentTI` will be prefixed
by the name of the component interface followed by a colon. For
example, if `ComponentTI` defined three attribute
interfaces

```
name
max
code
```

then these would be accessed through `CompositeTI` via

```
member:name
member:max
member:code
```

**Importing the interface to the elements of a vector**

Now consider a class named `Vector` which contains an array
of attributes of type `Element`, and that the text
interface to `Element` is defined by a class named
`ElementTI`. The `Vector` class must define
methods that return its size and provide access to each
`Element`, e.g.

```
class Vector : public Reference::Able {
public:
  unsigned get_size () const;
  Element* get_element (unsigned);
};
```

In the constructor for the `Vector` text
interface, the `Element` text interface is
imported as in the following example:

```
VectorTI::VectorTI (Vector* instance)
{
  import ( "x", ElementTI(), &Vector::get_element, &Vector::get_size );

  if (instance)
    set_instance (instance);
}
```

It is necessary to name the element interface so that is may be
distinguished from other members of the vector class. The names of
the attributes in `ElementTI` will be prefixed by the name
of the element interface, followed by a range in square brackets,
followed by a colon. Following the `ComponentTI` example
from the preceding section, the attributes would be accessed through
`VectorTI` via names like

```
x[1]:name
x[2,5-6]:max
x[4-]:code
```

The last notation means "from the fourth element to the end of the array".

**Importing the interface to the data in a map**

Now consider a class named `Map` which contains data of
type `Data` indexed by a variable of type `Key`;
the text interface to `Data` is defined by a class named
`DataTI`. The `Map` class must define a method
that returns an element of type `Data` given an argument of
type `Key`, e.g.

```
class Map : public Reference::Able {
public:
  Data* get_data (const Key&);
};
```

In the constructor for the `Map` text
interface, the `Data` text interface is
imported as in the following example:

```
MapTI::MapTI (Map* instance)
{
  import ( "y", Key(), DataTI(), ⤅::get_data );

  if (instance)
    set_instance (instance);
}
```

The names of the attributes in `DataTI` will be
prefixed by the name of the data interface, followed by one or more
keys separated by commas and delimited by square brackets, followed by
a colon. Continuing the example, suppose that

```
std::istream& operator >> (std::istream& is, Key& key);
```

is defined, which successfully parses colour names: "red", "blue",
"green", etc. The attributes of each `Data`
element would be accessed through `MapTI` via
names like

```
y[red]:name
y[cyan,blue]:max
y[yellow]:code
```

If the name given to a map has zero length, then the square brackets
may be omitted.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
