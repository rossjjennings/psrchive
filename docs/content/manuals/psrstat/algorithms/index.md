# Configurable Algorithms and Options

Some of the parameters listed below employ a special data type for
representing pulse phase, as explained [here](../phases).

## On-pulse and off-pulse identification

These algorithms identify regions of pulse phase and can be set 
using `psrstat -c on=NAME:OPTIONS` and `psrstat -c off=NAME:OPTIONS`.

---

`above`: Select all bins above a threshold times the standard 
deviation of the off-pulse noise, where the off-pulse region is 
identified using the currently configured off-pulse estimator.

  * `threshold` sigma [default: 3]

---

`consecutive`: Select all bins for which N contiguous bins are above a threshold 
times the standard deviation of the off-pulse noise, where the off-pulse region is 
identified using the currently configured off-pulse estimator.

  * `threshold` sigma [default: 3]
  * `consecutive` points above threshold [default: 3]

---

`cumulative`: Select the on-pulse region using the cumulative profile. The on-pulse 
region is defined as the region between the last bin to fall below `threshold` times
the total flux and the first bin to rise above `1.0-threshold` times the total flux. 
The off-pulse baseline is computed using the currently configured off-pulse estimator.
For more detail, see [cumulative](./onoff.md:cumulative).

  * `threshold` fraction of total integrated flux [default: 10%]

---

`set`: A static selection of regions of pulse phase.

  * `range` selected [phase range(s)](../phases) [default: NONE]

---

`normal`: Iteratively select bins that are less than threshold times the standard 
deviation away from the mean, as identified by an off-pulse estimator, assuming 
normally distributed data.

  * `threshold` sigma [default: 1]

---

`exponential`: Iteratively select bins that are less than threshold times the 
  mean (which is equal to the standard deviation) assuming exponentially distributed data.

  * `threshold` sigma [default: 1]

---

`iqr`: Iteratively select bins below thresholds computed using the inter-quartile range.

  * `threshold` for whiskers [default: 1.5]

---

`minimum`: Select continuous region with minimum mean.
  For more detail, see [`minimum`](onoff:minimum).

  * `smooth` the smoothing algorithm [default: boxcar mean with 15% duty cycle]
  * `find_min` boolean = find the minimum [default] or maximum
  * `search` the [phase range](../phases) to search [default: unspecified]
  * `found` the [phase range](../phases) found [read-only]

## Signal-to-noise ratio estimation

These can be set using `psrstat -c snr=NAME:OPTIONS`.

---

`phase`: Default algorithm uses cumulative to find the on-pulse region and minimum to find the baseline.
  For more detail, see [`phase`](snr:phase).

---

`standard`: Uses template matching to identify the amplitude of the pulse and the post-fit noise in the residual.

  * `file` filename of standard

---

`adaptive`: Uses cumulative to find the on-pulse and allows the off-pulse estimator to be configured.

  * `baseline` algorithm used to find the off-pulse baseline [default: minimum]

---

`modular`: Allows both the on-pulse and the off-pulse estimators to be configured.

  * `off` algorithm used to find the off-pulse baseline [default: normal]
  * `on` algorithm used to find the on-pulse region [default: consecutive]

---

`square`: Assumes that the signal is a square wave with a 50% duty cycle.

  * `threshold` multiple of off-pulse standard deviation required to detect on-pulse [default: 6]
  * `risetime` width of phase region ignored at transitions between off-pulse and on-pulse regions [default: 3%]

---

`pdmp`: Smooths profile with box-car mean of varying width and returns the maximum S/N.
  For more detail, see [`pdmp`](snr:pdmp).

## Pulse width estimation

These can be set using `psrstat -c width=NAME:OPTIONS`.

---

`transitions`: Searches for phases at which pulsed flux rises above and falls below a threshold.
  For more detail, see [`transitions`](width:transitions).

  * `bdc` baseline duty cycle (fraction of pulse period) of the boxcar used to smooth the profile when searching for the off-pulse baseline [default: 0.15]
  * `fmax` threshold (fraction of maximum pulsed flux) at which rising and falling edges will be sought [default: 0.5]

## Smoothing Algorithms and Options

`mean`: Computes the local mean of the boxcar.

  * `width` [phase range](../phases) (duty cycle) of the boxcar [default: 15%]

---

`median`: Computes the local median of the boxcar.

  * `width` [phase range](../phases) (duty cycle) of the boxcar [default: 15%]

---

`sinc`:

  * `width` [phase range](../phases) (effective width) of the sinc function; inversely proportional to the width of the lowpass filter in the frequency domain [default: 15%]

```{toctree}
:hidden:

onoff
snr
width
```