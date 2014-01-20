//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/stringcase.h

#ifndef __STRINGCASE_H
#define __STRINGCASE_H

#include <string>

//! Makes the string lowercase
std::string lowercase (std::string);

//! Makes the string uppercase
std::string uppercase (std::string);

//! Performs a case-insensitive comparison of two strings
bool casecmp (const std::string&, const std::string&);

#endif

