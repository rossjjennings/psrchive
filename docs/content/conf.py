# Configuration file for the Sphinx documentation builder.

import os
import sys

with open("../../.build-commons/docs/conf.py") as f:
    exec(f.read())

project = "PSRCHIVE"
copyright = "2006-2026, Willem van Straten"
author = "Willem van Straten"

# -- Breathe (Doxygen XML) configuration ------------------------------------

breathe_projects = {
    "psrchive": "../_doxygen/psrchive/xml",
    "util": "../_doxygen/util/xml",
}
breathe_default_project = "psrchive"

# -- Intersphinx (cross-references to MEAL and EPSIC) -----------------------

intersphinx_mapping = {
    "meal": ("https://meal.readthedocs.io/en/latest/", None),
    "epsic": ("https://epsic.readthedocs.io/en/latest/", None),
}

html_static_path = ['_static']

