#include <stdio.h>
#include <string>
#include <stdarg.h>

std::string
stringprintf(char *fmt ...)
{
  static char buf[1024];
  va_list args;

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  return std::string(buf);
}
