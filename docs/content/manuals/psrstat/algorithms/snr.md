# Signal-to-noise ratio estimation

## phase

This algorithm computes the signal-to-noise ratio (S/N) by integrating the flux in the 
on-pulse region and dividing by the standard deviation of the <i>smoothed</i> off-pulse 
phase bins, where the standard deviation of the <i>smoothed</i> off-pulse phase bins is 
equal to that of the <i>unsmoothed</i> off-pulse phase bins multiplied 
by the square root of the number of on-pulse phase bins.

The on-pulse region is found using the [cumulative](onoff:cumulative) algorithm.

## pdmp

This algorithm computes the signal-to-noise ratio (S/N) by smoothing the pulse profile with boxcars of varying width, somewhat like matched filtering, and returning the maximum S/N.

Before smoothing with various boxcar widths, the off-pulse mean is
subtracted.  The off-pulse region is found using an algorithm that
is effectively like the [minimum](onoff:minimum) algorithm,
with a boxcar width hard-coded to 40%.  Also, the off-pulse standard deviation is computed using the
currently installed baseline estimator.

During each boxcar iteration, the profile is smoothed by integrating all of the flux within the boxcar window, sliding this window by one phase bin and searching for the maximum in the pulsed flux.  The S/N for each iteration is defined as the integrated flux divided by the standard deviation of the <i>smoothed</i> off-pulse phase bins multiplied by the square root of one plus the duty cycle of the box car, where the standard deviation of the  <i>smoothed</i> off-pulse phase bins is equal to that of the <i>unsmoothed</i> off-pulse phase bins multiplied by the square root of the number of number of on-pulse phase bins (the number of phase bins in the boxcar window).

After each trial boxcar iteration, the boxcar width is increased by 50%.
