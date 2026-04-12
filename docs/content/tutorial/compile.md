# compile

**How to compile**
To compile the code from the [previous example](hello.md),
simply save it in a file named hello.C and run

```
g++ hello.C `psrchive --cflags --ldflags`
```

The shell script, `psrchive` is an idea borrowed from the
[GNU Scientific Library](http://www.gnu.org/software/gsl).
Depending on its arguments, it lists all of the compiler or linker
flags required to compile and link a PSRCHIVE
program. This script is installed with the rest of the software,
as described [here](../devel/install.md).

If you are a developer and wish to integrate your program with the
PSRCHIVE distribution, please follow the
instructions at [Making
Additions](../devel/development.shtml.html#additions).
