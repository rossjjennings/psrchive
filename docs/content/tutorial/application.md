# Writing an Application

One of the main hurdles when writing a new application is setting up
the skeleton, including command line parsing, file loading,
preprocessing options, etc. This task can be made simpler using the
`Pulsar::Application` base class. Furthermore, features
can be added in a modular fashion using the
`Pulsar::Application::Options` plugins.

---

A simple example of the usage of the
`Pulsar::Application` class can be found in

```
More/Applications/example.C
```

The remainder of this tutorial will refer to this code.

---

Step 1: **inherit the Pulsar::Application base class**

```
class example : public Pulsar::Application
{
public:

  //! Default constructor
  example ();

  //! Process the given archive
  void process (Pulsar::Archive*);
};
```

This simple class defines only the `process` method for
operating on a single `Pulsar::Archive` instance. The
`Pulsar::Application` class does the rest, including

* parsing command line options
* presenting help information
* parsing archive filenames
* looping over all archives
* performing any preprocessing

The following command line options are implemented by default:

```
 -h               help page
 -q               quiet mode
 -v               verbose mode
 -V               very verbose mode
 -M metafile      metafile contains list of archive filenames
```

---

Step 2: **define the constructor (and add Pulsar::Application::Options)**

```
example::example ()
  : Application ("example", "example psrchive program")
{
  add( new Pulsar::StandardOptions );
  add( new Pulsar::UnloadOptions );

  // default value for scale
  scale = 1.0;
}
```

This example makes use of the `Pulsar::StandardOptions`
class, which adds the standard preprocessing command line options:

```
 -j commands     execute pulsar shell preprocessing commands
 -J script       execute pulsar shell preprocessing script
```

It also uses the the `Pulsar::UnloadOptions` class to add
the standard command line options related to unloading results:

```
  -m             modify (overwrite) the original file
  -e ext         write files with a new extension
  -O path        write files to a new directory
```

Other `Pulsar::Application::Options` currently include
`Pulsar::CommonOptions`, which adds commonly used command
line options:

```
 -T              integrate all sub-integrations
 -F              integrate all frequency channels
 -p              integrate both polarizations
```

and `Pulsar::PlotOptions`, which adds plot device related
command line options:

```
 -D device       plot to the specified device
 -N x,y          divide the plot surface into x by y panels
```

---

Step 3: **add command line options**

```
void example::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  // add an option that enables the user to set the scale with -s
  arg = menu.add (scale, 's', "scale");
  arg->set_help ("multiply all amplitudes by 'scale'");

  // add an option that enables the user to set the source name with -name
  arg = menu.add (scale, "name", "string");
  arg->set_help ("set the source name to 'string'");
}
```

See the [`CommandLine::Menu` Class Reference](/api/util/class/classCommandLine_1_1Menu.rst) for further details on the variety of
ways to add command line options. Other psrchive programs provide further examples; for
example, see the source code in More/Applications/psradd.C.

---

Step 4: **define the process method**

```
void example::process (Pulsar::Archive* archive)
{
  if (!name.empty())
    archive->set_source (name);

  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  
  for (unsigned isub=0; isub < nsub; isub++)    
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
	Pulsar::Profile* profile = archive->get_Profile (isub, ipol, ichan);
	profile->scale(scale);
      }
}
```

The `example::process` method defines the processing
routine that is to be performed on each data file, which is likely the
primary reason that you are reading this tutorial.

---

Step 5: **run the program**

```
int main (int argc, char** argv)
{
  example program;
  return program.main (argc, argv);
}
```

The `Application::main` method performs the loop over all
files specified on the command line; inside this loop, it executes any
pre-processing commands then calls the `example::process`
method, which performs the work that is unique to this application.
