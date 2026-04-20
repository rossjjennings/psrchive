PSRCHIVE
========

PSRCHIVE is an Open Source C++ development
library for the analysis of pulsar astronomical data. It implements
an extensive range of algorithms for use in pulsar timing,
scintillation studies, polarimetric calibration, single-pulse work,
RFI mitigation, etc. These tools are utilized by a suite of
applications that come with the library. The software is
described in [Hotan, van Straten & Manchester (2004)](http://www.publish.csiro.au/?paper=AS04022).

[Pulsar data analysis with PSRCHIVE](http://arxiv.org/abs/1205.6276)
(van Straten, Demorest & Osłowski 2012) provides
an overview of functionality, including detailed usage examples and
directions to sample data.

### Portability

PSRCHIVE was designed to increase the
portability of both algorithms and data. The software is installed
and compiled using the standard GNU configure and make system. It is
also able to read astronomical data in a number of different file
formats, including:

* [PSRFITS](https://www.atnf.csiro.au/research/pulsar/psrfits_definition/Psrfits.md), a standard data storage format developed at the
  Australia Telescope National Facility;
* [EPN](https://arxiv.org/pdf/astro-ph/9801097), the file format of the European Pulsar Network; and
* Timer, used primarily at the Parkes Observatory
Pulsar Data Archival and Analysis.

.. toctree::
   :maxdepth: 2
   :caption: Overview

   content/install/index
   quickstart
   content/manuals/guide
   contributing
   citation
   content/credits

.. toctree::
   :maxdepth: 2
   :caption: User Guide

   content/manuals
   content/manuals/python

.. toctree::
   :maxdepth: 2
   :caption: Developer Guide

   architecture
   api/index
   content/devel/development
   content/devel/install
   content/devel/design
   content/devel/style
   content/devel/git_guide
   content/devel/policy
   content/tutorial

.. toctree::
   :maxdepth: 2
   :caption: Troubleshooting

   content/trouble
   content/qa/index
   content/warnings/tempo/index
   content/warnings/tempo2/index




