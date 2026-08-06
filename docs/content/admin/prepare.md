# PSRCHIVE Administration Guide

## Preparing a Release

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
of different machines, edit configure.ac and change the AC\_INIT line to
reflect the new release number; e.g.

```sh
AC_INIT([PSRCHIVE],[2025-08-21],[psrchive-developers@lists.sourceforge.net])
```

Then commit the change, tag it, and create the final version of the release; e.g.

```sh
git commit -m "release 2025-08-21" configure.ac
git push
git tag 2025-08-21
git push origin tag 2025-08-21
make dist
```

The `make dist` step will create the distribution file,
psrchive-2025-08-21.tar.gz.
[Release the distribution](release.md).
