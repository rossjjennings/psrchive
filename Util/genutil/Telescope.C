/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Telescope.h"
#include <string.h>

  const char Parkes = '7';
  const char ATCA = '2';
  const char Tidbinbilla = '6';
  const char Arecibo = '3';
  const char Hobart = '4';

//! Convert a telescope name to a code
char Telescope::code (const char* name)
{
  if (strcasecmp (name, "pks") == 0 || strcasecmp (name, "parkes") == 0)
    return Parkes;

  if (strcasecmp (name, "atca") == 0 || strcasecmp (name, "narrabri") == 0)
    return ATCA;

  if (strcasecmp (name, "tid") == 0 || strcasecmp (name, "tidbinbilla") == 0)
    return Tidbinbilla;

  if (strcasecmp (name, "Arecibo") == 0)
    return Arecibo;

  if (strcasecmp (name, "Hobart") == 0)
    return Hobart;

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

  default:
    return "";
  }
}
