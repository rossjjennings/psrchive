//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/LogFile.h

#ifndef __LogFile_h
#define __LogFile_h

#include "Reference.h"
#include <map>

//! A list of strings indexed by another string
class LogFile : public Reference::Able
{
 public:

  //! Null constructor
  LogFile () {}

  //! Construct from a filename
  LogFile (const std::string& filename);
  
  //! add a log message
  void add (const std::string& index, const std::string& message);
  
  //! get a log message
  std::string get_message (const std::string& index);
  
 protected:
  
  typedef std::map<std::string,std::string> container;
  container messages;
};

#endif
