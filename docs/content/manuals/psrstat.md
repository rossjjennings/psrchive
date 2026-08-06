# psrstat

## 1.0 Purpose

The pulsar data statistical query interface, **psrstat**,
provides access to various quantities that can be derived from the
data stored in a Pulsar Archive. The interface and output format of
this program is similar to `psredit`, and all of the
parameters available in `psredit` are also available in
**psrstat**. **psrstat** can also loop over
index ranges and evaluate mathematical expressions using the same
syntax understood by `psrplot`.

## 2.0 Usage

**psrstat** commands are of the form:

```
psrstat -c quantity[ies] [-l index=range] filename[s]
```

where `quantity[ies]` is a string of one or more quantities,
separated by commas. (Multiple quantities may also be specified by
using multiple `-c` options.)

Each quantity is either a single variable name or a mathematical
expression enclosed in braces; e.g. to query the center frequency,
off-pulse r.m.s., and the natural logarithm of the signal-to-noise
ratio

```
psrstat -c freq,off:rms,'{log($snr)}' filename[s]
```

Note that some quantities are derived from the pulse profile, and by
default the profile from the first sub-integration, frequency channel,
and polarization will used. It is also possible to specify either the
profile or the range of profiles from which such quantities are
derived using the `-l index=range` command line option, which
uses the same syntax as described in the
[psrplot loop documentation](psrplot/loop.md).

The syntax for variable substitution, mathematical expressions, and
numerical precision is described
[here](guide/design/evaluation.md).

## 3.0 Algorithms

Different algorithms are available for computing the signal-to-noise
ratio and for identifying the off-pulse and on-pulse regions of the
pulse profile. The algorithms can be selected, configured, executed,
and queried using syntax similar to that of other parameters, as
described [here](psrstat/algorithms.md).

## 4.0 Testing and examples

* None at this time.

## 5.0 Known bugs and features that require implementation

* None at this time.
