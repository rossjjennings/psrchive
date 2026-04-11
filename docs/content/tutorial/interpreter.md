# interpreter

**Adding to the Interpreter**
A number of PSRCHIVE programs, most notably
[psrsh](../manuals/psrsh.md), make use of the
`Pulsar::Interpreter` class to process user commands.

This tutorial describes how to add new functionality to the
`Pulsar::Interpreter` class, such that all PSRCHIVE programs that make use of it can
accept new commands.

Begin by choosing a name for the new command, which cannot be equal to
the name of an existing command. There are four steps required to add
the new command:

1. inherit the nested sub-class `Pulsar::Interpreter::Extension`;
2. declare and implement the method that will process the command;
3. add the command in the constructor of the new class; and
4. import the new class

---

# In More Detail ...

To serve as an example, consider adding a new command named
`cal`, which performs polarimetric calibration. The new
functionality will be implemented by a new class

```
// 1. Inherit the Extension base class
class CalInterpreter : public Pulsar::Interpreter::Extension
{
  public:

    // 2. Declare the method that will perform calibration
    std::string cal (const std::string&);

    // 3. The constructor will add the cal command
    CalInterpreter ();
};

[...]

Pulsar::Interpreter interpreter;

// 4. Import the new interpreter extension
interpreter.import( new CalInterpreter );
```

**Declaration of the method**

Note that the `cal` method receives a single
`std::string` argument and also returns a
`std::string`. This interface enables each command to
receive an arbitrary string of text as its arguments, and to report
any errors in the return value.

**Implementation of the constructor**
The `CalInterpreter` constructor is where the new method is
added to the list of available commands. This is done using the
`add_command` method of the `CommandParser` base
class (which is inherited by `Pulsar::Interpreter::Extension`)
as in the following example:

```
  add_command 
    ( &CalInterpreter::cal, 'c',
      "cal", "calibrate the currently loaded archive",
      "usage: cal <method> \n"
      "  string method      name of the method used to calibrate \n");
```

In the above example, the arguments to `add_command` are
as follows:

* `&CalInterpreter::cal` - the pointer to the method
  of the `CalInterpreter` class to be called when
  the command is entered by the user
* `'c'` - the shortcut key for this command
* `"cal"` - the full command name
* `"calibrate the currently loaded archive"` - a
  short help string, seen when the user enters `help` in
  psrsh.
* *the rest* - a detailed help string, seen when the user
  enters the `help cal` command in psrsh.

---

### Implementing the method

The `Pulsar::Interpreter` class provides some utility
methods that greatly simplify the implementation of new methods. The
following definition of the `scale` method is taken from
More/General/Interpreter.C:

```
string Pulsar::Interpreter::scale (const string& args)
try { 
  float factor = setup<float> (args);
  
  Archive* archive = get();

  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();

  for (unsigned isub=0; isub < nsub; isub++)
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
	archive->get_Profile(isub, ipol, ichan)->scale(factor);

  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}
```

In the above example, the following lines of code make use of
`Pulsar::Interpreter` methods:

`float factor = setup<float> (args);`

The `Pulsar::Interpreter::setup<T>` template method
receives a `std::string` argument and converts it to the
type `T` specified in the template argument. It checks to
ensure that only one argument was given, and throws an
`Error` exception if more than one argument is supplied, or
if the argument fails to parse into the specified type.

`Archive* archive = get();`

The `Pulsar::Interpreter::get` method returns the pointer
to the current `Pulsar::Archive` (the top of the stack).
If no data have been loaded, an `Error` exception is
thrown.

`return response (Good);` and
`return response (Fail, error.get_message());`

The `Pulsar::Interpreter::response` method receives one or two
arguments: a status code (`Good`, `Warn`, or
`Fail`) and, optionally, a text message.

In the interactive shell mode of [psrsh](../manuals/psrsh.md), the
`Pulsar::Interpreter::response` method inserts some
standardized text, based on the status code, to the beginning of the
message printed to the terminal. In the script command processing
mode, a `Fail` status will cause the script to abort.

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
