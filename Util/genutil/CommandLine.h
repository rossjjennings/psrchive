//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/CommandLine.h,v $
   $Revision: 1.4 $
   $Date: 2009/08/25 09:06:10 $
   $Author: straten $ */

#ifndef __CommandLine_h
#define __CommandLine_h

#include "tostring.h"

#include <string>
#include <vector>

#include <getopt.h>

namespace CommandLine {

  typedef std::pair<std::string,std::string> Help;

  //! A single item in a command line menu
  class Item
  {
  public:

    virtual ~Item () { }

    //! Return true if code matches
    virtual bool matches (int code) const = 0;

    //! Handle the argument
    virtual void handle (const std::string& arg) = 0;

    //! Return two columns of help text
    virtual Help get_help () const = 0;
  };

  class Menu;

  //! A single command line argument
  class Argument : public Item
  {

  protected:

    //! Single-character name of value
    std::string short_name;

    //! Long name of value
    std::string long_name;

    //! Type of value
    std::string type;

    //! Brief description of value
    std::string reminder;

    //! Detailed description of value
    std::string long_help;

    //! The has_arg attribute used by getopt_long
    int has_arg;

    //! Code assigned to this Argument by Menu class
    int val;

    friend class Menu;

  public:

    Argument () { val = 0; has_arg = no_argument; }

    void set_short_name (char c) { short_name = std::string(1,c); }
    void set_long_name (const std::string& s) { long_name = s; }
    void set_type (const std::string& s) { type = s; }
    void set_reminder (const std::string& s) { reminder = s; }
    void set_long_help (const std::string& s) { long_help = s; }
    void set_has_arg (int h) { has_arg = h; }

    //! Return true if key matches
    bool matches (int c) const { return val == c; }

    Help get_help () const;
  
  };

  //! A command line value
  template<typename T>
  class Value : public Argument
  {
  protected:

    //! Reference to the value to be set
    T& value;

  public:

    //! Default constructor
    Value (T& _value)
      : value (_value) { has_arg = required_argument; }

    //! Handle the argument
    void handle (const std::string& arg) { value = fromstring<T>(arg); }
  };

  //! A command line value that may be specified multiple times
  template<typename T>
  class Value< std::vector<T> > : public Argument
  {
  protected:

    //! Reference to the value to be set
    std::vector<T>& values;

  public:

    //! Default constructor
    Value (std::vector<T>& _values)
      : values (_values) { has_arg = required_argument; }

    //! Handle the argument
    void handle (const std::string& arg) 
    {
      values.push_back( fromstring<T>(arg) );
    }

  };

  //! A command line toggle
  template<>
  class Value<bool> : public Argument
  {
  protected:

    //! Reference to the value to be set
    bool& value;

  public:

    //! Default constructor
    Value (bool& _value) : value (_value) { }

    //! Handle the argument
    void handle (const std::string& arg) { value = !value; }
  };

  class Heading : public Item
  {

  public:

    //! Default constructor
    Heading (const std::string& _text) { text = _text; }

    //! Return true if key matches
    bool matches (const std::string& key) const { return false; }

    //! Handle the argument
    void handle (const std::string& arg) { }

    //! Return two columns of help text
    Help get_help () const { return Help(text,""); }

  protected:

    std::string text;
  };


  //! A command line menu
  class Menu
  {
  public:

    virtual ~Menu ();

    //! Parse the command line
    virtual void parse (int argc, char* const *argv);

    //! Add an item to the menu
    virtual void add (Item*);

    //! Add a Value with only a single letter name
    template<typename T>
    Argument* add (T& value, char short_name)
    { 
      Argument* argument = new Value<T> (value);
      argument->set_short_name (short_name);
      item.push_back (argument);
      return argument;
    }

    //! Add a Value with only a long string name
    template<typename T>
    Argument* add (T& value, const std::string& long_name)
    { 
      Argument* argument = new Value<T> (value);
      argument->set_long_name (long_name);
      item.push_back (argument);
      return argument;
    }

  protected:

    std::vector<Item*> item;
  };


}

#endif
