//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/units/stringtok.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:18:33 $
   $Author: straten $ */

#ifndef __STRINGTOK_H
#define __STRINGTOK_H

#include <string>

// ///////////////////////////////////////////////////////////
// returns the first sub-string of 'instr' delimited by
// characters in 'delimiters'.  the substring and any leading
// delimiter characters are removed from 'instr'
// ///////////////////////////////////////////////////////////
std::string stringtok (std::string& instr, const std::string & delimiters,
                  bool skip_leading_delimiters = true,
                  bool strip_leading_delimiters_from_remainder = true);

#endif

