# psrplot

## 1.0 Purpose

The Pulsar Archive Plotter, **psrplot**, is a highly
configurable plotting program that may be used to create customized
plots of pulsar data. Using the PGPLOT graphics subroutine library,
**psrplot** produces publication quality plots with any of
the available device handlers. Therefore, it can be used both during
data analysis as a flexible visualization tool and during manuscript
preparation to create more refined Postscript figures.

The program utilizes the family of clases derived from
`Plot`. Each class in the `Plot` family tree is
given a name, and each plot is chosen by specifying its name (or its
single-letter code). Each plot also accepts a number of configuration
options, which may be specified by name either on the command line or
through the use of a style file.

## 2.0 Usage

**psrplot** is fairly simple to use. Most commands are of
the form:

```
psrplot -p plot [-c options] [-j jobs] [-l loop] filenames
```

where:

* `options` are plotting options (such as the label on
  the x-axis);
* `jobs` are preprocessing operations to be
  performed on the data before plotting (such as integration in
  frequency); and
* `loop` is an index over which to loop (such as plotting
  for each sub-integration).

Please see the [psrsh](psrsh.html#jobs)
documentation for more about `jobs`.

The available plotting options will depend upon the
chosen plot. For a list of available plots, type:

```
psrplot -P
```

For a list of options specific to *plot*, use:

```
psrplot -C plot
```

There is also a fairly large number of plot options that are common to
all plots; however, each plot may have different default values for
each option. To list these common options and see the default values
for *plot*, use:

```
psrplot -A plot
```

Multiple filenames can be specified on the command line either
individually or using wildcard characters. Each file will be displayed
in sequence, one after the other.

## 2.1 Details

Please follow the links for more detailed information on each topic:

* [Available plots](psrplot/plots.md)
* [Plot customization](psrplot/customization.md)
  + [Command line arguments](psrplot/customization.md#command_line)
  + [Style files](psrplot/customization.md#style_file)
  + [Option sets](psrplot/customization.md#option_sets)
* [Plot labels](psrplot/label.md)
  + [Label names](psrplot/label.md#name)
  + [Multiple lines](psrplot/label.md#multiple)
  + [Variable substitution](psrplot/label.md#variable)
* [Viewport layout](psrplot/viewport.md)
  + [Position and size](psrplot/viewport.md#position)
  + [Overlaying multiple plots](psrplot/viewport.md#overlay)
  + [Multiple plot style files](psrplot/viewport.md#style)
* [Pen attributes](psrplot/pen.md)
  + [Character font and height](psrplot/pen.md#font)
  + [Line width and style](psrplot/pen.md#line)
* [Looping over data dimensions](psrplot/loop.md)

## 3.0 Algorithms

The attribute syntax is defined by the `TextInterface::Class`,
which is described in the
[Building a text interface](../tutorial/interface.md) tutorial.

## 4.0 Testing and examples

Examples are given throughout the documentation in section 2.1.

## 5.0 Known bugs and features that require implementation

* None at this time.
