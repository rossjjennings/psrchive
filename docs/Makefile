# Makefile for PSRCHIVE Sphinx + Breathe documentation
#
# Builds Doxygen XML, runs breathe-apidoc to split each project into
# per-class pages, then runs Sphinx.
# Usage:  make html

SPHINXBUILD   = sphinx-build
SPHINXOPTS    = -j 2
BUILDDIR      = _build
DOXYGENDIR    = _doxygen
APIDIR        = api

# Doxygen project name -> api/ subdirectory
DOX_PROJECTS  = util psrchive

.PHONY: help clean doxygen apidoc sphinx html

help:
	@echo "  make html    – build Doxygen XML, breathe-apidoc, then Sphinx HTML"
	@echo "  make doxygen – build Doxygen XML only"
	@echo "  make apidoc  – build Doxygen XML then per-class breathe-apidoc trees"
	@echo "  make clean   – remove all generated output"

doxygen:
	@for proj in $(DOX_PROJECTS); do \
		echo "==> Doxygen: $$proj"; \
		mkdir -p $(DOXYGENDIR)/$$proj; \
		doxygen $$proj.dox \
			> $(DOXYGENDIR)/$$proj.out 2> $(DOXYGENDIR)/$$proj.err; \
		grep '^Error' $(DOXYGENDIR)/$$proj.err || true; \
		./clean_xml.sh $(DOXYGENDIR)/$$proj/xml; \
	done

# Per-class API trees. Generated each build; excluded from version control
# via .gitignore. Output ends up at api/<lowercase project>/{class,struct,
# file,namespace}/*.rst plus *list.rst index files that the per-project
# .md page wires into a {toctree}.
apidoc: doxygen
	@for proj in $(DOX_PROJECTS); do \
		out=$(APIDIR)/$$proj; \
		xml=$(DOXYGENDIR)/$$proj/xml; \
		echo "==> breathe-apidoc: $$proj -> $$out"; \
		rm -rf $$out; \
		mkdir -p $$out; \
		breathe-apidoc -q -o $$out -p $$proj -m -f -g class $$xml; \
		python post_apidoc.py $$xml $$out; \
	done

sphinx:
	$(SPHINXBUILD) -b html $(SPHINXOPTS) . $(BUILDDIR)/html

html: apidoc sphinx
	@echo "Build finished. Open $(BUILDDIR)/html/index.html"

clean:
	rm -rf $(BUILDDIR) $(DOXYGENDIR) $(APIDIR)/*/class
