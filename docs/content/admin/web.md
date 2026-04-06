# PSRCHIVE Administration Guide

### Updating web pages

Check out the web/  module; e.g. from the top-level directory

```
git clone ssh://USERNAME@git.code.sf.net/p/psrchive/web web
```

To upload the web pages

```
cd web/
./upload.csh
```

To upload the doxygen-generated API documentation, from the top-level

```
cd Documentation/
make
make install
```

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
