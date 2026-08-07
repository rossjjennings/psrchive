
# Data and Model

Use the `-D data`  command line option to enable two reports that
print the input data (Stokes parameters and errors) and the model evaluated
for each datum, once for the initial guess (before solving the measurement equation)
and once for the best-fit model.

For each input source and frequency channel, two text files named
pcm\_reports/*<source\_name>*/results\_init\_ichan*<channel\_index>*.dat
and
pcm\_reports/*<source\_name>*/results\_ichan*<channel\_index>*.dat
will be output for the initial guess and best-fit, respectively, where
*<source\_name>* is the name of the source and
*<channel\_index>* is the frequency channel index
