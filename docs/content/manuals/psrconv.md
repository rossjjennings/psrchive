# psrconv

### 1.0 Purpose

This program is used to convert a data file from its current format to any
of the other formats supported by the PSRCHIVE scheme. At the present time,
supported file formats include Timer, Baseband, EPN, PSRFITS and ASCII.

### 2.0 Usage

`psrconv` is invoked on the command line with a list of files that
the user wishes to convert, along with the `-o` switch and a keyword
that identifies the output file format.
For a full list of options, use:

```
psrconv -h
```

The help text includes a list of supported file formats and their associated
keywords. Multiple filenames can be specified on the command line either
individually or using wildcard characters.

### 3.0 Algorithms

`psrconv` simply calls the load method associated with the original
file format to construct a Pulsar::Archive in memory, then unloads it using
the methods associated with the destination class. In most cases this will
produce acceptable results but the user should be aware that different file
formats can store different types (and amounts) of information and one format
may not translate well into another.

### 4.0 Testing and examples

To convert a Timer archive into a PSRFITS file:

`psrconv -o PSRFITS filename.ar`

The program creates a new file called `filename.rf`. The default
extension associated with the destination filename is used automatically.

### 5.0 Known bugs and features that require implementation

* The user should be able to specify a new file extension.

Copyright © 2006-2019 Aidan Hotan
Licensed under the Academic Free License version 2.1
