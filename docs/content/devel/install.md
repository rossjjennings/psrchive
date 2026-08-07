# Installation

The PSRCHIVE software makes use of the GNU
autotools to compile the source code and install the header files,
scripts, libraries and binary executables.
An introduction to GNU autoconf, automake, and libtool is available as
[The Goat Book](http://sources.redhat.com/autobook).
Developers with an interest in writing robust and portable code may
also take interest in the
[GNU coding standards](http://www.gnu.org/prep/standards).

## Building the Softwware

### Check-out

Developers can check out the software from GitLab with

```sh
git clone git@gitlab.com:pulsar-astronomy/psrchive.git
```

Upon successful checkout, a single directory, `psrchive`,
will be created in the current working directory.

### Bootstrap

Before configuring and compiling, developers must create the
`configure` script and Makefile.in input files by
running the GNU build tools. This procedure is performed by the
`bootstrap` script, which must be run in the
psrchive directory:

```sh
cd psrchive
./bootstrap
```

If you encounter problems during bootstrap, please ensure
that your [GNU autotools](../install/third/autotools.md) are up-to-date.

### Configure

After running bootstrap, the `configure` script is run to
determine the system configuration and produce the Makefiles.
If you plan to develop PSRCHIVE in parallel with
the software maintained at Swinburne or the ATNF, then please read these
important notes on [choosing an installation
directory](../install/third/install.md).
If the configure script fails, please refer to
[Installation Problems](../trouble/installation).

### Compile

In the top-level build directory (psrchive in most simple
cases), type `make` to compile and `make install` to
copy all products to the **install** directory. A complete list of
the standard make targets can be found in the [Standard Targets for Users](http://www.gnu.org/prep/standards/standards.html#Standard-Targets) section of the GNU coding standards.

In addition to the standard make targets, the following targets are defined:

* `libs`: compile and install only the libraries
* `include`:  install only the header files

