
# Prepare a release

When ready to distribute the next release of
PSRCHIVE, it is important to first thoroughly test
the code on as many different machines and architectures as possible.

To do this, run

```sh
make distcheck
```

This make target is built
into automake; it will create a tarball of the source code
(including the configure script), then unpack it, build it
from scratch, run make check and make install, and
make distclean. This step is vital to ensure that all
required files are packed and ready for release.

After successful completion of `make distcheck` on a number
of different machines, [Release the distribution](release.md).
