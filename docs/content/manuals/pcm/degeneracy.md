
## Measurement Equation Modeling (MEM) Degeneracy Under Commutation

*Part of [PCM](../pcm.md)*

It is necessary to make two assumptions to constrain
the degeneracy under commutation described in Appendix B of
[van Straten (2004)](http://adsabs.harvard.edu/abs/2004ApJS..152..129V).

1. ASSUMPTION 1: To constrain the mixing between Stokes I and V, one of
   the following four assumptions can be made.
   * 1.A) The noise diode has zero circular polarization; this
     assumption is selected by specifying the `-v` command line
     option.
   * 1.B) While pointing at a standard candle, the baseline flux
     density has zero circular polarization; this assumption is the
     default if the calibrator database that is provided
     to `pcm` contains FluxCal-On observations.
   * 1.C) Given a set of flux calibration observations, some made
     while pointing at a standard candle (FluxCal-On) and some made while
     pointing at "nearby blank sky" (FluxCal-Off), the difference in the
     off-pulse baseline flux density (FluxCal-On baseline minus
     FluxCal-Off baseline) is a reasonable approximation to the flux
     density of the standard candle and is assumed to have zero circular
     polarization; this assumption is selected by specifying
     the `-Y` command line option.
   * 1.D) The mixing between Stokes I and V is zero; that is, the
     receptors have equal ellipticities. This assumption is selected by
     specifying the `-k` command line option.
2. ASSUMPTION 2: To constrain the mixing between Stokes Q and U
   (rotation about line of sight), one of the following three assumptions
   can be made.
   * 2.A) the orientation offset of the first receptor is zero; or
   * 2.B) the orientation offset of the noide diode is zero.

Assumption 1.A is made by default if no
`Signal::FluxCalOn` observations
are provided, or if the -v command line option is specified.
In this case, Stokes V of the noise diode is fixed at zero.

Assumption 1.B is made by default if a
`Signal::FluxCalOn` observation is included as a
constraint. In this case, Stokes V of the noise diode is allowed to
vary as a free parameter.

Assumption 1.C is made only if both
`Signal::FluxCalOn` and `Signal::FluxCalOff`
observations are included as constraints and the -Y command
line option is specified. In this case, Stokes V of the noise diode
is allowed to vary as a free parameter.

Assumption 1.D is made only if the -k command
line option is specified. In this case, Stokes V of the noise diode
is allowed to vary as a free parameter.

Assumption 2.A is made by default. In this case, Stokes Q of the noise
diode is allowed to vary as a free parameter.

Assumption 2.B is made if the -q command line option is specified.

Except for Assumption 1.C, equally valid solutions are derived
regardless of the above assumptions; that is, except when Assumption
1.C is adopted, there is no change in the goodness of fit.
Assumption 1.C is different because new data (`Signal::FluxCalOff`
observations) are included as constraints.
