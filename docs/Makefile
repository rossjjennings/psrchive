# Makefile for PSRCHIVE Sphinx + Breathe documentation
#
# Builds Doxygen XML, runs breathe-apidoc to split each project into
# per-class pages, then runs Sphinx.
# Usage:  make html

SPHINXBUILD   = sphinx-build
SPHINXOPTS    = -j $(shell echo $$(($$(nproc) / 2)))
BUILDDIR      = _build
DOXYGENDIR    = _doxygen
APIDIR        = api

# Doxygen project name <-> api/ subdirectory
# (Doxygen project names are case-sensitive in conf.py; api/ uses lower-case)
DOX_PROJECTS    = Util psrchive

# Subset of DOX_PROJECTS that breathe-apidoc splits into per-class stubs.
# Util is omitted: it is ~430 internal helper classes that push the total
# stub count past RTD's 15-minute Community-tier build limit. api/util.md
# still renders Util via a single `doxygenindex` directive.
APIDOC_PROJECTS = psrchive

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
		doxygen $(DOXYGENDIR)/$$proj.dox.override \
			> $(DOXYGENDIR)/$$proj.out 2> $(DOXYGENDIR)/$$proj.err; \
		grep '^Error' $(DOXYGENDIR)/$$proj.err || true; \
	done

# Per-class API trees. Generated each build; excluded from version control
# via .gitignore. Output ends up at api/<lowercase project>/{class,struct,
# file,namespace}/*.rst plus *list.rst index files that the per-project
# .md page wires into a {toctree}.
apidoc: doxygen
	@for proj in $(APIDOC_PROJECTS); do \
		out=$(APIDIR)/$$(echo $$proj | tr '[:upper:]' '[:lower:]'); \
		xml=$(DOXYGENDIR)/$$proj/xml; \
		echo "==> breathe-apidoc: $$proj -> $$out"; \
		rm -rf $$out; \
		mkdir -p $$out; \
		breathe-apidoc -q -o $$out -p $$proj -m -f -g class $$xml; \
		: "Drop stub pages for compounds that have no XML in this project"; \
		: "(e.g. std::allocator that Doxygen mentions but does not document)"; \
		find $$out -mindepth 2 -name '*.rst' | while read f; do \
			b=$$(basename $$f .rst); \
			[ -f $$xml/$$b.xml ] || rm $$f; \
		done; \
	done

sphinx:
	$(SPHINXBUILD) -b html $(SPHINXOPTS) . $(BUILDDIR)/html

html: apidoc sphinx
	@echo "Build finished. Open $(BUILDDIR)/html/index.html"

clean:
	rm -rf $(BUILDDIR) $(DOXYGENDIR)/*/
	@for proj in $(APIDOC_PROJECTS); do \
		out=$(APIDIR)/$$(echo $$proj | tr '[:upper:]' '[:lower:]'); \
		rm -rf $$out; \
	done
