/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  For printing attributes (like centre frequency and bandwidth) along
  with statistics (like S/N and degree of polarization) from folded
  pulsar data.
*/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Statistics.h"
#include "Pulsar/Archive.h"

#include "substitute.h"
#include "evaluate.h"

#include "TextLoop.h"
#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>

using namespace std;
using namespace Pulsar;

//! Pulsar Statistics application
class psrstat: public Pulsar::Application
{
public:

  //! Default constructor
  psrstat ();

  //! Return usage information 
  std::string get_usage ();

  //! Return getopt options
  std::string get_options ();

  //! Parse a command line option
  bool parse (char code, const std::string& arg);

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  //! The expressions to be evaluated
  vector<string> expressions;

  //! Indeces over which to loop
  TextLoop loop;

  //! Print the name of each file
  bool output_filename;

  //! The interface to the current archive
  Reference::To<TextInterface::Parser> interface;

  //! Job to be performed on each leaf index
  void print ();

  void set_quiet () { output_filename = false; }
};

int main (int argc, char** argv)
{
  psrstat program;
  return program.main (argc, argv);
}

psrstat::psrstat ()
  : Pulsar::Application ("psrstat", "prints pulsar attributes and statistics")
{
  has_manual = true;
  version = "$Id: psrstat.C,v 1.4 2009/02/20 19:20:24 straten Exp $";

  // print/parse in degrees
  Angle::default_type = Angle::Degrees;

  // suppress warnings by default
  Archive::set_verbosity (0);

  // print the name of each file processed
  output_filename = true;

  loop.job.set( this, &psrstat::print );

  add( new Pulsar::StandardOptions );
}

std::string psrstat::get_options ()
{
  return "c:l:";
}

std::string psrstat::get_usage ()
{
  return
    " -c exp1[,exp2]  expressions to be evaluated and printed \n"
    " -l name=<range> loop over the range of the named parameter \n"
    "\n"
    " Multiple expressions and/or index ranges may be specified by using \n"
    " the -c and/or -l options multiple times. \n"
    "\n"
    " expn is either the name of a parameter (attribute or statistic) \n"
    " or a mathematical expression involving one or more parameters. \n"
    "\n"
    " Note that parameter names are case insensitive.\n"
    "\n"
    " For the complete list of all available parameters in an archive, \n"
    " run \"psrstat <filename>\" without any command line options. \n";

#if 0
    "\n"
    "INDn describes an index (e.g. subint, pol, chan) over which to loop. \n"
    "     For example: \n"
    "\n"
    "     -l chan=0-5     loop over frequency channels 0 to 5 (inclusive) \n"
    "     -l subint=2,5-  loop over sub-integrations 2 and 5 to last \n";
#endif
}

//! Parse a command line option
bool psrstat::parse (char code, const std::string& arg)
{
  switch (code)
  {
  case 'c':
    separate (arg, expressions, ",");
    break;

  case 'l':
    loop.add_index( new TextIndex(optarg) );
    break;

  default:
    return false;
  }

  return true;
}

void psrstat::process (Pulsar::Archive* archive)
{
  interface = standard_interface( archive );

  if (expressions.size() == 0)
  {
    cout << interface->help (true) << endl;;
    return;
  }

  if (output_filename)
    cout << archive->get_filename();

  loop.set_container (interface);
  loop.loop ();
}

void psrstat::print ()
{
  for (unsigned j = 0; j < expressions.size(); j++)
  {
    string text = expressions[j];

    if (verbose)
      cerr << "psrstat: processing '" << text << "'" << endl;

    if ( text.find('$') == string::npos )
      cout << interface->process ( text );
    else
      cout << evaluate( substitute( text, interface.get() ) ); 
  }
  
  cout << endl;
}

