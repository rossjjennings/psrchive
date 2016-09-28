//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/TextInterfaceInterpreter.h

#ifndef __TextInterfaceInterpreter_h
#define __TextInterfaceInterpreter_h

#include "TextInterface.h"

namespace TextInterface {

  //! Text interface to a text interpreter
  /*! In this template: I is a the interpreter class; Get is the
    method of I that returns a string; and Set is the method of I that
    accepts a string */
  template<class I, class Get, class Set>
  class Interpreter : public Value {

  public:
    
    //! Set all attributes on construction
    Interpreter (const std::string& _name, I* ptr, Get _get, Set _set)
    { name = _name, interpreter = ptr, get = _get; set = _set; }

    //! Get the name of the value
    std::string get_name () const
      { return name; }

    //! Get the value as text
    std::string get_value () const
      { return (interpreter->*get)(); }

    //! Set the value as text
    void set_value (const std::string& text)
      { (interpreter->*set)(text); }

    //! Get the description of the value
    std::string get_description () const
      { return description; }

    //! Set the description of the value
    void set_description (const std::string& text)
      { description = text; }
       
    //! Get the detailed description of the value
    std::string get_detailed_description () const
      { return detailed_description; }

    //! Set the detailed description of the value
    void set_detailed_description (const std::string& text)
      { detailed_description = text; }

    void set_modifiers (const std::string& modifiers) const
    {
      tostring_precision = fromstring<unsigned> (modifiers);
    }

    void reset_modifiers () const
    {
      tostring_precision = 0;
    }
    
  protected:

    std::string name;
    std::string description;
    std::string detailed_description;
    I* interpreter;
    Get get;
    Set set;

  };

  template<class I, class Get, class Set>
    Interpreter<I,Get,Set>* 
    new_Interpreter (const std::string& n, I* i, Get g, Set s)
    {
      return new Interpreter<I,Get,Set> (n, i, g, s);
    }

  template<class I, class Get, class Set>
    Interpreter<I,Get,Set>* 
    new_Interpreter (const std::string& n, const std::string& d, 
		     I* i, Get g, Set s)
    {
      Interpreter<I,Get,Set>* result = new Interpreter<I,Get,Set> (n, i, g, s);
      result->set_description (d);
      return result;
    }

  template<class I, class Get, class Set>
    Interpreter<I,Get,Set>* 
    new_Interpreter (const std::string& n, const std::string& d, 
		     const std::string& dd,
		     I* i, Get g, Set s)
    {
      Interpreter<I,Get,Set>* result = new Interpreter<I,Get,Set> (n, i, g, s);
      result->set_description (d);
      result->set_detailed_description (dd);
      return result;
    }

}

#endif
