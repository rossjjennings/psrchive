# Complete Installation Instructions

## Complete Installation Instructions

These instructions can be followed to install PSRCHIVE and its dependencies.
Super user (root) privileges are not required, and it is
assumed that the C shell is used.

There are also some notes on installing PSRCHIVE on [Ubuntu running on Windows 10](https://docs.google.com/document/d/1k61ya7rOvUKfWppfZib3Sed1zOvL6_2MeHp6j9LdWog/edit?usp=sharing).

#### Setup the environment

Add the following lines to $HOME/.cshrc

```
setenv PSRHOME $HOME/Pulsar
setenv PATH ${PATH}:$PSRHOME/bin

setenv PGPLOT_DIR $PSRHOME/pgplot
setenv PGPLOT_FONT $PGPLOT_DIR/grfont.dat

setenv TEMPO2 $PSRHOME/tempo2
setenv PSRCAT_FILE $PSRHOME/psrcat/psrcat.db
```

or, if you prefer `bash`, add the following lines to $HOME/.profile

```
export PSRHOME=$HOME/Pulsar
export PATH=${PATH}:$PSRHOME/bin

export PGPLOT_DIR=$PSRHOME/pgplot
export PGPLOT_FONT=$PGPLOT_DIR/grfont.dat

export TEMPO2=$PSRHOME/tempo2
export PSRCAT_FILE=$PSRHOME/psrcat/psrcat.db
```

Create the PSRHOME directory, change to it,
and load the new environment.

```
mkdir $HOME/Pulsar
cd $HOME/Pulsar
source $HOME/.cshrc
```

The environment variables will be automatically set on each
subsequent login.

#### Download and build PSRCHIVE

It is recommended to download the latest version from the Git repository.
Follow the instructions at [Anonymous Git Access](current.md).

Be sure to also follow the link to the
[Build and Installation](current/build.md) directions,
including the
[Automatic Package Installation](third/install.md) directions.

You are now ready to go!

#### Installation via conda

As an alternative to building from source, PSRCHIVE can
also be installed into a [conda](https://docs.conda.io/en/latest/index.md)
environment.
These instructions assume you already have conda installed. If not, refer to
the [miniconda
installation instructions](https://docs.conda.io/en/latest/miniconda.md).
First, add the `conda-forge` channel (this can also be done by editing $HOME/.condarc):

```
conda config --add channels conda-forge
```

Then either create a new environment ("pulsar" in this example) that includes psrchive:

```
conda create -n pulsar psrchive
conda activate pulsar
```

Or, add it to an existing environment:

```
conda activate my_env
conda install psrchive
```

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
