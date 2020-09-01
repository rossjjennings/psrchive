
default: epsic Util MEAL psrchive

epsic Util MEAL psrchive:
	@mkdir -p html/$@
	@cp commands.sty html/$@/commands.sty
	doxygen $@.dox > $@.dox.out 2> $@.dox.err
	@grep '^Error' $@.dox.err || echo 2> /dev/null

install:
	cp html/*.tag `psrchive --html`
	./upload.csh

