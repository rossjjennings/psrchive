/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Telescope.h"
#include <string.h>

using namespace std;

//! Convert a telescope name to a code
char Telescope::code (const string& code)
{
  const char* name = code.c_str();

  if (strlen(name) == 1)
    return name[0];

  if (strcasecmp (name, "pks") == 0 || strcasecmp (name, "parkes") == 0)
    return Parkes;

  if (strcasecmp (name, "atca") == 0 || strcasecmp (name, "narrabri") == 0)
    return ATCA;

  if (strcasecmp (name, "tid") == 0 || strcasecmp (name, "tidbinbilla") == 0)
    return Tidbinbilla;

  if (strcasecmp (name, "Arecibo") == 0)
    return Arecibo;

  if (strcasecmp (name, "Hobart") == 0 || strcasecmp (name, "DSS43") == 0)
    return Hobart;

  if (strcasecmp (name, "AAT") == 0 || strcasecmp (name, "UAO") == 0)
    return AAT;

  if (strcasecmp (name, "GBT") || strcasecmp (name, "Greenbank") == 0)
    return Greenbank;

  if (strcasecmp (name, "WSRT") || strcasecmp (name, "Westerbork") == 0)
    return WSRT;
  
  return 0;
}

//! Convert a telecope code to a name
const char* Telescope::name (char code)
{
  switch (code) {
  case Parkes:
    return "Parkes";

  case ATCA:
    return "ATCA";

  case Tidbinbilla:
    return "Tidbinbilla";

  case Arecibo:
    return "Arecibo";

  case Hobart:
    return "Hobart";

  case WSRT:
    return "WSRT";

  default:
    return "";
  }
}
