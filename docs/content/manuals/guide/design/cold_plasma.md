# User's Guide: Cold Plasma Effects

When supported by the underlying file format,
PSRCHIVE maintains and can differentiate
between cold plasma effects (dispersion and birefringence) that arise
in the interstellar medium from those that arise in the Earth's
ionosphere or the solar system. Anything that is not interstellar
is called *auxiliary*; however, the auxiliary dispersion measures and
rotation measures can also be used to represent changes in
interstellar DM / RM.

* Interstellar cold plasma effects are corrected with respect to the centre frequency stored in the archive header.
* Auxiliary cold plasma effects are corrected with respect to infinite frequency.

## Interstellar DM and RM parameters

The interstellar dispersion measure (DM) and Faraday rotation measure (RM)
and their related correction flags are stored in the primary header of a file
and can be queried and set using `psredit`:

* `rmc` [boolean] Interstellar Faraday rotation has
  been corrected; applies to all sub-integrations
* `dmc` [boolean] Interstellar dispersion has
  been corrected; applies to all sub-integrations
* `rm` [double-precision floating-point value]
  The interstellar Faraday rotation measure; applies to all sub-integrations
* `dm` [double-precision floating-point value]
  The interstellar dispersion measure; applies to all sub-integrations

## Auxiliary DM and RM parameters

The following six parameters are used to store information about
auxiliary dispersion and birefringence and can be queried and set
using `psredit`:

* `aux:rmc` [boolean] auxiliary Faraday rotation has
  been corrected; applies to all sub-integrations
* `aux:rm_model` [string] describes the model used to
  predict auxiliary rotation measure
* `aux:dmc` [boolean] auxiliary dispersion has
  been corrected; applies to all sub-integrations
* `aux:dm_model` [string] describes the model used to
  predict auxiliary dispersion measure
* `int:aux:rm` [double-precision floating-point value]
  for each sub-integration, the auxiliary rotation measure predicted
  by the model for the corresponding epoch
* `int:aux:dm` [double-precision floating-point value]
  for each sub-integration, the auxiliary dispersion measure predicted
  by the model for the corresponding epoch

### Example Usage: Edit auxiliary RM using a `psrsh`script

For example set the rotation measure due to the ISM to 166 and the
rotation measure due to the ionosphere to -9.9 for the first
sub-integration and -8.7 for the second sub-integration, create and
run the following `psrsh` script:

```
#!/usr/bin/env psrsh

edit rm=166
edit int[0]:aux:rm=-9.9
edit int[1]:aux:rm=-8.7
```

### Example Usage: Add the required auxiliary RM extension using `psredit`

If a data file supports optional storage of auxiliary cold plasma parameters
but does not yet contain the relevant extensions, they can be added with a
command like the following

```
psredit -c int:ext=+aux -c int:aux:rm=1.0 filename.ar -e new
```

Here `-c int:ext=+aux` instructs `psredit` to add the relevant extensions
and `-c int:aux:rm=1.0` sets the auxiliary RM to 1.0 in every sub-integration.
The auxiliary RM must be set to a non-zero value in at least one sub-integration.
Instead of setting it to unity in every sub-integration, it is possible to set the
values independently for every sub-integration, either on the command line; e.g.

```
psredit -c int:ext=+aux -c 'int[0]:aux:rm=1.2' -c 'int[1]:aux:rm=3.4' filename.ar -e new
```

or by providing a text file containing all of the same strings; e.g.

```
psredit -c int:ext=+aux -C setrm.txt filename.ar -e new
```

where setrm.txt contains the following lines

```
int[0]:aux:rm=1.2
int[1]:aux:rm=3.4
```

## Expected Behaviour

### When either an fscrunch or tscrunch is performed:

1. If `rmc`/`dmc` is false (0), then the interstellar RM/DM that
   applies to all sub-integrations is corrected with reference to the weighted centre-frequency
   of the resulting frequency channel (fscrunch) or sub-integration (tscrunch). The `rmc`/`dmc`
   flags are not changed and remain false (0).
2. If `aux:rmc`/`aux:dmc` is false (0), then
   the auxiliary RM/DM of each sub-integration is corrected with respect to infinite frequency
   and the `aux:rmc`/`aux:dmc` flags are set to true (1).

### When defaraday/dedisperse is performed:

1. If `rmc`/`dmc` is false (0), then the interstellar RM/DM that
   applies to all sub-integrations is corrected with reference to the centre frequency stored
   in the archive. The `rmc`/`dmc` are set to true (1).
2. If `aux:rmc`/`aux:dmc` is false (0), then
   the auxiliary RM/DM of each sub-integration is corrected with respect to infinite frequency
   and the `aux:rmc`/`aux:dmc` flags are set to true (1).

For further information about weighted centre frequencies, the
meanings of the `rmc` and `dmc` flags, and the
behaviour of PSRCHIVE when integrating in time
and frequency, please read [Weighted Means](weighted_means.md).
