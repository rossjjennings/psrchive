
# Modeling variations of instrumental response across the beam

*Part of [PCM](../pcm.md)*

By default, `pcm` uses a single Jones matrices to model the linear
transformation of the electric field vector along the boresight of a steerable dish.
For antennas that are not fully steerable (like a dipole array), or for which
the response might vary with pointing due to geometry or gravitational deformation,
it may prove beneficial to model the variation of the instrumental
response as some two-dimensional function of sky coordinates. A two-dimensional model
of the beam can also be useful when interested in the off-axis polarization of a fully-steerable dish
(e.g. [Liao et al 2016](https://ui.adsabs.harvard.edu/abs/2016ApJ...833..289L/abstract)).

For a steerable antenna, a single pulsar can be observed at multiple offsets within the primary beam.
If the antenna is not fully steerable, modelling the variability of the instrumental response
requires observations of multiple pulsars that span a variety of declinations and hour angles.

To simultaneously model observations of more than one pulsar using `pcm`,
specify multiple templates from which to choose phase bins as constraints. For example,

```
pcm -c 0742-2822.DFT -c 2111+46.DFT -c 0450+55.DFT [...]
```

A generic model of the various possible kinds of beam variability requires a flexible model, and `pcm`
uses a YAML-formatted file to configure such a model. The YAML file is provided on the command line
using the (overloaded) `-P` option; for example

```
pcm -c 0742-2822.DFT -c 2111+46.DFT -c 0450+55.DFT -P beam.yaml [...]
```

## Example beam configuration

Consider a model in which both the differential gain (`gamma`) and the differential phase (`phi`)
of a single-axis transformation (`SingleAxis`) each vary as a two-dimensional polynomial
function (`Polynomial2D`) of hour angle (`ha`) and declination (`dec`), as configured
by the following YAML file

```
model:
  name: SingleAxis
  G:fit: 0
chain:
  param: gamma
  model:
    name: Polynomial2D
    ncoef: 4:3
    fit[0]: 0
  args: [ha, dec]
chain:
  param: phi
  model:
    name: Polynomial2D
    ncoef: 4:3
    fit[0]: 0
  args: [ha, dec]
```

Both polynomials have four coefficients (polynomial order = 3; i.e. cubic) in the hour-angle dimension and
three coefficients (order = 2; i.e. quadratic) in the declination dimension. They are configured to disable
fitting the constant term (the zeroth coefficient) to avoid covariance/degeneracy with the differential gain and phase
of the bore-sight differential gain and phase.
