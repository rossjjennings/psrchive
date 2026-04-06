# PSRCHIVE Style Guide

## Style Guide

The following guidelines are common to most software development projects:

* use design patterns,* always protect attributes,* avoid multiple inheritance

---

## Design patterns

[Design patterns](http://en.wikipedia.org/wiki/Design_pattern_%28computer_science%29) are general repeatable solutions to problems
commonly encountered in software design. It is very infrequent that a
specific software design question has not already been answered, and
it saves a lot of time and effort to be familiar with this body of
work.

---

## Protect Attributes

No matter how simple your class, all of its **attributes (member
variables) should be protected or private** and access should be
mediated through separate set and get methods (member functions).
This is not merely a matter of style; the primary benefit of this
component of data abstraction is design flexibility:

* one may later decide that an attribute is either redundant or
  needs generalization; a good example is the
  `reference_frequency` attribute of the
  `Calibration::Faraday` class. When it was required to
  correct to infinite frequency, it became better to store the
  reference as a wavelength. The change to the internal storage of
  the attribute required no changes to any code that used the
  `get|set_reference_frequency` methods.* any additional error checking and/or other behaviours may be
    implemented in set and/or get methods; for example, a base class
    may ensure that preprocessing steps are performed on input data by
    making the input data attribute private and implementing these steps
    in the set input data method;* set and get methods enable *lazy evaluation*; e.g.

      ```
        //  the result attribute is cached internally and recomputed only when necessary 

        void MyClass::set_attribute (double new_value)
        {
          if (current_value == new_value)
            return;
          needs_computation = true;
          current_value = new_value;
        }

        double MyClass::get_result () const
        {
          if (needs_computation)
            compute_result ();
          return result;
        }
      ```

---

## Avoid Multiple inheritance

Multiple inheritance should be used judiciously;
an excellent alternative is the Policy or
[Strategy](http://en.wikipedia.org/wiki/Strategy_pattern)
behavioural design pattern.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
