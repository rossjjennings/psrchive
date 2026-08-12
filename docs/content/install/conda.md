
# Installation via conda

As an alternative to building from source, PSRCHIVE can
also be installed into a [conda](https://docs.conda.io/en/latest/index.md)
environment.
These instructions assume you already have conda installed. If not, refer to
the [miniconda installation instructions](https://www.anaconda.com/docs/getting-started/miniconda/main).
First, add the `conda-forge` channel (this can also be done by editing `$HOME/.condarc`):

```sh
conda config --add channels conda-forge
```

Then either create a new environment ("pulsar" in this example) that includes psrchive:

```sh
conda create -n pulsar psrchive
conda activate pulsar
```

Or, add it to an existing environment:

```sh
conda activate my_env
conda install psrchive
```
