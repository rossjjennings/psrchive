#include "string_utils.h"

// //////////////////////////////////////////////////////////////////
// stringload
// fills a string with characters from the stream described by FILE*
// until nbytes are read, or an EOF condition arises.
// return value is number of characters read (not incl. \0) or -1 on
// error
// //////////////////////////////////////////////////////////////////

static const unsigned rdsize = 256;
static char* rdline = NULL;

ssize_t stringload (string* str, FILE* fptr, size_t nbytes)
{
  if (rdline == NULL) rdline = new char [rdsize];
  size_t eachread = rdsize;
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

ssize_t stringload (string* str, istream &istr, streamsize nbytes)
{
  if (rdline == NULL) rdline = new char [rdsize];
  streamsize eachread  = rdsize;
  streamsize bytesread = 0;

  streampos start_pos = istr.tellg();

  while (!nbytes || bytesread<nbytes) {
    if (nbytes && eachread > (nbytes-bytesread))
      eachread = nbytes-bytesread;

    istr.read (rdline, eachread);
    streampos bytes = istr.tellg() - (bytesread + start_pos);
    if (bytes < 1)
      break;
    rdline [bytes] = '\0';
    *str += rdline;
    bytesread += bytes;
  }
  return bytesread;
}

// //////////////////////////////////////////////////////////////////
// stringload
// fills a vector of string with the first word from each line
// in the file.  A line is delimited by \n or commented by #.
// //////////////////////////////////////////////////////////////////

int stringfload (vector<string>* lines, const char* filename)
{
   FILE* fptr = fopen (filename, "r");
   if (fptr == NULL) {
     fprintf (stderr, "stringload:: Could not open %s", filename);
     perror ("");
     return -1;
   }
   int ret = stringload (lines, fptr);
   fclose (fptr);
   return ret;
}

int stringload (vector<string>* lines, FILE* fptr)
{
  static char* rdline = NULL;
  if (!rdline) rdline = new char [FILENAME_MAX+1];

  char* whitespace = " \t\n";
  // load the lines from file
  while (fgets (rdline, FILENAME_MAX, fptr) != NULL) {
    char* eol = strchr (rdline, '#');
    if (eol)
      *eol = '\0';
    char* line = strtok (rdline, whitespace);
    if (line) {
      lines->push_back (string(line));
    }
  }
  return 0;
}
