# User's Guide: Baseline Removal

PSRCHIVE implements two high-level strategies for removing the off-pulse baseline from folded profiles

1. `RemoveBaseline::Total` - a common baseline mask is applied to all sub-integrations and frequency channels; and
2. `RemoveBaseline::Each` - a unique baseline mask is computed for each sub-integration and frequency channels.

The various algorithms that are available to compute the baseline mask
are described under [Configurable Algorithms and Options](../../psrstat/algorithms.md).

## `RemoveBaseline::Total`

Assuming that the recorded dispersion measure accurately describes the relationship between pulse phase and radio frequency
and that the phase model accurately predicts pulse phase in every sub-integration, this strategy includes the following steps

1. compute the total intensity integrated over all polarizations, frequency channels, and sub-integrations;
2. compute the common baseline mask; i.e. identify the phase bins to be included in the baseline using the current off-pulse estimation algorithm; and
3. for each polarization, frequency channel, and sub-integration, use the common baseline mask to compute and subtract the mean of the off-pulse phase bins

## `RemoveBaseline::Each`

This strategy includes the following steps

1. for each frequency channel and sub-integration, compute a unique baseline mask;
2. for each polarization, use the unique baseline mask to compute and subtract the mean of the off-pulse phase bins
