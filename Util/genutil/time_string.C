#include "string_utils.h"

// simple function returns the time in a user-friendly string
string time_string (double seconds)
{
  if (seconds > 86400.0)
   return stringprintf ("%.1fhr", seconds / 86400.0);
  else if (seconds > 3600.0)
    return stringprintf ("%.1fhr", seconds / 3600.0);
  else if (seconds > 60.0)
    return stringprintf ("%.1fmin", seconds / 60.0);
  else if (seconds > 0.1)
    return stringprintf ("%.2fs", seconds);
  else
    return stringprintf ("%.2fms", seconds * 1000.0);
}

