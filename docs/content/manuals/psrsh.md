# psrsh

### 1.0 Purpose

`psrsh`  is a command language interpreter that provides
access to PSRCHIVE data processing
algorithms. It can be used either as an interactive shell or as a
shell script command processor. If available, `psrsh` will
use the GNU readline command-line editor, programmable word
completion, and history mechanism. The development and use of
`psrsh` scripts can significantly reduce file I/O: data can
be loaded once and processed in many different ways, including
polarimetric calibration and RFI mitigation.

### 2.0 Usage

#### 2.1 Interactive shell

When `psrsh` is started without any command-line options, it
presents a command prompt and waits for a command to be entered. A
list of available commands can be obtained by entering `?` or
`help`. More detailed help on a specific command is
available by typing

```
psrsh> help command
```

To exit the shell, type `quit`.#### 2.2 Shell script

To execute a sequence of commands on one or more archives without
any interaction, run

```
psrsh script archive1 [archive2 ...]
```

where script is a text file containing a list of commands
that are understood by the interpreter. Commands must be on separate
lines, and lines beginning with # are treated as comments.

It is also possible to make script executable by placing

```
#!/usr/bin/env psrsh
```

in the first line of the file and appropriately setting the permissions
of the file. The script may then be run as

```
script archive1 [archive2 ...]
```

By default, unless the script contains a call to the `unload` 
method, the results of the script will not be written to disk.
However, if any of the[standard output
options](guide/design/options.shtml.html#output) are used, then `psrsh`  will unload the results
as specified. This allows scripts to behave like most other PSRCHIVE programs.
#### 2.3 Job preprocessor

Some PSRCHIVE programs (e.g.[psrplot](psrplot.md) and [psradd](psradd.md)) use the `psrsh`
interpreter to perform preprocessing on input archives. These
programs generally accept preprocessing commands, or *jobs*,
using the `-j` and `-J` command-line options. The
`-j` option accepts a semicolon-separated list of commands and
the `-J` option accepts a `psrsh` script; for example:

```
psrplot -j job1[;job2;...] -J script ...
```

Both the `-j` and `-J` options can be used at the
same time, and both may be used more than once. If a job command
requires one or more arguments, then the command and its argument[s]
must be enclosed in single or double quotes; for example:

```
psradd -j 'cmd1 arg1 arg2,cmd2,cmd3 arg1'
```

Used in this way, the `psrsh` interpreter provides a
standardized interface with highly flexible and programmable pre- or
post-processing functionality. It is also possible for applications
to filter their input using the `test` command, which behaves
much like the assert function in C and can evaluate a wide variety of
expressions.

For a complete list of supported commands, type `psrsh -H`.

#### 2.4 Shortcut Keys

Certain commands can be entered using a single character shortcut key,
and shortcut keys may be combined to form a single string (so long as
that string does not equal another command name). For example, to
integrate an archive in frequency and time, reduce the number of phase
bins to 128, and rotate by one quarter turn of phase before plotting:

```
psrplot -j 'FTB 128,r .25' etc.
```

Shortcut keys are shown in square brackets when you run `psrsh -H`.#### 2.5 Archive Stack

The `psrsh` interpreter supports storage of multiple archives
on a stack, enabling backup of data before an operation is performed
(history). When an archive is loaded using the command `load
filename` it automatically replaces the top of the stack.
The `push` and `pop` commands work as follows:

* `push`  
  push a copy of the current archive on top of the stack.
* `pop`  
  pop the archive currently on top of the stack. If not named (see next
  section) the archive will be deleted.#### 2.5 Archive Names

It is also possible to assign a name to each archive both on and off
the stack, using the following commands:

* `set name`  
  assign a name to the current top of the stack. If the archive is
  removed from the stack, it will not be deleted.
* `get name`  
  replace the current top of the stack with the named archive.
* `remove name`  
  delete the named archive

Some commands also accept names as an optional argument:

* `load name filename`  
  loads the specified file and assigns it a name.
  The archive does not replace the top of the stack.
* `unload name filename`  
  unloads the named archive with the specified filename.
* `push name`  
  push the named archive onto the top of the stack
  (differs from `get name`, which replaces
  the top of the stack).

Some commands require a name as an argument, such as:

* `append name`  
  copies data from the named archive into the archive
  currently at the top of the stack.

### 3.0 Algorithms

The `psrsh` interpreter acts as an interface to various
algorithms in the PSRCHIVE library. It does
not implement any new data reduction algorithms, but makes it possible
to conveniently combine commands in ways that may not have been
previously possible.

### 4.0 Testing and examples

* [Interactive shell](psrsh/examples/shell.md)
* [Executable script](psrsh/examples/script.md)
* [Polarimetric calibration](psrsh/examples/calibration.md)

### 5.0 Known bugs and features that require implementation

* plotting?

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
