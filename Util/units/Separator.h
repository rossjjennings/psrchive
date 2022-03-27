//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/Separator.h

#ifndef __Separator_H
#define __Separator_H

#include <string>
#include <vector>
#include <map>

// #define _DEBUG 1

#if _DEBUG
#include <iostream>
#endif

//! separates a string into a container of strings using various rules
class Separator
{
  std::string delimiters;
  std::string opening_brackets;
  std::map<char,char> brackets;

  bool preserve_numeric_ranges;
  bool isnumeric (char c);
  bool part_of_numeric_range (const std::string& s, std::string::size_type pos);

 public:

  // construct with the default set of brackets
  Separator ();

  // add a pair of brackets that protect contents from separation
  void add_bracket (char open, char close);

  // set the delimiters used to separate
  void set_delimiters (const std::string& d) { delimiters = d; }

  // do not separate tokens that appear to be part of a numeric range
  void set_preserve_numeric_ranges (bool f) { preserve_numeric_ranges = f; }

  template<typename Container>
    void separate (std::string text, Container& container);
};

template<typename Container>
void Separator::separate (std::string text, Container& container)
{
#if _DEBUG
  std::cerr << "Separator::separate"
    " delimiters=\"" << delimiters << "\""
    " opening brackets=\"" << opening_brackets << "\"" << std::endl;
#endif
  std::string opener = delimiters + opening_brackets;

  while (text.length())
  {
    /*
      search for the first instance of a delimiter that is
      
      1) not enclosed in brackets
      2) not part of a range of numbers
      
      nested brackets are not supported
    */
    
    std::string::size_type end = 0;

    while ( (end = text.find_first_of (opener, end) ) != std::string::npos )
    {
      if ( opening_brackets.find (text[end]) != std::string::npos )
      {
#if _DEBUG
	std::cerr << "Separator::separate '" << text[end] << "'"
	  " is an opening bracket" << std::endl;
#endif
	end = text.find (brackets[text[end]], end+1);
      }

      else if (preserve_numeric_ranges && part_of_numeric_range (text, end))
      {
#if _DEBUG
	std::cerr << "Separator::separate '" << text[end] << "'"
	  " is part of a numeric range" << std::endl;
#endif
	end ++;
      }
      else
      {
#if _DEBUG
	std::cerr << "Separator::separate '" << text[end] << "'"
	  " is naked" << std::endl;
#endif
	break;
      }
    }
    
    // the first naked delimiter
    std::string token = text.substr (0, end);

    std::string::size_type length = token.length();
    
#if _DEBUG
    std::cerr << "Separator::separate token='" << token << "'" << std::endl;
#endif
    
    if ( (token[0] == '"' && token[length-1] == '"') ||
         (token[0] == '\'' && token[length-1] == '\'') )
    {
      token = token.substr (1, length-2);
#if _DEBUG
      std::cerr << "Separator::separate quotation stripped token ='" << token << "'" << std::endl;
#endif
    }

    container.push_back (token);
    
    end = text.find_first_not_of (delimiters, end);
    text.erase (0, end);

#if _DEBUG
    std::cerr << "Separator::separate text='" << text << "'" << std::endl;
#endif
    
  }
}


#endif // !__Separator_H
