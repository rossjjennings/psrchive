
# Specifying a phase value, range, or set of ranges

Several algorithms are parameterized by input values and can report
output values in units of pulsar longitude, or phase.  These can be
specified in units of either

* `turn` [default] - fractional turns of the pulsar (periodic on 0 to 1)
* `deg` - degrees of longitude (periodic on 0 to 360)
* `rad` - radians of phase (periodic on 0 to pi)
* `ms` - time in milliseconds (periodic on 0 to the pulsar spin period)
* `bin` - number of phase bins (periodic on 0 to the number of phase bins in the pulse profile)

The unit can be specified using modifier notation, which follows the
variable name and is preceded by a % sign.  For example, to specify
two on-pulse phase ranges that span 20 to 30 degrees and 200 to 210
degrees of longitude, then query the number of phase bins in that
range and the start and end phase bins of the ranges:

```sh
psrstat -c on=set -c on:range=20:30,200:210%deg -c on:count -c on:range%bin file.ar
```

In a file with 2048 phase bins in each profile, the result of the
above command would be

```text
on:count=112 on:range=114:171,1593:1650%bin
```

