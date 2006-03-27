//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/separate.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/27 21:38:17 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_SEPARATE_H
#define __UTILS_UNITS_SEPARATE_H

#include <vector>
#include <string>

//! Separate a list of comma-separated commands into a vector of strings
void separate (std::string cmds, std::vector<std::string>&, bool& edit);
void separate (std::string cmds, std::vector<std::string>&, char lim);
void separate (std::string cmds, std::vector<std::string>&);
void separate (std::string cmds, std::vector<std::string>&, bool& e, char lim);

#endif

