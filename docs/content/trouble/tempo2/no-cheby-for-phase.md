# TEMPO2 no cheby for phase

PSRCHIVE throws

```
Tempo2::Predictor::iphase: no cheby for phase
```

when the tempo2 predictor attached to an observation does not
contain a Chebyshev polynomial segment that covers the requested
pulse phase.

## How the predictor works

`Tempo2::Predictor::iphase` is the inverse of `Tempo2::Predictor::phase`:
given a pulse phase, it returns the MJD at which that phase occurred.
It does this by iterating over the predictor's Chebyshev polynomial
segments and selecting the one whose valid phase range contains the
requested phase. If no segment's range contains the phase, the
inversion cannot proceed and the error above is raised.

## Common causes

1. **Predictor coverage too narrow in time.** The predictor file was
   generated for an MJD range that does not include the observation.
   For example, if the predictor spans MJD 53000-53010 and the
   observation is at MJD 53020, every segment's valid range lies
   before the observation's pulse phase, so no segment matches.
2. **Predictor coverage too narrow in phase.** The individual segment
   length was chosen so small that phases near a segment boundary
   fall between two adjacent segments rather than inside one.

## Diagnostics

Inspect the predictor file for its MJD coverage:

```
head predictor.dat
```

Confirm that the observation's start and end MJDs lie within the
span reported in the header. If not, regenerate the predictor using
`tempo2 -f ephem.par -pred "..."` with an explicit MJD range that
includes the observation plus a margin.

Also verify that the ephemeris used to generate the predictor is
the same ephemeris attached to the archive being folded.

## Related

- `tempo2 -pred` usage from the [tempo2 manual](https://bitbucket.org/psrsoft/tempo2/src/master/)
  covers how to set predictor coverage explicitly.
