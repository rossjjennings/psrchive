//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/separate.h

#ifndef __UTILS_UNITS_SEPARATE_H
#define __UTILS_UNITS_SEPARATE_H

#include <vector>
#include <string>

//! Return true if the input is enclosed in bracket
bool braced (const std::string&);

//! Separate a list of delimiter-separated commands into a vector of strings
void separate (std::string text, std::vector<std::string>& list,
	       const std::string& delimiters = " \t\n");

void standard_separation (std::vector<std::string>& list, const std::string&);

#endif

