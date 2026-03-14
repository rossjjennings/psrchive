# Configuration file for the Sphinx documentation builder.

project = "PSRCHIVE"
copyright = "2004-2026, Willem van Straten et al."
author = "Willem van Straten"

extensions = [
    "breathe",
    "myst_parser",
]

# -- Breathe (Doxygen XML) configuration ------------------------------------

breathe_projects = {
    "psrchive": "_doxygen/psrchive/xml",
    "Util": "_doxygen/Util/xml",
    "MEAL": "_doxygen/MEAL/xml",
    "epsic": "_doxygen/epsic/xml",
}
breathe_default_project = "psrchive"

# -- General configuration ---------------------------------------------------

templates_path = ["_templates"]
exclude_patterns = ["_build", "_doxygen", ".venv"]

# -- Options for HTML output -------------------------------------------------

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]

# -- MyST (Markdown) support -------------------------------------------------

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}
