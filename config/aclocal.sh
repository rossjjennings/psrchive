#!/bin/sh

files="$* Makefile.aclocal psrchive.m4"

echo ""

echo 'aclocal_update: \'

for file in $files; do
  echo '	$(top_srcdir)/config/'$file' \'
done

echo

for file in $files; do
  echo '$(top_srcdir)/config/'$file' : $(PSRCHIVE_ACLOCAL)/'$file
  echo '	cp $(PSRCHIVE_ACLOCAL)/'$file' $(top_srcdir)/config/'$file
  echo
done

