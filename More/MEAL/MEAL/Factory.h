//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Attic/Factory.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:15 $
   $Author: straten $ */

#ifndef __Calibration_Factory_H
#define __Calibration_Factory_H

#include "Functor.h"
#include "stringtok.h"

#include <fstream>

namespace Factory {

  template< class T >
  T* load (const string& filename, Functor< T*(string) >& constructor,
	   bool verbose = false)
  {
    ifstream input (filename.c_str());
    if (!input)
      throw Error (FailedSys, "Factory::load", "ifstream (" + filename + ")");

    string line;

    T* instance = 0;

    while (!input.eof()) {

      getline (input, line);
      line = stringtok (line, "#\n", false);  // get rid of comments

      if (!line.length())
	continue;

      if (!instance) {

	// the first key loaded should be the name of the instance
	string key = stringtok (line, " \t");

	if (verbose)
	  cerr << "Factory::load construct new " << key << endl;

	instance = constructor (key);
	
	if (!line.length())
	  continue;

      }

      if (verbose)
	cerr << "Factory::load parse line '" << line << "'" << endl;
      
      instance->parse (line);

    }

    return instance;
 
 }

}

#endif

