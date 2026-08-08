# Configure the environment

As an example, add the following lines to `$HOME/.profile` or `$HOME/.bashrc`
to install PSRCHIVE in `$HOME/Pulsar`, assuming that its dependencies (PGPLOT, Tempo2, and psrcat) are also installed there.

```sh
export PSRHOME=$HOME/Pulsar
export PATH=${PATH}:$PSRHOME/bin
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:$PSRHOME/lib/pkgconfig
export PGPLOT_DIR=$PSRHOME/pgplot
export PGPLOT_FONT=$PGPLOT_DIR/grfont.dat

export TEMPO2=$PSRHOME/tempo2
export PSRCAT_FILE=$PSRHOME/psrcat/psrcat.db
```
