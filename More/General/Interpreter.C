/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Interpreter.h"
#include "TextInterface.h"

#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveTI.h"

#include "Pulsar/ScatteredPowerCorrection.h"
#include "Pulsar/Transposer.h"

#include "string_utils.h"
#include "tostring.h"
#include "Error.h"

void Pulsar::Interpreter::init()
{
  clobber = true;
  inplace = false;
  stopwatch = false;
  reply = false;
  
  prompt = "psrsh> ";
  
  add_command
    (&Interpreter::get_report,
     "report", "display plugin information",
     "no arguments required\n");
  
  add_command 
    ( &Interpreter::load,
      "load", "load archive from file",
      "usage: load [name] <filename>\n"
      "  string filename   name of the file to be read \n"
      "  string name       if specified, assign name to archive\n" );
  
  add_command 
    ( &Interpreter::unload,
      "unload", "unload archive or set unload options",
      "usage: unload [name] <filename> \n"
      "  string filename   name of the file to be written \n"
      "  string name       if specified, unload the named archive \n" );
  
  add_command
    ( &Interpreter::push,
      "push", "push archive onto stack",
      "usage: push [name] \n"
      "  string name       name of archive to push \n"
      "                    if not specified, push clone of current archive\n");
  
  add_command
    ( &Interpreter::pop,
      "pop", "pop current archive off top of stack",
      "usage: pop \n" );
  
  add_command
    ( &Interpreter::set,
      "set", "set name of current archive",
      "usage: set <name> \n"
      "  string name       name to give to current archive \n");
  
  add_command
    ( &Interpreter::get,
      "get", "get named archive",
      "usage: get <name> \n"
      "  string name       name of archive to become current \n");
  
  add_command
    ( &Interpreter::remove,
      "remove", "remove named archive",
      "usage: remove <name> \n"
      "  string name       name of archive to be removed \n" );
  
  add_command
    ( &Interpreter::clone,
      "clone", "duplicate an archive",
      "usage: clone <name> \n"
      "  string name       name of archive to be cloned \n"
      "                    if not specified, clone current archive \n" );

  add_command
    ( &Interpreter::extract,
      "extract", "duplicate part of an archive",
      "usage: extract [name] <subints> \n"
      "  string name       name of archive from which to extract \n"
      "                    if not specified, current archive is used \n"
      "  unsigned subints  range[s] of subints to be extracted \n" );
  
  add_command
    ( &Interpreter::edit,
      "edit", "edit archive parameters",
      "usage: edit <command> ...\n"
      "  string command    any edit command as understood by psredit \n" );
  
  add_command
    ( &Interpreter::append,
      "append", "append data from one archive to another",
      "usage: append <name> \n"
      "  string name       name of archive to be appended \n" );
  
  add_command 
    ( &Interpreter::fscrunch, 'F',
      "fscrunch", "integrate archive in frequency",
      "usage: fscrunch [chans] \n"
      "  unsigned chans    number of desired frequency channels \n"
      "                    if not specified, fscrunch all (chans=1)\n" );
  
  add_command 
    ( &Interpreter::tscrunch, 'T',
      "tscrunch", "integrate archive in time",
      "usage: tscrunch [subints] \n"
      "  unsigned subints  number of desired sub-integrations \n"
      "                    if not specified, tscrunch all (subints=1)\n" );

  add_command 
    ( &Interpreter::pscrunch, 'p',
      "pscrunch", "integrate archive to produce total intensity",
      "usage: pscrunch \n" );
  
  add_command
    ( &Interpreter::bscrunch, 'B',
      "bscrunch", "integrate archive in phase bins",
      "usage: bscrunch bins \n"
      "  unsigned bins     number of desired phase bins\n" );

  add_command 
    ( &Interpreter::centre, 'C',
      "centre", "centre profiles using the polyco",
      "usage: centre \n" );

  add_command 
    ( &Interpreter::dedisperse, 'D',
      "dedisperse", "dedisperse all profiles in an archive",
      "usage: dedisperse \n" );

  add_command 
    ( &Interpreter::defaraday, 'R',
      "defaraday", "apply faraday rotation correction",
      "usage: defaraday \n" );

  add_command 
    ( &Interpreter::correct_instrument,
      "pac", "apply parallactic angle correction",
      "usage: pac \n");

  add_command 
    ( &Interpreter::scattered_power_correct,
      "spc", "apply scattered power correction",
      "usage: spc \n");

  add_command 
    ( &Interpreter::screen_dump,
      "screendump", "display raw data points",
      "usage: screendump \n");
  
  add_command 
    ( &Interpreter::toggle_clobber,
      "clobber", "toggle overwrite permission",
      "usage: clobber \n");

}

Pulsar::Interpreter::Interpreter()
{
  init ();
}

//! construct from command line arguments
Pulsar::Interpreter::Interpreter (int &argc, char** &argv)
{
  init ();
}

//! destructor
Pulsar::Interpreter::~Interpreter ()
{

}

bool Pulsar::Interpreter::fault () const
{
  return status == Fail;
}

string Pulsar::Interpreter::get_report (const string& args)
{
  Pulsar::Archive::agent_report ();
  return response (Good);
}

/*!
  Eventually, the Interpreter class might filter special arguments
  out of the list before passing the remainder along to the method.
*/
vector<string> Pulsar::Interpreter::setup (const string& text)
{
  status = Undefined;

  vector<string> arguments;
  Tokenise (text, arguments, " \t\n");

  return arguments;
}

/*!
  Eventually, the Interpreter class might filter special arguments
  out of the list before passing the remainder along to the method.
*/
string Pulsar::Interpreter::response (Status s, const string& text)
{
  status = s;

  if (!reply)
    return text;

  string out;

  switch (status) {
  case Good:
    out = "ok";      break;
  case Warn:
    out = "warning"; break;
  case Fail:
    out = "fail";    break;
  default:
    out = "?";       break;
  }

  if (!text.length())
    return out;

  return out + " " + text;
}

void Pulsar::Interpreter::set (Archive* data)
{
  if (theStack.empty() || !inplace)
    theStack.push (data);
  else
    theStack.top() = data;
}

Pulsar::Archive* Pulsar::Interpreter::get ()
{
  if (theStack.empty() || !theStack.top())
    throw Error (InvalidState, "Pulsar::Interpreter::get",
		 "no archive in stack");
  return theStack.top();
}

void Pulsar::Interpreter::setmap (const string& name, Archive* data)
{
  if (!clobber) {
    map< string, Reference::To<Archive> >::iterator entry = theMap.find (name);
    if (entry != theMap.end())
      throw Error (InvalidState, "Pulsar::Interpreter::set",
		   "archive already exists with name '" + name + "'");
  }
  theMap[name] = data;
}

Pulsar::Archive* Pulsar::Interpreter::getmap (const string& name, bool ex)
{
  map< string, Reference::To<Archive> >::iterator entry = theMap.find (name);

  if (entry == theMap.end()) {
    // not found
    if (!ex) 
      return 0;
    throw Error (InvalidState, "Pulsar::Interpreter::get",
		 "no archive named '" + name + "'");
  }
  return entry->second;
}


//! push a clone of the current stack top onto the stack
string Pulsar::Interpreter::push (const string& args)
try {
  vector<string> arguments = setup (args);

  if (arguments.size() > 1)
    return response (Fail, "push: please specify only one name");

  if (arguments.size())
    theStack.push ( getmap(arguments[0]) );
  else
    theStack.push( get()->clone() );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "push: " + error.get_message());
}


//! pop the top of the stack
string Pulsar::Interpreter::pop (const string& args)
{
  if (theStack.empty())
    return response (Warn, "pop: currently at bottom");

  theStack.pop();
  return response (Good);
}

string Pulsar::Interpreter::set (const string& args)
try {
  vector<string> arguments = setup (args);

  if (arguments.size() != 1)
    return response (Fail, "set: please specify one name");

  setmap( arguments[0], get() );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "set: " + error.get_message());
}

string Pulsar::Interpreter::get (const string& args)
try {
  vector<string> arguments = setup (args);

  if (arguments.size() != 1)
    return response (Fail, "get: please specify one name");

  set( getmap(arguments[0]) );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "get: " + error.get_message());
}


string Pulsar::Interpreter::remove (const string& args)
{
  vector<string> arguments = setup (args);

  if (arguments.size() != 1)
    return response (Fail, "remove: please specify one name");

  string name = arguments[0];
  map< string, Reference::To<Archive> >::iterator entry = theMap.find (name);

  if (entry == theMap.end())
    return response (Fail, "no archive named " + name);

  theMap.erase (entry);

  return response (Good);
}


string Pulsar::Interpreter::clone (const string& args)
try { 
  vector<string> arguments = setup (args);

  if (!arguments.size() > 1)
    return response (Fail, "clone: please specify only one name");

  if (arguments.size() == 1)
    set( getmap(arguments[0])->clone() );
  else
    set( get()->clone() );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "clone: " + error.get_message());
}

string Pulsar::Interpreter::extract (const string& args)
try {
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, help("extract"));

  /*
    if the first argument is a valid map name, then start with this archive;
    otherwise, start with the top of the stack 
  */

  Reference::To<Archive> archive = getmap (arguments[0], false);

  unsigned range = 0;

  if (archive)
    range ++;
  else
    archive = get();

  vector<unsigned> indeces;

  /*
    parse the (remaining) options as though they were indeces
  */
  for (unsigned i = range; i < arguments.size(); i++)
    TextInterface::parse_indeces (indeces, arguments[i]);

  /*
    extract the sub-integrations
  */
  set( archive -> extract(indeces) );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "extract: " + error.get_message());
}

string Pulsar::Interpreter::edit (const string& args)
try { 
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, "edit: please specify at least one editor command");

  if (!interface)
    interface = new ArchiveTI;

  interface->set_instance (get());

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++)
    retval += interface->process (arguments[icmd]);

  return retval;
}
catch (Error& error) {
  return response (Fail, "append: " + error.get_message());
}

string Pulsar::Interpreter::append (const string& args)
try { 
  vector<string> arguments = setup (args);

  if (!arguments.size() != 1)
    return response (Fail, "append: please specify one name");

  get()->append( getmap(arguments[0]) );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "append: " + error.get_message());
}

string Pulsar::Interpreter::load (const string& args)
try {
  vector<string> arguments = setup (args);
  
  if (!arguments.size() || arguments.size() > 2)
    return response (Fail, help("load"));

  if (arguments.size() == 2)
    setmap( arguments[0], Archive::load(arguments[1]) );
  else
    set( Archive::load(arguments[0]) );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "load: " + error.get_message());
}

string Pulsar::Interpreter::unload (const string& args)
try {
  vector<string> arguments = setup (args);
  
  if (arguments.size() || arguments.size() > 2)
    return response (Fail, help("unload"));

  if (arguments.size() == 2)
    getmap( arguments[0] )->unload( arguments[1] );
  else
    get()->unload( arguments[0] );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "unload: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// fscrunch <string> <int>
//
string Pulsar::Interpreter::fscrunch (const string& args)
try {
  unsigned scrunch_to = setup<unsigned> (args, 0);
  
  if (scrunch_to)
    get() -> fscrunch_to_nchan (scrunch_to);
  else
    get() -> fscrunch();
  
  return response (Good);

}
catch (Error& error) {
  return response (Fail, "fscrunch: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// tscrunch <string> <int>
//
string Pulsar::Interpreter::tscrunch (const string& args)
try {
  unsigned scrunch_to = setup<unsigned> (args, 0);
  
  if (scrunch_to)
    get() -> tscrunch_to_nsub (scrunch_to);
  else
    get() -> tscrunch();
  
  return response (Good);
}
catch (Error& error) {
  return response (Fail, "tscr: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// pscrunch <string> <int>
//
string Pulsar::Interpreter::pscrunch (const string& args)
try {
  if (args.length())
    return response (Fail, "pscr: accepts no arguments");

  get() -> pscrunch();

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "pscr: " + error.get_message());
}

string Pulsar::Interpreter::bscrunch (const string& args)
try {
  unsigned scrunch_to = setup<unsigned> (args);
  
  if (!scrunch_to)
    return response (Fail, "bscr: invalid number of bins");

  get() -> bscrunch_to_nbin (scrunch_to);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, "bscr: " + error.get_message());
}

string Pulsar::Interpreter::centre (const string& args)
try {
  if (args.length())
    return response (Fail, "centre: accepts no arguments");
  get()->centre();
  return response (Good);
}
catch (Error& error) {
  return response (Fail, "centre: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// dedisp <string>
//

string Pulsar::Interpreter::dedisperse (const string& args)
try {
  if (args.length())
    return response (Fail, "dedisp: accepts no arguments");

  if (get()->get_dedispersed())
    return response (Warn, "dedisp: already dedispersed");

  get()->dedisperse();

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "dedisp: " + error.get_message());
}

string Pulsar::Interpreter::correct_instrument (const string& args)
try {
  if (args.length())
    return response (Fail, "pac: accepts no arguments");

  if (get()->get_instrument_corrected())
    return response (Warn, "pac: already corrected");

  get()->correct_instrument();

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "pac: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// frc <string>
//

string Pulsar::Interpreter::defaraday (const string& args)
try {
  if (args.length())
    return response (Fail, "defaraday: accepts no arguments");

  if (get()->get_faraday_corrected())
    return response (Warn, "defaraday: already corrected");

  get()->defaraday();

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "defaraday: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// spc <string>
//

string Pulsar::Interpreter::scattered_power_correct (const string& args)
try {
  if (args.length())
    return response (Fail, "spc: accepts no arguments");

  Archive* arch = get();
  if (arch->get_state() == Signal::Stokes)
    arch->convert_state(Signal::Coherence);
  
  Pulsar::ScatteredPowerCorrection spc;
  for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
    spc.transform (arch->get_Integration(isub));

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "spc: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// clobber
//

string Pulsar::Interpreter::toggle_clobber (const string& args)
{
  clobber = !clobber;
  
  if (!reply)
    return "";

  if (clobber)
    return "will clobber named archives";
  else
    return "will not clobber named archives";
}

// //////////////////////////////////////////////////////////////////////
//
// screendump <string>
//
string Pulsar::Interpreter::screen_dump (const string& args)
try {
  if (args.length())
    set( getmap (args) );

  Reference::To<Pulsar::Archive> copy = get() -> clone();
  copy -> tscrunch();
  copy -> fscrunch();

  Transposer transposer (copy);
  transposer.set_dim (0, Signal::Polarization);
  transposer.set_dim (1, Signal::Phase);
  transposer.set_dim (2, Signal::Frequency);

  vector<float> data;

  transposer.get_amps (data);
	
  for (unsigned i = 0; i < (copy->get_nchan())*(copy->get_nbin()); i+=4) {
	  
    if (copy -> get_npol() == 4) {
      cerr << data[i] << "\t" << data[i+1] << "\t" 
	   << data[i+2]  << "\t" << data[i+3] << endl;
    }
    
    if (copy -> get_npol() == 2) {
      cerr << data[i] << "\t" << data[i+1] << endl;
    }
    
    if (copy -> get_npol() == 1) {
      cerr << data[i] << endl;
    }
    
  }

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "screenDump: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// Helper functions
//
void Pulsar::Interpreter::Tokenise(const string& str,
				   vector<string>& tokens,
				   const string& delimiters)
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos = str.find_first_of(delimiters, lastPos);
  
  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}

