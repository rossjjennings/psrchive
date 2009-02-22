/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "TextInterface.h"
#include "stringtok.h"
#include "pad.h"

#include <stdio.h>
#include <string.h>

#include <algorithm>

using namespace std;

bool TextInterface::label_elements = false;

string TextInterface::Parser::process (const string& command)
{
#ifdef _DEBUG
  cerr << "TextInterface::Parser::process command='" << command << "'" << endl;
#endif

  if (command == "help")
    return help ();

  string::size_type set = command.find('=');

  // if no equals sign is present, assume that command is get key
  if (set == string::npos)
  {
    string name = command;
    string value = get_name_value (name);

    string prefix;
    if (prefix_name)
      prefix = name + "=";

    return indentation + prefix + value;
  }

  // string before the equals sign
  string before = command.substr (0,set);
  // string after the equals sign
  string after = command.substr (set+1);

  // remove any white space from the key
  string param = stringtok (before, " \t");

  set_value (param, after);
  return "";
}

string TextInterface::Parser::help (bool default_value)
{
  unsigned i = 0;

  string name_label = "Attribute Name";
  string description_label = "Description";
  string value_label = (default_value) ? "Value" : "";

  unsigned max_namelen = name_label.length();
  unsigned max_descriptionlen = description_label.length();

#ifdef _DEBUG
  cerr << "TextInterface::Parser::help nvalue=" << get_nvalue() << endl;
#endif

  // find the maximum string length of the name and description
  for (i=0; i<get_nvalue(); i++)
  {
    if (get_name(i).length() > max_namelen)
      max_namelen = get_name(i).length();
    if (get_description(i).length() > max_descriptionlen)
      max_descriptionlen = get_description(i).length();
  }

  max_namelen += 3;
  max_descriptionlen += 3;

  // repeat the dash an appropriate number of times
  string sep (max_namelen + max_descriptionlen + value_label.length(), '-');

  // make a nice header
  string help_str =
    pad(max_namelen, name_label) + 
    pad(max_descriptionlen, description_label) + value_label + "\n" +
    sep + "\n";

  for (i=0; i<get_nvalue(); i++)
    help_str += 
      pad(max_namelen, get_name(i)) + 
      pad(max_descriptionlen, get_description(i)) +
      ((default_value) ? (get_value(get_name(i)) + "\n") : "\n");

  return help_str;
}

static bool alphabetical_lt (const Reference::To<TextInterface::Value>& v1, 
				const Reference::To<TextInterface::Value>& v2)
{
  return v1->get_name() < v2->get_name();
}


//! Add a new value interface
void TextInterface::Parser::add_value (Value* value)
{
  values.push_back (value);

  if (alphabetical)
    sort (values.begin(), values.end(), alphabetical_lt);
}

//! Remove the named value interface
void TextInterface::Parser::remove (const std::string& name)
{
  delete find (name);
  clean_invalid ();
}

void TextInterface::Parser::clean_invalid () 
{
  unsigned i=0; 
  while ( i < values.size() )
    if (!values[i])
      values.erase( values.begin() + i );
    else 
      i++;
}

TextInterface::Parser::Parser ()
{
  alphabetical = false;
  import_filter = false;
  prefix_name = true;
}

//! Get the value of the value
string TextInterface::Parser::get_value (const string& name) const
{
  string temp = name;
  return get_name_value (temp);
}

//! Get the value of the value
string TextInterface::Parser::get_name_value (string& name) const
{
  // an optional precision may be specified
  std::string::size_type dot = name.find('%');

  if (dot == std::string::npos)
    return find(name)->get_value();

  // strip off the precision
  string precision = name.substr(dot+1);
  name = name.substr(0,dot);

  ModifyRestore<unsigned> temp ( tostring_precision,
				 fromstring<unsigned> (precision) );

  return find(name)->get_value();
}

//! Set the value of the value
void TextInterface::Parser::set_value (const string& name, const string& value)
{
  find(name)->set_value(value);
}

//! Get the number of values
unsigned TextInterface::Parser::get_nvalue () const
{
  return values.size(); 
}

//! Get the name of the value
string TextInterface::Parser::get_name (unsigned i) const 
{
  return values[i]->get_name();
}

//! Get the description of the value
string TextInterface::Parser::get_description (unsigned i) const
{
  return values[i]->get_description();
}

//! Return a pointer to the named class value interface
TextInterface::Value* 
TextInterface::Parser::find (const string& name, bool throw_exception) const
{
#ifdef _DEBUG
  cerr << "TextInterface::Parser::find (" << name << ") size=" << values.size() 
	    << endl;
#endif

  string key = name;

  if (aliases)
    key = aliases->substitute (key);

  for (unsigned i=0; i<values.size(); i++)
  {
    if (values[i]->matches (key))
    {
#ifdef _DEBUG
      cerr << "TextInterface::Parser::find value[" << i << "]=" 
		<< values[i]->get_name() << " matches" << endl;
#endif
      const_cast<Parser*>(this)->setup( values[i] );
      return values[i];
    }
  }

  if (throw_exception)
    throw Error (InvalidParam, "TextInterface::Parser::find",
		 "no value named " + name);

  return 0;
}

//! Insert the interface into self
void TextInterface::Parser::insert (Parser* other)
{
  if (!other)
    return;

  for (unsigned i=0; i < other->values.size(); i++)
    if (!import_filter || !find(other->values[i]->get_name(),false))
    {
      other->setup( other->values[i] );
      add_value( other->values[i] );
    }
}

//! Import a nested interface
void TextInterface::Parser::insert (const std::string& prefix, Parser* other)
{
  if (!other)
    return;

  for (unsigned i=0; i < other->values.size(); i++)
    if (!import_filter || !find(other->values[i]->get_name(),false))
    {
      other->setup( other->values[i] );
      add_value( new NestedValue(prefix, other->values[i]) );
    }
}

//! Clear all nested interfaces
void TextInterface::Parser::clean ()
{
  for (unsigned i=0; i < values.size(); )
  {
    NestedValue* nested = dynamic_kast<NestedValue>( values[i] );
    if (nested)
      values.erase( values.begin() + i );
    else
      i++;
  }
}



bool TextInterface::NestedValue::matches (const std::string& name,
					  const std::string& prefix,
					  const Value* value)
{
#ifdef _DEBUG
  cerr << "TextInterface::NestedValue::matches prefix=" << prefix 
       << " name=" << name;
#endif

  std::string::size_type length = prefix.length();

  if ( name[length] != ':' )
  {
#ifdef _DEBUG
    cerr << " false" << endl;
#endif
    return false;
  }

  if ( strncmp (name.c_str(), prefix.c_str(), length) != 0 )
  {
#ifdef _DEBUG
    cerr << " false" << endl;
#endif
    return false;
  }

  std::string remainder = name.substr (length+1);

#ifdef _DEBUG
  cerr << " maybe" << endl;
  cerr << "TextInterface::NestedValue::matches nested name="
       << value->get_name() << " remain=" << remainder << endl;
#endif

  bool result = value->matches (remainder);

#ifdef _DEBUG
  cerr << "TextInterface::NestedValue::matches result=" << result << endl;
#endif
  return result;
}


/*! Parses text into key, range, and remainder
  \retval true if key matches name
*/
bool TextInterface::match (const string& name, const string& text,
			   string* range, string* remainder)
{
#ifdef _DEBUG
  cerr << "TextInterface::match name=" << name << " text=" << text;
#endif

  string::size_type length = name.length();

  if ( length && strncmp (text.c_str(), name.c_str(), length) != 0 )
  {
#ifdef _DEBUG
    cerr << " false" << endl;
#endif
    return false;
  }

  string::size_type end = text.find (':', length);
  if (end == string::npos)
  {
    if (remainder)
    {
#ifdef _DEBUG
      cerr << " false" << endl; 
#endif
      return false;
    }
    else
      end = text.length();
  }

#ifdef _DEBUG
  cerr << " maybe" << endl;
#endif

  // the range is everything between the end of the variable name and the colon
  *range = text.substr (length, end-length);

#ifdef _DEBUG
  cerr << "TextInterface::match range=" << *range << endl;
#endif

  // the remainder is everything following the colon
  if (remainder)
  {
    *remainder = text.substr (end+1);
#ifdef _DEBUG
    cerr << "TextInterface::match remainder=" << *remainder << endl;
#endif
  }

  // a map may have no name
  if (!length)
    return true;

  // the range may be a wildcard
  if ((*range) == "*" || (*range) == "?")
    return true;

  length = range->length();

  // a range may be empty ...
  if (!length)
    return true;

  // .. or must be enclosed in square brackets
  if ((*range)[0] == '[' && (*range)[length-1] == ']')
    return true;

#ifdef _DEBUG
  cerr << "FALSE" << endl;
#endif

  return false;
}

void check_range (int i, int n, const char* name)
{
  if (i < 0)
    throw Error (InvalidRange, "TextInterface::parse_indeces",
		 "%s=%d < 0", name, i);

  if (i >= n)
    throw Error (InvalidRange, "TextInterface::parse_indeces",
		 "%s=%d >= size=%d", name, i, n);
}

void TextInterface::parse_indeces (vector<unsigned>& index,
				   const string& name,
				   unsigned size)
{
  if (name == "*")
    return;

  if (name == "") {
    index.resize (size);
    for (unsigned i=0; i<size; i++)
      index[i] = i;
    return;
  }

  string::size_type length = name.length();

  string range = name;
  
  // look for the opening and closing braces
  if (name[0] == '[' && name[length-1] == ']')
    range = name.substr (1,length-2);
  else if (name[0] == '[' || name[length-1] == ']')
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "unbalanced brackets in '" + name + "'");

#ifdef _DEBUG
  cerr << "TextInterface::parse_indeces range started" << endl;
#endif

  if (range.empty())
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "no range in '" + name + "'");

  for (unsigned i=0; i<range.size(); i++)
    if (!isdigit(range[i]) && range[i] != '-' && range[i] != ',')
      throw Error (InvalidParam, "TextInterface::parse_indeces",
		   "invalid range '" + range + "'");

  string backup = range;

  while (!range.empty()) {

    string sub = stringtok (range, ",", false, false);

    if (range[0]==',')
      range.erase(0,1);
    
    int start, end;
    char c;

    int scanned = sscanf (sub.c_str(), "%d%c%d", &start, &c, &end);

    if (scanned == 3)  {

      if (c != '-')
        throw Error (InvalidParam, "TextInterface::parse_indeces",
                     "invalid sub-range '" + range + "'");

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces start=" << start 
	   << " end=" << end << endl;
#endif

      check_range (start, size, "start");
      check_range (end, size, "end");

      if (end < start)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "end=%d < start=%d", end, start);

      for (int i=start; i <= end; i++)
	index.push_back (i);

    }

    else if (scanned == 2)  {

      if (c != '-')
        throw Error (InvalidParam, "TextInterface::parse_indeces",
                     "invalid sub-range '" + range + "'");

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces index=" << start << endl;
#endif

      check_range (start, size, "start");

      for (unsigned i=start; i < size; i++)
	index.push_back (i);

    }

    else if (scanned == 1)  {

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces index=" << start << endl;
#endif

      check_range (start, size, "start");

      index.push_back (start);

    }

    else
      throw Error (InvalidParam, "TextInterface::parse_indeces",
		                 "invalid sub-range '" + sub + "'");
  }

}

