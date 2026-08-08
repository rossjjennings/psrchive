# How to Compile

To compile the code from the [previous example](hello.md),
simply save it in a file named hello.C and run

```sh
g++ hello.C `pkg-config psrchive --cflags --libs`
```

To use [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config),
the `PKG_CONFIG_PATH` environment variable includes the path
to the location where the file `psrchive.pc` is installed, as shown
in the example [environment configuration](../install/environment).

When the PSRCHIVE software is installed as described [here](../install/build),
the `psrchive.pc` package configuration is installed in `$prefix/lib/pkgconfig`.

If you are a developer and wish to integrate your program with the
PSRCHIVE distribution, please see the notes at [making additions](../devel/additions).
