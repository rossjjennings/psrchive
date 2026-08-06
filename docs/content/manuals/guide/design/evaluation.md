# Evaluation of Mathematical Expressions

Both [psrstat](../../psrstat.md) and
[psrplot](../../psrstat.md) make use of the same
interpreter for the evaluation of mathematical expressions.
`psrstat` uses this interpreter to print useful information
to stdout, and `psrplot` can use it to label plots. This
page documents the features and syntax of the interpreter that are
common to both programs.

## Variable substitution

Any word preceded by a dollar sign (`$`) will be treated as a
variable name, and its value will be queried from the archive and
substituted. For example:

```
psrplot -c below:r='DM: $dm' [...]
```

would print a string like "DM: 2.64510488510132" just below the upper
right hand corner of the plot frame. (To set the number of significant
digits printed, see the notes on [numerical precision](Numerical precision) below.)

For a full list of available variable names,
run `psrstat <filename>`.

## Mathematical expressions

Any string of characters enclosed in braces (`{ }`) will be
evaluated as a mathematical expression and the result will be
substituted into the string. For example:

```
psrstat -c '{fabs($bw)/$freq}' [...]
```

would print the fractional bandwidth for each file specified on the
command line.

Any mathematical expression that can be parsed by the [Parsifal
evaluateExpression](http://www.parsifalsoft.com/examples/evalexpression/index.md) function may be used inside the braces.

## Vector functions

A number of functions that operate on arrays of values are also
available, including

* min - minimum value
* max - maximum value
* med - median value
* avg - mean value
* rms - standard deviation
* sum - sum of all values

The function name is specified before the opening brace bracket, and
the vector expression is included inside the braces. For example, to
print the mean value of the reduced chi-squared statistic stored in
the polarization calibrator solution extension, averaged over
frequency channels 16 through 121:

```
psrstat -c '{sum{$pcal:eqn[16-121]:chisq}/sum{$pcal:eqn[16-121]:nfree}}' [...]
```

Note that the outer braces are required to invoke evaluation of
the final division operation.

## Numerical precision

The numerical precision of floating point values may be set by
appending a precision specifier to either the variable name or
mathematical expression. This specifier is a percent sign followed by
an integer (`%N`) where *N* is the total number
of significant digits to be printed. For example:

```
psrplot -c below:r='DM: $dm%4' [...]
```

would add a string like "DM: 2.645" below the upper right hand corner of
each plot. Likewise:

```
psrstat -c 'Fractional BW: {fabs($bw)/$freq}%2' [...]
```

would print a string like "Fractional BW: 0.048" for each file.
