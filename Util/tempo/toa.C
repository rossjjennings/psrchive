#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <time.h>

#include "string_utils.h"
#include "polyco.h"
#include "toa.h"

toa::toa (char* datastr)
{
  if (load (datastr) < 0) {
    throw ("toa::toa (char* datastr) failed");
  }
}

toa::toa (const toa & in_toa)
{
  init ();
  this->operator=(in_toa);
}

// ////////////////////////////////////////////////////////////////////////
//
// Parkes_load,unload - load/unload TOA in the Parkes Format
// 
//   columns     item
//     1-1     must be blank
//     26-34   Observing frequency (MHz)
//     35-55   TOA (decimal point must be in column 42)
//     56-63   Phase offset (fraction of P0, added to TOA)
//     64-71   TOA uncertainty
//     80-80   Observatory (one-character code)
//
// ////////////////////////////////////////////////////////////////////////

int toa::Parkes_load (const char* instring)
{
  destroy ();
  if ((instring[0]=='C') || (instring[0]=='c'))
    deleted = 1;
  return parkes_parse (instring+25);
}

int toa::parkes_parse (const char* instring)
{
  int scanned = sscanf (instring, "%lf %s %f %f %d",
 			&frequency, datestr, &phs, &error, &telescope);
  if (!deleted && scanned < 5) {
    // an invalid line was not commented out
    fprintf (stderr, 
	     "toa::parkes_parse(char*) Error scan:%d/5 '%s'\n",
	     scanned, instring);
    return -1;
  }
  if (arrival.Construct(datestr) < 0)  {
    fprintf (stderr, "toa::parkes_parse(char*) Error MJD parsing '%s'.\n",
	     datestr);
    return -1;
  }

  if (!valid())
    return -1;

  format = Parkes;
  return 0;
}

int toa::Parkes_unload (char* outstring) const
{
  for (int ic=0; ic<25; ic++)
    outstring [ic] = ' ';

  if (deleted)
    outstring[0]='C';

  return parkes_out (outstring+25);
}

int toa::parkes_out (char* outstring) const
{
  // output the basic line
  sprintf (datestr, "%8.7lf", frequency);
  sprintf (outstring, "%9.9s  %s   %4.2f%8.2f         %1d ",
 	   datestr, arrival.printdays(13).data(), phs, error, telescope);
  return 0;
}

int toa::Parkes_unload (FILE* outstream) const
{
  sizebuf (81);
  Parkes_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// Princeton_load,unload - load/unload TOA in the Princeton Format
// 
//   columns     item
//    1-1     Observatory (one-character code)
//    2-2     must be blank
//    16-24   Observing frequency (MHz)
//    25-44   TOA (decimal point must be in column 30 or column 31)
//    45-53   TOA uncertainty 
//    69-78   DM correction (pc cm^-3)
//
// ////////////////////////////////////////////////////////////////////////

int toa::Princeton_load (const char* instring)
{
  destroy ();

  telescope = instring[0] - '0';

  int scanned = sscanf (instring+15, "%lf %s %f %f",
 			&frequency, datestr, &error, &dmc);

  if (scanned < 4) {
    // an invalid line was not commented out
    fprintf (stderr, 
	     "toa::Princeton_load(char*) Error scanning '%s'\n",
	     instring);
    return -1;
  }
  if (arrival.Construct(datestr) < 0)  {
    fprintf (stderr, "toa::Princeton_load(char*) Error MJD parsing '%s'.\n",
	     datestr);
    return -1;
  }

  if (!valid())
    return -1;

  format = Princeton;
  return 0;
}

int toa::Princeton_unload (char* outstring) const
{
  // output the basic line
  outstring[0] = char ('0' + telescope);

  sprintf (outstring+15, "%8.7g %13.13s %4.2f %8.2f",
 	   frequency, arrival.printdays(13).data(), error, dmc);
  return 0;
}

int toa::Princeton_unload (FILE* outstream) const
{
  sizebuf (81);
  Princeton_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// Psrclock_load,unload - unload TOA in the Psrclock Format
// 
// ////////////////////////////////////////////////////////////////////////

int toa::Psrclock_load (const char* instring)
{
  destroy ();

  char* whitespace = " \n\t";
  string parse = instring + 1;
  filename = stringtok (&parse, whitespace);

  if ((instring[0]=='C') || (instring[0]=='c'))
    deleted = 1;

  if (sscanf (parse.data(), "%d %d", &subint, &subband) != 2) {
    fprintf (stderr, "toa::Psrclock_load - error parsing '%s'\n", instring);
    return -1;
  }
  // remove the subint and subband
  stringtok (&parse, whitespace);
  stringtok (&parse, whitespace);

  if (parkes_parse (parse.data()) < 0)
    return -1;

  guess_instrument();
  format = Psrclock;
  return 0;
}

int toa::Psrclock_unload (char* outstring) const
{
  if (deleted)
    outstring[0]='C';
  else
    outstring[0]=' ';

  sprintf (outstring+1, "%s %3d %2d", filename.data(), subint, subband);

  outstring [filename.length()+8] = ' ';
  return parkes_out (outstring+filename.length()+10);
}

int toa::Psrclock_unload (FILE* outstream) const
{
  sizebuf (81 + filename.length());
  Psrclock_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// Rhythm_load,unload - load/unload TOA in the Rhythm Format
// 
// ////////////////////////////////////////////////////////////////////////

int toa::Rhythm_load (const char* instring)
{
  if (Parkes_load (instring) < 0)
    return -1;

  // the basic toa line has been parsed.
  
  int scanned = sscanf (instring+1, "%s", datestr);
  if (scanned == 1) {
    struct tm date;
    extern long timezone; // defined in time.h
    if (str2tm (&date, datestr) < 0) {
      fprintf (stderr, "toa::load(char*) Error parsing '%s' as UTC.\n",
	       datestr);
      return -1;
    }
    calculated = mktime (&date);
    calculated -= timezone;
  }

  if (strlen(instring) < 81+14 || instring[80] != 'R')
    return 0;

  scanned = sscanf (instring+81, "%6d%6d%2d", &subint, &subband, &subpoln);
  if (scanned < 3) {
    fprintf (stderr,
	     "toa::Rhythm_load(char*) Error scanning '%s'\n",
	     instring + 81);
    return -1;
  }

  filename = string (instring+81+14);
  filename = stringtok (&filename, " #\n\t");

  guess_instrument();
  format = Rhythm;
  return 0;
}

int toa::Rhythm_unload (char* outstring) const
{
  Parkes_unload (outstring);

  // output the time stamp
  if (calculated == -1)
    time ((time_t*)&calculated);

  int out = (int)strftime (outstring+1, 24, "%Y/%m/%d-%H:%M:%S",
			   gmtime (&calculated));
  // over-write any \0 placed at the end of the string
  if (out < 24)
    *(outstring+1+out) = ' ';

  outstring[80] = 'R';

  sprintf (outstring+81, "%6d%6d%2d %s", 
	   subint, subband, subpoln, filename.data());
  return 0;
}

int toa::Rhythm_unload (FILE* outstream) const
{
  sizebuf (82 + 14 + filename.length() + 1);
  Rhythm_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// generic load,unload - load/unload TOA in appropriate format
//
// these functions will usually be called at the high level
// 
// ////////////////////////////////////////////////////////////////////////

int toa::load (const char* instring)
{
  if (!instring)
    return -1;

  if (instring[80] == 'R')
    return Rhythm_load (instring);

  else if (isdigit(instring[0]))
    return Princeton_load (instring);

  else if (instring[1] == ' ')
    return Parkes_load (instring);

  else
    return Psrclock_load (instring);
}

// returns 0 if toa was loaded, 1 if end of file reached, -1 if error occurs

int toa::load (FILE * instream)
{
  // start with a length of line
  size_t chunk = 160;
  sizebuf (chunk);

  buffer[0] = '\0';

  char*  startbuf = buffer;
  size_t inbuf = 0;

  while (fgets (startbuf, int(bufsz-inbuf), instream) != NULL) {
    size_t rlen = strlen (buffer);
    if ((rlen < bufsz-1) || (buffer[rlen-1] == '\n'))
      break;

    // the buffer was filled but a newline was not encountered
    inbuf = bufsz-1;
    sizebuf (bufsz+chunk);
    startbuf = buffer + inbuf;
  }
  if (ferror (instream)) {
    perror ("toa::load(FILE*) error");
    return -1;
  }
  if (buffer[0] == '\0')
    return 1;

  if (verbose)
    fprintf (stderr, "toa::load(FILE*) to parse '%s'\n", buffer);

  return load (buffer);
}

int toa::unload (FILE* outstream, Format fmt) const
{
  if (fmt == Unspecified)
    fmt = format;

  switch (fmt) {
  case Parkes:
    return Parkes_unload (outstream);
  case Princeton:
    return Princeton_unload (outstream);
  case Psrclock:
    return Psrclock_unload (outstream);
  case Rhythm:
    return Rhythm_unload (outstream);
  default:
    fprintf (stderr, "toa::unload undefined format\n");
    return -1;
  }
}

int toa::unload (char* outstring, Format fmt) const
{
  if (fmt == Unspecified)
    fmt = format;

  switch (fmt) {
  case Parkes:
    return Parkes_unload (outstring);
  case Princeton:
    return Princeton_unload (outstring);
  case Psrclock:
    return Psrclock_unload (outstring);
  case Rhythm:
    return Rhythm_unload (outstring);
  default:
    fprintf (stderr, "toa::unload undefined format\n");
    return -1;
  }
}

// ////////////////////////////////////////////////////////////////////////
//
// Tempo_unload - unload minimal TOA data in appropriate format
// 
// ////////////////////////////////////////////////////////////////////////

int toa::Tempo_unload (FILE* outstream) const
{
  switch (format) {
  case Parkes:
  case Psrclock:
  case Rhythm:
    return Parkes_unload (outstream);
  case Princeton:
    return Princeton_unload (outstream);
  default:
    fprintf (stderr, "Tempo_unload undefined format\n");
    return -1;
  }
}

int toa::Tempo_unload (char* outstring) const
{
  switch (format) {
  case Parkes:
  case Psrclock:
  case Rhythm:
    return Parkes_unload (outstring);
  case Princeton:
    return Princeton_unload (outstring);
  default:
    fprintf (stderr, "Tempo_unload undefined format\n");
    return -1;
  }
}

void toa::guess_instrument ()
{
  if (filename.empty())
    return;

  const char* beg = strrchr (filename.data(), '/');
  if (beg)
    beg ++;
  else
    beg = filename.data();
    
  switch (*beg) {
  case 'c':
  case 'd':
    instrument = FPTM; 
    break;
  case 'f':
  case 'g':
    instrument = FILTERBANK;
    break;
  case 's':
  case 'a':
    instrument = S2;
    break;
  case 'o':
    instrument = CPSR;
    break;
  default:
    instrument = UNKNOWN;
  }
}

// ////////////////////////////////////////////////////////////////////////
//
// toa::shift
//
// shifts a time of arrival to the nearest zero phase
// 
// ////////////////////////////////////////////////////////////////////////

double toa::shift (const polyco& poly) const
{ 
  double toashift = poly.phase (this->arrival, frequency).fracturns();
  if (toashift >.5) toashift -= 1.0;
  return (toashift*poly.period (this->arrival));
}

// ////////////////////////////////////////////////////////////////////////
//
// vector load/unload - load vector of toa objects from file
//
// ////////////////////////////////////////////////////////////////////////

int toa::load (const char* filename, vector<toa>* toas)
{
  FILE* fp;
  if((fp = fopen(filename, "r"))==NULL){
    cerr << "toa_model::load - error opening file " << filename << endl;
    return -1;
  }
  int ret = load (fp, toas);
  fclose (fp);
  return ret;
}

int toa::load (FILE* instream, vector<toa>* toas)
{
  toa tmp_toa;
  while (tmp_toa.load(instream) == 0)
    toas->push_back(tmp_toa);
  return 0;
}

int toa::unload(const char* filename, const vector<toa>& toas, Format fmt)
{
  FILE * fp;
  if((fp = fopen(filename, "w"))==NULL){
    cerr << "toa_model::unload - error opening file " << filename << endl;
    return -1;
  }
  int ret = unload (fp, toas, fmt);
  fclose(fp);
  return ret;
}

int toa::unload(FILE* outstream, const vector<toa>& toas, Format fmt)
{
  for (int i=0; i<toas.size(); ++i) {
    if (verbose)
      cerr << "toa_model::unload - unloading toa " << i << endl;
    if (toas[i].unload(outstream, fmt) < 0)
      return -1;
  }
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// low-level toa stuff
// 
// ////////////////////////////////////////////////////////////////////////

int    toa::verbose = 0;
char*  toa::buffer = NULL;
size_t toa::bufsz = 0;
char   toa::datestr [25];

void toa::init()
{
  format = Unspecified;
  frequency = 0.0;
  arrival = MJD (0.0,0.0,0.0);
  error = 0.0;
  telescope = 0;
  phs = 0.0;
  dmc = 0.0;
  observatory [0] = '\0';

  calculated = -1;
  filename.erase();
  subint = -1;
  subband = -1;
  subpoln = -1;
  instrument = UNKNOWN;

  deleted = 0;
  selected = 1;
}

void toa::destroy()
{
  init ();
}

void toa::sizebuf (size_t length)
{
  if (bufsz < length) {
    void* temp = realloc (buffer, length);
    if (temp == NULL) {
      fprintf (stderr, "toa::sizebuf Cannot allocate buffer space\n");
      throw ("bad_alloc");
    }
    buffer = (char*) temp;
    bufsz = length;
  }
}

bool toa::valid()
{
  if (frequency < 20.0) {
    fprintf (stderr, "toa::load(char*) Error: FREQUENCY:%lf is too small.\n",
 	     frequency);
    return 0;
  }
  if (arrival < MJD (40000,0,0.0)) {
    fprintf (stderr, "toa::load(char*) Error: MJD:%s is too small\n",
 	     arrival.printall());
    return 0;
  }
  if ((error <= 0.0) || (error > 999999999.0)) {
    fprintf (stderr, "toa::load(char*) Error: ERROR:%f is weird.\n", error);
    return 0;
  }

  // passed all tests, return true
  return 1;
}

// ////////////////////////////////////////////////////////////////////////
//
// toa_model - a nice class for handling vectors of toas.
// 
// ////////////////////////////////////////////////////////////////////////

int toa_model::verbose = 0;

toa_model::toa_model (const toa_model & toamodel)
{
  this->operator=(toamodel);
}

toa_model & toa_model::operator = (const toa_model & toamodel)
{
  if (this != &toamodel)
    toas = toamodel.toas;
  return(*this);
}

void toa_model::load (const char * filename)
{
  toas.clear();
  if (toa::load (filename, &toas) < 0)
    throw ("toa_model::load");
}

void toa_model::unload (const char * filename)
{
  if (toa::unload (filename, toas) < 0)
    throw ("toa_model::load");
}
