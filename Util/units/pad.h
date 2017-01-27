//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// psrchive/Util/units/pad.h

#ifndef __UTILS_UNITS_PAD_H
#define __UTILS_UNITS_PAD_H

#include <string>

// Pad text with spaces up to length, to the right or left of the original
std::string pad (unsigned length, const std::string& text, bool right = true);

#endif
