//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/parse.h,v $
   $Revision: 1.1 $
   $Date: 2005/12/05 03:07:36 $
   $Author: straten $ */

#ifndef __MEAL_FileParse_H
#define __MEAL_FileParse_H

#include "stringtok.h"
#include <fstream>

namespace MEAL {

  template< class T >
  void parse (const std::string& filename, T* instance)
  {
    std::ifstream input (filename.c_str());
    if (!input)
      throw Error (FailedSys, "MEAL::parse", "ifstream (" + filename + ")");

    std::string line;

    while (!input.eof()) {

      std::getline (input, line);
      line = stringtok (line, "#\n", false);  // get rid of comments

      if (!line.length())
	continue;

      instance->parse (line);

    }

  }

}

#endif

