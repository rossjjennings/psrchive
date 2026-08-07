
# Pulse width estimation

(label-width-transitions)=
## transitions

Pulse width is estimated by searching for the phase bins in which the
pulsed flux density first rises above and last falls below a threshold
defined by a fraction (e.g. 10% or 50%) of the maximum pulsed flux
density in the pulse profile.

For a given transition (rising or falling), linear interpolation of
the flux densities in the two bins on either side of the transition is
used to determine the pulse phase of the transition with an accuracy
of less than one phase bin.

The estimated pulse width is equal to the difference
between the pulse phases of the rising and falling transitions.

To estimate the uncertainty of the width estimate, the search is
repeated two additional times with the threshold offset by &sigma;
and -&sigma;, where &sigma; is the standard deviation of the flux
density in the off-pulse phase bins.  The uncertainty is half of the
difference between the -&sigma; and &sigma; width estimates.

(label-width-mtm)=
## mtm

Computes the effective pulse width as defined by combining Equation 13 of
[van Straten (2006)](http://adsabs.harvard.edu/abs/2006ApJ...642.1004V)
with the approximation that arrival-time error
is roughly equal to the pulse width divided by the signal-to-noise
ratio.

