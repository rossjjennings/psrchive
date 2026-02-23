# Makefile for PSRCHIVE Sphinx + Breathe documentation
#
# Builds Doxygen XML (via override configs), then runs Sphinx.
# Usage:  make html

SPHINXBUILD   = sphinx-build
SPHINXOPTS    =
BUILDDIR      = _build
DOXYGENDIR    = _doxygen

# Doxygen override configs (build order matches Documentation/Makefile)
DOX_PROJECTS  = epsic Util MEAL psrchive

.PHONY: help clean doxygen html

help:
	@echo "  make html    – build Doxygen XML then Sphinx HTML"
	@echo "  make doxygen – build Doxygen XML only"
	@echo "  make clean   – remove all generated output"

doxygen:
	@for proj in $(DOX_PROJECTS); do \
		echo "==> Doxygen: $$proj"; \
		mkdir -p $(DOXYGENDIR)/$$proj; \
		doxygen $(DOXYGENDIR)/$$proj.dox.override \
			> $(DOXYGENDIR)/$$proj.out 2> $(DOXYGENDIR)/$$proj.err; \
		grep '^Error' $(DOXYGENDIR)/$$proj.err || true; \
	done

html: doxygen
	$(SPHINXBUILD) -b html $(SPHINXOPTS) . $(BUILDDIR)/html
	@echo "Build finished. Open $(BUILDDIR)/html/index.html"

clean:
	rm -rf $(BUILDDIR) $(DOXYGENDIR)/*/
