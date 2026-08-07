# Command Line Options

To increase consistency, a number of PSRCHIVE programs employ a standard set of command
line options, including

```{contents}
:depth: 1
:local:
```

(label-options-preamble)=
## Standard preamble

```
  -h,--help      print this help page
  -i,--version   version information
  -q             quiet mode
  -v             verbose mode
  -V             very verbose mode
  -M metafile    metafile contains list of archive filenames
```

(label-options-preprocessing)=
## Standard preprocessing

Some programs make use of the [psrsh](../../psrsh.md)
command language interpreter to preprocess data. The relevant
command line options are

```
  -j commands    execute pulsar shell preprocessing commands
  -J script      execute pulsar shell preprocessing script
```

Please see the [psrsh](job-preprocessor) manual for more details.

(label-options-output)=
## Standard output

Some programs that output a modified version of the data
(e.g. psredit, psrsh, and paz) accept the following
standard output command line options:

```
  -m             modify (overwrite) the original file
  -e ext         write files with a new extension
  -O path        write files to a new directory
```
