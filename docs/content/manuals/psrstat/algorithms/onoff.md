# On-pulse and off-pulse identification

## cumulative

The on-pulse region is estimated using the cumulative profile, and is
defined as the region between the last bin to fall below a threshold
fraction [default: 10%] of the total flux and the first bin to rise
above 1.0-threshold times the total flux.

The cumulative profile is computed by replacing the value in each
phase bin with the integrated sum of all preceding phase bins;
stated differently, the cumulative profile is to the pulse profile as
the cumulative probability is to the probability density (i.e. the
pulse profile is treated as a histogram).  This raises some important
points for consideration:

1. The integrated sum of all preceding phase bins depends on the
rotational phase of the pulse profile.  Stated differently, how is the
cumulative distribution defined on a circle (i.e. when the probability
density is cyclic)?

2. The variance in each phase bin of the cumulative sum increases
linearly; therefore, the noise statistics that should be applied when
comparing integrated flux to thresholds should also vary with phase
and will depend on the rotational phase of the pulse profile.  

In an incomplete attempt to work around the first issue, caused by the cyclic
property of pulse profiles, the PeakCumulative class computes the
cumulative profile twice; it shifts the profile by 180 degrees between
trials and takes the solution that results in the narrowest pulse.

The second issue (linear increase in variance with pulse phase) is
ignored in the current implementation.

## minimum

This algorithm can be used to find either the on-pulse or off-pulse
region; by default, it is configured to search for the off-pulse
(minimum).  It first smooths the pulse profile using a box-car
function (e.g. mean or median) and then sets the off- or on-pulse
region to span the width of the box-car, centred on where the smoothed
profile reaches its minimum [default] or maximum value.

Because it searches for an extremum in noisy data, this algorithm will
always under-estimate the true off-pulse mean or over-estimate the
true on-pulse mean.

