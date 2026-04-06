# User's Guide: Standard Command Line Options

For the sake of consistency, a number of PSRCHIVE programs employ a standard set of command
line options, including

* [Standard preamble](options.shtml.html#preamble),* [Standard preprocessing](options.shtml.html#preprocessing)* [Standard output](options.shtml.html#output).

### Standard preamble

```
  -h,--help      print this help page
  -i,--version   version information
  -q             quiet mode
  -v             verbose mode
  -V             very verbose mode
  -M metafile    metafile contains list of archive filenames
```

### Standard preprocessing

Some programs make use of the [psrsh](../../psrsh.md)
command language interpreter to preprocess data. The relevant
command line options are

```
  -j commands    execute pulsar shell preprocessing commands
  -J script      execute pulsar shell preprocessing script
```

Please see the [psrsh](../../psrsh.html#jobs) manual for more
details.
### Standard output

Some programs that output a modified version of the data
(e.g. psredit, psrsh, and paz) accept the following
standard output command line options:

```
  -m             modify (overwrite) the original file
  -e ext         write files with a new extension
  -O path        write files to a new directory
```

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
