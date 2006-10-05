
SUBDIRS	= Util Core More

bin_SCRIPTS = psrchive_cflags psrchive_ldflags

psrchive_ldflags: Makefile
	@echo "#!/bin/csh" > psrchive_ldflags
	@echo "echo -L${prefix}/lib -lpsrmore -lpsrcore -lpsrutil \
		${LDFLAGS} @FFT_LIBS@ @SLA_LIBS@ @CFITSIO_LIBS@ @MPI_LIBS@ \
		@PGPLOT_LIBS@ @QT_LIBS@" > psrchive_ldflags
	@chmod a+x psrchive_ldflags

psrchive_cflags: Makefile
	@echo "#!/bin/csh" > psrchive_cflags
	@echo "echo -I${prefix}/include ${CPPFLAGS}" > psrchive_cflags
	@chmod a+x psrchive_cflags

libs: install-binSCRIPTS
	$(MAKE) RECURSIVE_TARGETS="all-recursive libs-recursive" libs-recursive

libs-am:
	@echo
	@echo PSRCHIVE libraries have been installed in ${prefix}/lib
	@echo

include: install-data

