# myprog

### 1.0 Purpose

This file represents the basic structure to which all PSRCHIVE application notes and manuals should
conform. Authors of an application should copy this file to name.shtml
where name is the name of their new program, then edit the text below
each header according to the instructions. Once checked into CVS, the
file will be automatically uploaded to the PSRCHIVE web site. When major changes are made to the
application source code, this file should be updated to reflect the
new state of the software.

Describe the purpose of the program here... The second line of this
file selects the StyleSheet called psrchive.css, which includes a few
extra html directives that should be used when describing code,
commands and terminal output. Use `this` when writing
inline C or C++ code in this document, or this style when
describing shell variables, filenames, paths, and extenstion. To
describe terminal input from the keyboard, use `this
style`. Sample output from the terminal should look `like
this`. Application names should also be highlighted like
`this`.

Please also see the [Composing Documentation](manuals/tutorial/documentation.md) tutorial.

### 2.0 Usage

Describe the usage of the program here, including command line arguments.
Show examples of configuration files or input data like this:

```
# myprog configuration file

# Values are assigned to keys by name
key = value
```

Give examples of execution like this:

```
> myprog -c config filenames

myprog: running ... done.
```

### 3.0 Algorithms

If relevant, describe the way in which the program works, or some of the
routines that it calls. Show code samples like this:

```
class MyClass {
public:
  void something ();
};
```

### 4.0 Testing and examples

Show a few examples of how the program should be run and what you might
expect to get back.

### 5.0 Known bugs and features that require implementation

* This is a list of known bugs.
