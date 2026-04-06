# pam

### 1.0 Purpose

The Pulsar Archive Manipulator, `pam`, is designed to do
the dirty work of messing around with archives on disk, processing
them, compressing them, tweaking them... As such it is one of the most
dangerous of the PSRCHIVE applications. Like `vap` and
`pav` it has a bewildering array of command line options,
a summary of which can be printed using the faithful "-h". Of particular
note are the options related to file output. `pam` has
a sort of in-built safety catch in that it does nothing to the data
files unless you explicitly tell it to do so. Processing options on their
own result in no change to the archive on disk, they are performed on
the file once it has been read into memory. In order to make the change
permanent, you have to write out this new copy, either in place of the
old file or with a different name or in a new location with the same or
different name, etc. The essential options involved are:

```
"-m"  Modify (or mangle, depending on your perspective) the original file
      where it stands... no going back!

"-e"  Write out modified copies, changing the file extension to preserve
      the originals.

"-u"  Write out new archives with (by default) the same name, but in
      the given location (can be combined with "-e").
```

### 2.0 Usage

`pam` is very simple to use, all commands are of the form

```
pam options filenames
```

where the options can be anything from a large list of available
parameters. For a full list of options, use:

```
pam -h
```

Multiple filenames can be specified on the command line either
individually or using wildcard characters. Don't forget to specify
what you want done with the data, using one of the three output
options listed above. If you fail to do so, you will be given a
friendly reminder that you have just wasted a whole bunch of CPU
cycles (but possibly spared your hair and / or sanity for a few more
days).

### 3.0 Algorithms

`pam` has a job lot of functionality, built around the
core of the PSRCHIVE base classes and their methods. It'll even let
you override header parameters in some cases, if you need to change
the recorded feed basis in an archive for example.

### 4.0 Testing and examples

Pretty straight forward really. The following command will reduce each
".ar" file to a single profile, saving a lot of disk space.

```
pam -m -FTp *.ar
```

Or, if you just want to make timing profiles and preserve the raw data,
you could use:

```
pam -e tt -FTp *.ar
```

then run `pat` on the resulting ".tt" files.

### 5.0 Known bugs and features that require implementation

* None at this time.

Copyright © 2006 Aidan Hotan
Licensed under the Academic Free License version 2.1
