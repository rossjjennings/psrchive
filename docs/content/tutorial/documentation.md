# PSRCHIVE: Composing Documentation

**Composing Documentation**
The PSRCHIVE online documentation uses Apache
[Server
Side Includes (SSI)](http://httpd.apache.org/docs/2.2/howto/ssi.html#basic) to place the header and side navigation bars
on each page. Therefore, each file must be named with the
.shtml extension. To achieve the same style and layout as
the rest of the site, simply include the files open.shtml
and close.shtml, as in the following example.

```
<html><head><title>PSRCHIVE Pronunciation</title>
<!--#include virtual="/open.shtml" -->

<h2><font color=red>PSR</font>CHIVE Pronunciation Guide</h2>

<font size=6> p&#0275; &#0277s &auml;r&rsquo; k&#0299;v </font>

<!--#include virtual="/close.shtml" -->
```

*Note that* open.shtml *is included before
the* <head> *tag is closed.* This example
can be seen [here](../pronunciation.md). The only unique
content in the above example is the text enclosed in the title,
<title>, the heading, <h2>, and the
line of Unicode characters enclosed by <font>.

To change the name in the copyright notice,
include a line like the following:

```
<!--#set var="name" value="Dr. Astronomer" -->
```

before the call to include close.shtml.

To set the fonts and colours on each page of the PSRCHIVE site, cascading style files are used. For
example, it is often useful to set sections of code, or commands to be
entered in the shell, or the contents of a file, apart from the main
text of the documentation. This is commonly done using the
preformatted text (<pre>) environment, for which three
different styles have been defined.

For inline descriptions of C/C++ code, use
<code>
`MyClass`
</code>
and for blocks of code, use

```
<pre id=code>

class MyClass {
public:
  void something ();
};

</pre>
```

For inline descriptions of commands typed at the keyboard, including
command line arguments, use
<kbd>
`myprog`
</kbd>
and for blocks of commands, including sample output, use

```
<pre id=kbd>
> myprog -c option filenames

<samp>
sample program output
</samp>

</pre>
```

For inline descriptions of other things like filenames and their
contents use
<var>
filename.txt
</var>
and for blocks of file contents, use

```
<pre id=var>

# Example configuration file

# Values are assigned to keys by name
key = value

</pre>
```

Please also see the [Application Manual Template](../manuals/template.md).

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1
