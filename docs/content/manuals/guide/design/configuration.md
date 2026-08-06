# User's Guide: Configuration

There may be more than one way to compute a certain value or perform a
certain operation. For example, there are a number of baseline
removal and on-pulse detection algorithms, and a nuber of different
ways to define/compute the signal-to-noise ratio.

PSRCHIVE implements various algorithms for such
tasks, and can programs can be configured to use these via a
simple configuration file.
See the [psrchive\_config](../../config.md) manual for more details.

If you like [Design Patterns](http://en.wikipedia.org/wiki/Design_Patterns), the selection of algorithms is an example of the
Strategy design. In PSRCHIVE, this pattern is
most often implemented using the `Functor` template class,
which allows the strategy to be implemented using a pointer to a

* function;
* functor instance; or
* instance of a class and a pointer to a member function

## Common Configuration Cravings

* [Always use tempo2 predictors instead of tempo polycos](manuals/guide/design/configuration/predictor.md)
