# Installation Directory

The installation directory is the directory to which all build
products (header files, libraries, executables, and runtime data) will
be installed when you type `make install`. The installation
directory is set when you run the `configure` script, and is
stored in the Makefile variable, prefix. When you type
`make install`, build products are installed in

```
$prefix/include
$prefix/lib
$prefix/bin
$prefix/share
```

There are currently three ways in which the installation
directory can be specified.

### 1. Default installation

The default installation directory is $PSRHOME/$LOGIN\_ARCH.
If the PSRHOME environment variable is not set,
/usr/local will be used instead.

### 2. Custom installation

As with any package that is configured and built using the GNU
autotools, the install directory may be specified using the
`--prefix` argument to the `configure` script.

### 3. Joint development installation

#### CVSHOME installation

Most Swinburne and ATNF developers will prefer to install in
$CVSHOME/$LOGIN\_ARCH; to set this up to be the default, add

```
setenv CONFIG_SITE $CVSHOME/psrchive/config/cvshome
```

to your .cshrc file. This assumes that you performed the CVS checkout
in $CVSHOME. If you checked out in a different directory,
modify the above line accordingly.

#### LOCAL\_ROOT installation

In order to enable behaviour that more closely matches that of the
Standard Makefile scheme, add

```
setenv CONFIG_SITE config/local_root
```

to your .cshrc file. Note that this environment variable does not
specify the full path to the local\_root script, only a
relative path. Therefore, this solution does not depend upon any
environment variable other than LOGIN\_ARCH. However,
psrchive must be your current working directory when you
run the `configure` script.

#### LOCAL\_ROOT + CVSHOME installation

If you must build in a directory other than the source directory
(psrchive) as is the case if you must compile on [multiple architectures](multiple.md), then the previous
solution will not work for you. However, if you have checked out the
software into a specified directory, say $CVSHOME, then you
can use another form of the above solution by adding

```
setenv CONFIG_SITE $CVSHOME/psrchive/config/local_root
```

to your .cshrc file. This will find the local\_root script
whatever your current working directory and, unless the current
working directory ends with psrchive, prefix
will be set to pwd. If the current working directory ends
with psrchive, then psrchive will be replaced
with $LOGIN\_ARCH.

### Changing the installation directory

It is possible to install build products in a directory other than the
installation directory that was set when the `configure` script
was run. It is not necessary to `make distclean`, nor must
the `configure` script be run again. Simply run

```
make install prefix=desired_install_path
```

Note, however, that if you use this option, you will have to set the
PSRCHIVE environment variable to *desired\_install\_path*.
