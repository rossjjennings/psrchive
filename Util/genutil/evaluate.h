//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/evaluate.h,v $
   $Revision: 1.1 $
   $Date: 2007/02/08 13:07:25 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_EVALUATE_H
#define __UTILS_UNITS_EVALUATE_H

#include <string>

//! Replaces strings enclosed in { } with the mathematical expression
std::string evaluate (const std::string& text, char cstart='{', char cend='}');

#endif
