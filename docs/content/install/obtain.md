
# Obtain the source code

You can either download a stable release as a single file or clone the latest development version of PSRCHIVE.

## Download a stable release

Released versions of PSRCHIVE are available for download
at [GitLab](https://gitlab.com/pulsar-astronomy/psrchive/-/releases).

## Clone the development version

To compile the development version of PSRCHIVE requires the [GNU autotools](third/autotools).

For anonymous access via HTTPS,
```
git clone --recursive https://gitlab.com/pulsar-astronomy/psrchive.git
```

If you have a GitLab account, clone with SSH.
```
git clone --recursive git@gitlab.com:pulsar-astronomy/psrchive.git
```

After cloning the repository, run the bootstrap script.
```
cd psrchive/
./bootstrap
```

```{toctree}
:hidden:
:maxdepth: 2

third/autotools.md
```