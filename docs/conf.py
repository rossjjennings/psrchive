# Configuration file for the Sphinx documentation builder.

project = "PSRCHIVE"
copyright = "2006-2026, Willem van Straten"
author = "Willem van Straten"

extensions = [
    "breathe",
    "myst_parser",
    "sphinx_click",
    "sphinx_copybutton",
    "sphinx.ext.autodoc",
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.mathjax",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
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

# -- MyST (Markdown) support -------------------------------------------------

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}

myst_enable_extensions = [
    "attrs_inline",
    "colon_fence",
    "amsmath",
    "dollarmath",
]

# -- Copy button configuration -----------------------------------------------

copybutton_prompt_text = "$ "
