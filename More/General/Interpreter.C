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
  initialize_readline ("psrsh");

  inplace = false;
  clobber = true;
  stopwatch = false;
  reply = true;
  
  prompt = "psrsh> ";
  
  add_command
    (&Interpreter::get_report,
     "report", "display plugin information",
     "no arguments required\n");
  
  add_command 
    ( &Interpreter::load,
      "load", "load archive from file",
      "usage: load <filename> [name]\n"
      "  string filename   name of the file to be read \n"
      "  string name       if specified, assign name to archive\n" );
  
  add_command 
    ( &Interpreter::unload,
      "unload", "unload archive or set unload options",
      "usage: unload [name] <filename> \n"
      "  string filename   name of the file to be written \n"
      "  string name       if specified, unload the named archive \n"
      "or: unload extension <ext> \n"
      "  string ext        extension added to filename when unloading \n"
      "or: unload path <dir> \n"
      "  string dir        path to which archives are unloaded \n");
  
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
      "usage: clone [from] <name> \n"
      "  string from       name of archive to be cloned \n"
      "                    if not specified, clone current archive \n"
      "  string name       name to be given to clone \n" );

  add_command
    ( &Interpreter::extract,
      "extract", "duplicate part of an archive",
      "usage: extract [from] [to] <subints> \n"
      "  string from       name of archive from which to extract \n"
      "                    if not specified, current archive is used \n"
      "  string to         name to be given to extracted copy \n"
      "                    if not specified, copy will become current \n"
      "  unsigned subints  range[s] of subints to be extracted \n" );
  
  add_command
    ( &Interpreter::edit,
      "edit", "edit archive parameters",
      "usage: edit <command> ...\n"
      "  string command    any edit command as understood by psredit \n" );
  
  add_command
    ( &Interpreter::append,
      "append", "append data from one archive to another",
      "usage: append [onto] <other> \n"
      "  string onto       name of archive to which other will be appended \n"
      "  string other      name of archive to be appended \n" );
  
  add_command 
    ( &Interpreter::fscrunch, 'F',
      "fscrunch", "integrate archive in frequency",
      "usage: fscrunch [name] [chans] \n"
      "  string name       name of archive to be fscrunched \n"
      "                    if not specified, current archive is used \n"
      "  unsigned chans    number of desired frequency channels \n"
      "                    if not specified, fscrunch all (chans=1)\n" );
  
  add_command 
    ( &Interpreter::tscrunch, 'T',
      "tscrunch", "integrate archive in time",
      "usage: tscrunch [name] [subints] \n"
      "  string name       name of archive to be tscrunched \n"
      "                    if not specified, current archive is used \n"
      "  unsigned subints  number of desired sub-integrations \n"
      "                    if not specified, tscrunch all (subints=1)\n" );

  add_command 
    ( &Interpreter::pscrunch, 'p',
      "pscrunch", "integrate archive in polarisation",
      "usage: pscrunch [name] \n"
      "  string name       name of archive to be tscrunched \n"
      "                    if not specified, current archive is used \n" );
  
  add_command
    ( &Interpreter::bscrunch, 'B',
      "bscrunch", "integrate archive in phase bins",
      "usage: bscrunch [name] bins \n"
      "  string name       name of archive to be tscrunched \n"
      "                    if not specified, current archive is used \n"
      "  unsigned bins     number of desired phase bins\n" );

  add_command 
    (&Interpreter::centre, 'C',
     "centre", "centre all profiles in an archive",
     "centre <string>   centre stack archive\n");

  add_command 
    (&Interpreter::dedisperse, 'D',
     "dedisp", "dedisperse all profiles in an archive",
     "dedisp <string>   dedisperse stack archive\n");

  add_command 
    (&Interpreter::defaraday, 'R',
     "defaraday", "apply faraday rotation correction",
     "usage: defaraday [name]\n"
     "  string name    name of archive to correct" );

  add_command 
    (&Interpreter::correct_instrument,
     "pac", "apply parallactic angle corrections",
     "pac <string>   apply parallactc angle corrections to a stack item\n");

  add_command 
    (&Interpreter::scattered_power_correct,
     "spc", "apply scattered power corrections",
     "spc <string>   apply scattered power corrections to a stack item\n");

  add_command 
    (&Interpreter::screen_dump,
     "screendump", "display raw data points",
     "screendump <string>   write out the raw data: polns in columns\n");
  
  add_command 
    (&Interpreter::toggle_clobber,
     "clobber", "toggle overwrite permission",
     "clobber   set whether or not you can clobber stack items\n");

}

Pulsar::Interpreter::Interpreter()
{
  init ();
}

//! construct from command line arguments
Pulsar::Interpreter::Interpreter (int &argc, char** &argv)
{
  for (int i=0; i<argc; i++)
    cerr << "args[" << i << "]=" << argv[i] << endl;

  init ();
}

//! destructor
Pulsar::Interpreter::~Interpreter ()
{

}

string Pulsar::Interpreter::get_report (const string& args)
{
  string ok;
  
  if (reply)
    ok = "ok";
  
  Pulsar::Archive::agent_report ();
  
  return ok;
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
    return "";

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
  cerr << "Pulsar::Interpreter::get '" << name << "'" << endl;
  map< string, Reference::To<Archive> >::iterator entry = theMap.find (name);

  if (entry == theMap.end()) {
    cerr << "Pulsar::Interpreter::get '" << name << "' not found" << endl;
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

  if (!arguments.size() || arguments.size() > 2)
    return response (Fail, "clone: please specify one or two names");

  string name = arguments[0];

  /* if two names are specified, get the name of the first argument
     and name the clone with the second argument.  otherwise, clone
     the current top of the stack */
  if (arguments.size() == 2) {
    set( getmap(name) );
    name = arguments[1];
  }

  setmap( name, get()->clone() );
}
catch (Error& error) {
  return response (Fail, "clone: " + error.get_message());
}

string Pulsar::Interpreter::extract (const string& args)
try {
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, "extract: please specify one or two names");

  unsigned range = 0;

  /*
    if the first argument is a valid map name, then start with this archive;
    otherwise, start with the top of the stack 
  */

  Reference::To<Archive> archive = getmap (arguments[0], false);
  string from_name;
  if (archive) {
    range ++;
    from_name = arguments[0];
  }
  else {
    archive = get();
    from_name = "_top";
  }

  if (!archive)
    return response (Fail, "extract: no data");

  /*
    if the first/next argument does not parse into a range, then assume
    that it is the name to be given to the result
  */

  string into_name;
  vector<unsigned> indeces;
  TextInterface::parse_indeces (indeces, arguments[range]);

  if (!indeces.size()) {
    into_name = arguments[range];
    range++;
  }

  /*
    parse the remaining options as though they were indeces
  */
  for (unsigned i = range; i < arguments.size(); i++)
    TextInterface::parse_indeces (indeces, arguments[i]);

  /*
    extract the sub-integrations
  */
  set( archive -> extract(indeces) );

  /*
    if into_name is set, then name the result
  */
  if (into_name.length())
    setmap( into_name, get() );
  else
    into_name = "_top";

  /*
    report
  */
  string ok = "extract: subint(s) ";
  for (unsigned k = range; k < arguments.size(); k++)
    ok += arguments[k] + " ";
  ok += "from " + from_name + " into " + into_name;

  return response (Good, ok);
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

  if (!arguments.size() || arguments.size() > 2)
    return response (Fail, "append: please specify one or two names");

  string name = arguments[0];

  /* if two names are specified, get the first argument and append the
     second argument.  otherwise, append the first argument the
     current top of the stack */
  if (arguments.size() == 2) {
    cerr << "Pulsar::Interpreter::append to '" << name << "'" << endl;
    set( getmap (name) );
    name = arguments[1];
  }

  cerr << "Pulsar::Interpreter::append other '" << name << "'" << endl;
  Archive::set_verbosity(3);
  get()->append( getmap(name) );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "append: " + error.get_message());
}

string Pulsar::Interpreter::load (const string& args)
try {
  vector<string> arguments = setup (args);
  
  if (!arguments.size() || arguments.size() > 2)
    return response (Fail, "load: please specify filename [name]");

  set( Archive::load(arguments[0]) );

  if (arguments.size() == 2)
    setmap( arguments[1], get() );

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "load: " + error.get_message());
}

string Pulsar::Interpreter::unload (const string& args)
try {
  vector<string> arguments = setup (args);
  
  if (arguments.size() == 2 && arguments[0] == "extension") {
    unload_extension = arguments[1];
    return response (Good);
  }
  if (arguments.size() == 2 && arguments[0] == "path") {
    unload_path = arguments[1];
    return response (Good);
  }
  if (!arguments.size() || arguments.size() > 2)
    return response (Fail, "unload: please specify [name] filename");
  
  string name = arguments[0];

  /* if two names are specified, get the name of the first argument
     and unload to the second argument.  otherwise, write the current
     top of the stack to the first argument */

  if (arguments.size() == 2) {
    set( getmap (name) );
    name = arguments[1];
  }

  string newname;
	
  if (!unload_path.empty()) {
    newname = unload_path;
    newname += "/";
  }

  newname += name;
  newname += unload_extension;

  get()->unload(newname);

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "unload: " + error.get_message());
}

unsigned Pulsar::Interpreter::setup_get (const std::string& args)
{
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return 0;

  unsigned retval = 0;

  if (arguments.size() == 1) {
    // the only argument could be either a map name or the unsigned integer
    if (sscanf(arguments[0].c_str(), "%u", &retval) == 1)
      return retval;
  }
  
  else if (arguments.size() == 2) {
    // the second argument must be an unsigned integer
    if (sscanf(arguments[1].c_str(), "%u", &retval) != 1)
      throw Error (InvalidParam, "Pulsar::Interpreter::setup_get",
		   "failed to parse '" + arguments[1] + "' as unsigned");
  }

  else
    throw Error (InvalidParam, "Pulsar::Interpreter::setup_get",
		 "invalid arguments: '" + args + "'");

  set( getmap (arguments[0]) );
}

// //////////////////////////////////////////////////////////////////////
//
// fscrunch <string> <int>
//
string Pulsar::Interpreter::fscrunch (const string& args)
try {
  unsigned scrunch_to = setup_get (args);
  
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
  unsigned scrunch_to = setup_get (args);
  
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
  unsigned scrunch_to = setup_get (args);
  
  if (scrunch_to)
    return response (Fail, "pscr: accepts no arguments");

  get() -> pscrunch();

  return response (Good);
}
catch (Error& error) {
  return response (Fail, "pscr: " + error.get_message());
}

string Pulsar::Interpreter::bscrunch (const string& args)
try {
  unsigned scrunch_to = setup_get (args);
  
  if (!scrunch_to)
    return response (Fail, "bscr: requires a valid number of bins");

  get() -> bscrunch_to_nbin (scrunch_to);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, "bscr: " + error.get_message());
}

string Pulsar::Interpreter::centre (const string& args)
try {
  if (args.length())
    set( getmap (args) );
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
    set( getmap (args) );

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
    set( getmap (args) );

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
    set( getmap (args) );

  if (get()->get_faraday_corrected())
    return response (Warn, "frc: already corrected");

  get()->defaraday();
  return response (Good);
}
catch (Error& error) {
  return response (Fail, "frc: " + error.get_message());
}

// //////////////////////////////////////////////////////////////////////
//
// spc <string>
//

string Pulsar::Interpreter::scattered_power_correct (const string& args)
try {
  if (args.length())
    set( getmap (args) );

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
  string ok;
  
  clobber = !clobber;
  
  if (reply) {
    if (clobber) ok = "Clobber set to true";
    if (!clobber) ok = "Clobber set to false";
  }
  
  return ok;
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

