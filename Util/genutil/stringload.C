#include "string_utils.h"

// //////////////////////////////////////////////////////////////////
// stringload
// fills a string with characters from the stream described by FILE*
// until nbytes are read, or an EOF condition arises.
// return value is number of characters read (not incl. \0) or -1 on
// error
// //////////////////////////////////////////////////////////////////

ssize_t stringload (string* str, FILE* fptr, size_t nbytes)
{
  char rdline [101];

  size_t eachread = 100;
  size_t bytesread = 0;
  while (!nbytes || bytesread<nbytes) {
    if (nbytes && eachread > (nbytes-bytesread))
      eachread = nbytes-bytesread;
    size_t bytes = fread (rdline, 1, eachread, fptr);
    if (!bytes)
      break;
    rdline [bytes] = '\0';
    *str += rdline;
    bytesread += bytes;
  }
  if (ferror (fptr))  {
    perror ("stringload: fread failed");
    return -1;
  }
  return bytesread;
}

