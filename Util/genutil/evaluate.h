//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// psrchive/Util/genutil/evaluate.h

#ifndef __UTILS_UNITS_EVALUATE_H
#define __UTILS_UNITS_EVALUATE_H

#include <string>

//! Replaces strings enclosed in { } with the mathematical expression
std::string evaluate (const std::string& text, char cstart='{', char cend='}');

//! Evaluates a mathematical expression
double compute (const std::string& text);

#endif
