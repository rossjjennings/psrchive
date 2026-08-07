
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
manual.  For example, to use only tempo2 predictors:

1. create a configuration file and edit it;
2. set `Predictor::default = tempo2`; e.g.

```
###########################################################################
#
# Predictor::default - Default predictor type
#
# The default phase predictor model type: 'polyco' or 'tempo2'
#
# Predictor::default = tempo2
```

3. set `Predictor::policy = default`; e.g.

<pre id=var>
###########################################################################
#
# Predictor::policy - Policy for generating new predictors
#
# The type of predictor constructed when generating a new 
# pulse phase prediction model (in Archive::update_model). 
# There are currently two policies for choosing the type of 
# the new predictor: 
# 
#  'input'   - same as the currently installed predictor 
#  'default' - determined by Predictor::default
#
# Predictor::policy = default
</pre>

4. put the configuration file where PSRCHIVE looks for it (see [psrchive\_config](../../config.md)).

