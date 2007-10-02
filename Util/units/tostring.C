/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "tostring.h"

std::ostringstream* tostring_ost = 0;
std::istringstream* fromstring_ist = 0;

unsigned tostring_precision = 0;
bool tostring_places = false;

