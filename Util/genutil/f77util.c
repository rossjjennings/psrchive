/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "f77util.h"

#include <string.h>
#include <ctype.h>

void f2cstr (const char* f_str, char* c_str, unsigned length)
{
  /* fprintf (stderr, "f2cstr in '%.*s'\n", length, f_str); */

  unsigned i = length-1;
  
  strncpy (c_str, f_str, length);

  c_str[length] = '\0';

  while (f_str[i] == ' ' || !isascii(f_str[i]))
  {
    c_str[i] = '\0';
    if (i == 0)
      break;
    i--;
  }

  /* fprintf (stderr, "f2cstr out '%s'\n", c_str); */
}

void c2fstr (char* f_str, const char* c_str, unsigned length)
{
  unsigned i = strlen(c_str);
  
  strncpy (f_str, c_str, length);
  for (; i < length; i++)
    f_str[i] = ' ';
}

