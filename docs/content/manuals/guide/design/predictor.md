
# Phase predictor configuration

Currently, PSRCHIVE supports the use of three
different pulse phase predictor models:

1. tempo polyco
2. tempo2 predictor
3. simple predictor (period, DM, acceleration)

By default, PSRCHIVE will use a phase prediction
model of the same type that was stored with the data on which it is
operating.  However, you may wish to replace the current predictor with
one of a different type, or establish the convention that only predictors
of a certain type will be used from this point forward, for ever and ever.

This can be configured using PSRCHIVE configuration
files, as described in the <a href="/manuals/config">psrchive_config</a> 
manual.  For example, to use only tempo2 predictors, add the following
lines to your configuration file:

```
Predictor::default = tempo2
Predictor::policy = default
```
