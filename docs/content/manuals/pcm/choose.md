# pcm

### Optimal selection of pulse phase bins for MEM

`pcm` can automatically choose the best on-pulse phase bins
to use as model constraints. These are chosen from the pulsar
archive specified with `-c filename`.

By default, `pcm` will select a mixture of pulse phase bins
with the highest total intensity and the highest polarized flux.
To specify a selection strategy, use `-B name`,
where name selects the value to be maximized:

* `int` - total intensity* `pol` - polarized flux* `inv` - invariant interval* `orth` - orthogonality

Orthogonality is maximized by choosing the phase bin with
maximum polarized flux, then each subsequent phase bin that maximizes
the magnitude of the vector cross product of p=(Q,U,V) with the
previously chosen phase bin.

A mixture of values can be maximized by specifying a list of names
separated by commas; e.g. to maximize the invariant interval and
orthogonality

```
pcm -c choose.ar -B inv,orth [...]
```

When more than one value is specified, each strategy will be used in
turn for each phase bin selected.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
