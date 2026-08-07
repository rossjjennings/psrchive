# TEMPO no polynomials loaded

PSRCHIVE throws

```
polyco::load(FILE*): no polynomials loaded
```

when `Tempo::polyco::load` was asked to read a polyco file and the
file contained zero valid polynomial records.

## Common causes

1. **tempo produced no output.** The polyco file is empty or
   truncated because `tempo` failed to generate any polynomials.
   Check the `tempo` log (and its `-f` ephemeris) for errors.
2. **Wrong file.** The program was given a file that is not a
   polyco file at all — for example, a plain ephemeris `.par`
   file, a polyco stub header with no coefficient block, or a
   tempo2 predictor.
3. **File not yet written.** The polyco file was opened before
   generation finished, or the generation process was interrupted.

## Diagnostics

Verify that the polyco file has content:

```
wc -l polyco.dat
head polyco.dat
```

A valid polyco file contains one two-line header per polynomial,
followed by `NCOEF` lines of coefficients. A file shorter than
about 15 lines is almost certainly incomplete. If `head` shows
no records at all, rerun tempo with the appropriate ephemeris and
inspect its stderr for the underlying failure.

## Related

- [TEMPO warnings](../trouble/nopolynomials/index.md) describes how
  to tune `TZNSPAN` and `TZNCOEF` when the polynomial accuracy is
  insufficient but polynomials are generated.
