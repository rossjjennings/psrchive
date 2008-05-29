
default: Util MEAL psrchive

Util MEAL psrchive:
	doxygen $@.dox > doxygen.out 2> doxygen.err
	@grep '^Error' doxygen.err

install:
	cp html/*.tag `psrchive_ssi`
	./upload.csh

