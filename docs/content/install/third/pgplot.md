# PGPLOT Graphics Subroutine Library

PSRCHIVE uses the PGPLOT Graphics Subroutine
Library to produce publication quality plots in a variety of styles.
[PGPLOT](http://www.astro.caltech.edu/~tjp/pgplot/) is
available from T. J. Pearson.

### Automatic Installation

The PSRCHIVE configure  script
produces a package installation script that can be used to automatically
install PGPLOT; e.g.

```
./configure
packages/pgplot.csh
```

Upon completion, the installation script will provide instructions on
how to set the $PGPLOT\_DIR  and $PGPLOT\_FONT 
environment variables. After setting these variables, re-run the
`configure`  script to enable PGPLOT functionality.

---

### Manual Download/Installation

Download PGPLOT as described in
[step one](http://www.astro.caltech.edu/~tjp/pgplot/install.md).

Unpack the zipped tarfile somewhere temporary, make
an installation directory, and run `makemake` with the
appropriate arguments. For example, assuming the installation
directory is $HOME/pgplot  on a Linux system with the GNU
compiler collection:

```
cd /tmp
gunzip -c $HOME/pgplot5.2.tar.gz  | tar xvf -
mkdir $HOME/pgplot
cp pgplot/drivers.list $HOME/pgplot
```

Then edit the file $HOME/pgplot/drivers.list,
removing the exclamation marks from in front of the drivers
that you wish to enable.

We recommend at
least all of the PSDRIV (Postscript) and XWDRIV (X
Window) drivers and, optionally, the PNDRIV (Portable Network
Graphics) drivers. Note that, if you enable PNDRIV, you will
have to edit the `makemake` script that comes with PGPLOT.
Simply remove the following line:

```
pndriv.o : ./png.h ./pngconf.h ./zlib.h ./zconf.h
```

If you will want to use --enable-shared while running configure, you
need to apply a patch to the makemake script to enable shared pgplot
libraries. To do this, run the following commands:

```
cd /tmp/pgplot
patch < /usr/local/src/psrchive/packages/makemake.sharedcpg.patch
```

where we assumed the sources of psrchive are located in
/usr/local/src .

When finished editing, run `makemake` as follows:

```
cd $HOME/pgplot
/tmp/pgplot/makemake /tmp/pgplot linux g77_gcc_aout
```

If only `gfortran` is installed on your system, then
edit the makefile produced by `makemake`, change
FCOMPL appropriately and remove the -u flag from
FFLAGC.

To ensure that everything is made (mostly important on OS X):

```
make
make cpg
make pgxwin_server
make grfont.dat
```

Finally, you will have to add the following to your .cshrc file:

```
setenv PGPLOT_DIR $HOME/pgplot
setenv PGPLOT_FONT $PGPLOT_DIR/grfont.dat

if ( $?LD_LIBRARY_PATH ) then
  setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$PGPLOT_DIR
else
  setenv LD_LIBRARY_PATH $PGPLOT_DIR
endif
```
