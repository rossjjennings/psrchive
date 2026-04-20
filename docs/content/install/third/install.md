# Package Installation

PSRCHIVE includes a number of scripts that may be used to automatically install its dependencies. These scripts are created by the `configure` script, which must be run first. After installing all of the required packages and setting the relevant environment variables as instructed, the `configure` script must be run again. For example,

```
./configure
./packages/fftw.csh
./packages/cfitsio.csh
./packages/pgplot.csh
# set the PGPLOT_DIR and PGPLOT_FONT environment variables
./packages/tempo2.csh
# set the TEMPO2 environment variable
./packages/psrcat.csh
# set the PSRCAT_FILE environment variable
./configure
```
