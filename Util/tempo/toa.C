#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <time.h>

#include "string_utils.h"
#include "polyco.h"
#include "toa.h"

Tempo::toa::toa (char* datastr)
{
  if (load (datastr) < 0) {
    throw ("Tempo::toa::toa (char* datastr) failed");
  }
}

Tempo::toa::toa (Format fmt)
{
  init ();
  if (fmt == Rhythm)
    set_when_calculated (time(NULL));
}

Tempo::toa::toa (const toa & in_toa)
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

int Tempo::toa::Parkes_load (const char* instring)
{
  if (verbose)
    cerr << "Tempo::toa::Parkes_load" << endl;

  destroy ();

  if ((instring[0]=='C') || (instring[0]=='c'))
    state = Deleted;

  return parkes_parse (instring+25);
}

int Tempo::toa::parkes_parse (const char* instring)
{
  int scanned = sscanf (instring, "%lf %s %f %f %d",
 			&frequency, datestr, &phs, &error, &telescope);

  if (state != Deleted && scanned < 5) {
    // an invalid line was not commented out
    if (verbose) cerr << "Tempo::toa::parkes_parse(char*) Error scan:"
		      << scanned << "/5 '" << instring << "'" << endl;
    return -1;
  }
  if (arrival.Construct(datestr) < 0)  {
    if (verbose) cerr << "Tempo::toa::parkes_parse(char*) Error MJD parsing '" 
		      << datestr << "'" << endl;
    return -1;
  }

  if (!valid())
    return -1;

  format = Parkes;
  return 0;
}

int Tempo::toa::Parkes_unload (char* outstring) const
{
  for (int ic=0; ic<25; ic++)
    outstring [ic] = ' ';

  if (state == Deleted)
    outstring[0]='C';

  return parkes_out (outstring+25);
}

int Tempo::toa::parkes_out (char* outstring) const
{
  // output the basic line
  sprintf (datestr, "%8.7lf", frequency);
  sprintf (outstring, "%8.8s   %s   %5.2f %7.2f        %1d ",
 	   datestr, arrival.printdays(13).c_str(), phs, error, telescope);
  return 0;
}

int Tempo::toa::Parkes_unload (FILE* outstream) const
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

int Tempo::toa::Princeton_load (const char* instring)
{
  if (verbose)
    cerr << "Tempo::toa::Princeton_load" << endl;

  destroy ();

  telescope = instring[0] - '0';

  int scanned = sscanf (instring+15, "%lf %s %f %f",
 			&frequency, datestr, &error, &dmc);

  if (scanned < 4) {
    // an invalid line was not commented out
    if (verbose) cerr << "Tempo::toa::Princeton_load(char*) Error scanning '"
		      << instring << "'" << endl;
    return -1;
  }
  if (arrival.Construct(datestr) < 0)  {
    if (verbose) cerr << "Tempo::toa::Princeton_load(char*) Error MJD parsing '"
		      << datestr << "'" << endl;
    return -1;
  }

  if (!valid())
    return -1;

  format = Princeton;
  return 0;
}

int Tempo::toa::Princeton_unload (char* outstring) const
{
  // output the basic line
  outstring[0] = char ('0' + telescope);

  sprintf (outstring+15, "%8.7g %13.13s %4.2f %8.2f",
 	   frequency, arrival.printdays(13).c_str(), error, dmc);
  return 0;
}

int Tempo::toa::Princeton_unload (FILE* outstream) const
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

int Tempo::toa::Psrclock_load (const char* instring)
{
  if (verbose)
    cerr << "Tempo::toa::Psrclock_load ";

  destroy ();

  string whitespace (" \n\t");

  string parse = instring + 1;

  auxinfo = stringtok (&parse, whitespace) + " "
    + stringtok (&parse, whitespace) + " " + stringtok (&parse, whitespace);

  if ((instring[0]=='C') || (instring[0]=='c'))
    state = Deleted;

  if (verbose)
    cerr << "Tempo::toa::Psrclock_load Parkes format = '" << parse << "'" << endl;

  if (parkes_parse (parse.c_str()) < 0)
    return -1;

  // LOAD AUX
  format = Psrclock;

  if (verbose) {
    cerr << "Tempo::toa::Psrclock_loaded ";
    unload (stderr);
  }
  return 0;
}

int Tempo::toa::Psrclock_unload (char* outstring) const
{
  if (state == Deleted)
    outstring[0]='C';
  else
    outstring[0]=' ';

  sprintf (outstring+1, "%s    ", auxinfo.c_str());

  return parkes_out (outstring + 1 + auxinfo.length() + 3);
}

int Tempo::toa::Psrclock_unload (FILE* outstream) const
{
  sizebuf (81 + auxinfo.length());
  Psrclock_unload (buffer);
  fprintf (outstream, "%s\n", buffer);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// Rhythm_load,unload - load/unload TOA in the Rhythm Format
// 
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::Rhythm_load (const char* instring)
{
  if (verbose)
    cerr << "Tempo::toa::Rhythm_load" << endl;

  if (Parkes_load (instring) < 0)
    return -1;

  // the basic toa line has been parsed.
  
  int scanned = sscanf (instring+1, "%s", datestr);
  if (scanned == 1) {
    struct tm date;
    extern long timezone; // defined in time.h
    if (str2tm (&date, datestr) < 0) {
      if (verbose) cerr << "Tempo::toa::load(char*) Error parsing '"
			<< datestr << "' as UTC." << endl;
      return -1;
    }
    calculated = mktime (&date);
    calculated -= timezone;
  }

  if (strlen(instring) < 81+14 || instring[80] != 'R')
    return 0;

  auxinfo = instring + 81;

  // LOAD AUX

  format = Rhythm;
  return 0;
}

int Tempo::toa::Rhythm_unload (char* outstring) const
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

  sprintf (outstring+81, "%s", auxinfo.c_str());
  return 0;
}

int Tempo::toa::Rhythm_unload (FILE* outstream) const
{
  sizebuf (83 + auxinfo.length());
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

int Tempo::toa::load (const char* instring)
{
  if ( !instring )
    return -1;

  if ( instring[80] == 'R' )
    return Rhythm_load( instring );

  else if ( isdigit( instring[0] ) )
    return Princeton_load( instring );

  else if ( instring[17] != ' ' )
    return Psrclock_load( instring );

  else
    return Parkes_load( instring );
}

// returns 0 if toa was loaded, 1 if end of file reached, -1 if error occurs

int Tempo::toa::load (FILE * instream)
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
    perror ("Tempo::toa::load(FILE*) error");
    return -1;
  }
  if (buffer[0] == '\0')
    return 1;

  if (verbose)
    cerr << "Tempo::toa::load(FILE*) to parse '" << buffer << "'" << endl;

  return load (buffer);
}

int Tempo::toa::unload (FILE* outstream, Format fmt) const
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
    if (verbose) cerr << "Tempo::toa::unload undefined format" << endl;
    return -1;
  }
}

int Tempo::toa::unload (char* outstring, Format fmt) const
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
    if (verbose) cerr << "Tempo::toa::unload undefined format" << endl;
    return -1;
  }
}

// ////////////////////////////////////////////////////////////////////////
//
// Tempo_unload - unload minimal TOA data in appropriate format
// 
// ////////////////////////////////////////////////////////////////////////

int Tempo::toa::Tempo_unload (FILE* outstream) const
{
  switch (format) {
  case Parkes:
  case Psrclock:
  case Rhythm:
    return Parkes_unload (outstream);
  case Princeton:
    return Princeton_unload (outstream);
  default:
    if (verbose) cerr << "Tempo::toa::Tempo_unload undefined format" << endl;
    return -1;
  }
}

int Tempo::toa::Tempo_unload (char* outstring) const
{
  switch (format) {
  case Parkes:
  case Psrclock:
  case Rhythm:
    return Parkes_unload (outstring);
  case Princeton:
    return Princeton_unload (outstring);
  default:
    if (verbose) cerr << "Tempo::toa::Tempo_unload undefined format" << endl;
    return -1;
  }
}


// ////////////////////////////////////////////////////////////////////////
//
// Tempo::toa::shift
//
// shifts a time of arrival to the nearest zero phase
// 
// ////////////////////////////////////////////////////////////////////////

double Tempo::toa::shift (const polyco& poly) const
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

int Tempo::toa::load (const char* filename, vector<toa>* toas)
{
  FILE* fp = fopen(filename, "r");
  if (fp==NULL){
    cerr << "\n\nTempo::toa::load - error opening '" << filename << "'::";
    perror ("");
    cerr << endl;
    return -1;
  }
  int ret = load (fp, toas);
  fclose (fp);
  return ret;
}

int Tempo::toa::load (FILE* instream, vector<toa>* toas)
{
  toa tmp_toa;
  while (tmp_toa.load(instream) == 0)
    toas->push_back(tmp_toa);
  return 0;
}

int Tempo::toa::unload(const char* filename, const vector<toa>& toas, Format fmt)
{
  FILE * fp;
  if((fp = fopen(filename, "w"))==NULL){
    cerr << "Tempo::toa::unload - error opening file " << filename << endl;
    return -1;
  }
  int ret = unload (fp, toas, fmt);
  fclose(fp);
  return ret;
}

int Tempo::toa::unload(FILE* outstream, const vector<toa>& toas, Format fmt)
{
  for (unsigned i=0; i<toas.size(); ++i) {
    if (verbose)
      cerr << "Tempo::toa::unload - unloading toa " << i << endl;
    if (toas[i].unload(outstream, fmt) < 0)
      return -1;
  }
  return 0;
}

double Tempo::toa::getData (DataType code) const
{
  switch (code) {
  case Frequency:
    return frequency;
  case Arrival:
    return arrival.in_days();
  case Error:
    return error;
  case Telescope:
    return telescope;
  case PhaseOffset:
    return phs;
  case DMCorrection:
    return dmc;
  default:
    break;
  }

  if (resid.valid) {
    switch (code) {
    case BarycentreArrival:
      return resid.mjd;
    case ResidualPhase:
      return resid.turns;
    case ResidualTime:
      return resid.seconds;
    case BinaryPhase:
      return resid.binaryphase;
    case BarycentreFrequency:
      return resid.obsfreq;
    case Weight:
      return resid.weight;
    case PrefitResidualTime:
      return resid.preres;
    default:
      break;
    }
  }

  if (auxdata && code > PrefitResidualTime)
    return auxdata -> getData (code);
  
  throw FaultCode;
}

const char* Tempo::toa::getDescriptor (DataType code) const
{
  switch (code) {
  case Nothing:
    return "Nothing";
  case Frequency:
    return "Frequency";
  case Arrival:
    return "Arrival";
  case Error:
    return "Error";
  case Telescope:
    return "Telescope";
  case PhaseOffset:
    return "Phase Offset";
  case DMCorrection:
    return "DM Correction";
  case BarycentreArrival:
    return "Barycentric Arrival";
  case ResidualPhase:
    return "Residual Phase";
  case ResidualTime:
    return "Residual Time";
  case BinaryPhase:
    return "Binary Phase";
  case BarycentreFrequency:
    return "Barycentric RF";
  case Weight:
    return "Fit Weight";
  case PrefitResidualTime:
    return "Prefit Residual";
  default:
    break;
  }

  if (auxdata)
    return auxdata -> getDescriptor (code);
  return NULL;
}

// ////////////////////////////////////////////////////////////////////////
//
// low-level toa stuff
// 
// ////////////////////////////////////////////////////////////////////////

bool   Tempo::toa::verbose = false;
char*  Tempo::toa::buffer = NULL;
size_t Tempo::toa::bufsz = 0;
char   Tempo::toa::datestr [25];

void Tempo::toa::init()
{
  format = Unspecified;
  auxdata = NULL;
  resid.valid = false;

  frequency = 0.0;
  arrival = MJD (0.0,0.0,0.0);
  error = 0.0;
  telescope = 0;
  phs = 0.0;
  dmc = 0.0;
  observatory [0] = '\0';

  calculated = -1;
  auxinfo.erase();

  state = Normal;
}

void Tempo::toa::destroy()
{
  if (auxdata) delete auxdata; auxdata = NULL;
  init ();
}

void Tempo::toa::sizebuf (size_t length)
{
  if (bufsz < length) {
    void* temp = realloc (buffer, length);
    if (temp == NULL) {
      cerr << "Tempo::toa::sizebuf Cannot allocate buffer space" << endl;
      throw ("bad_alloc");
    }
    buffer = (char*) temp;
    bufsz = length;
  }
}

bool Tempo::toa::valid()
{
  if (frequency < 20.0) {
    if (verbose) cerr << "Tempo::toa::load(char*) Error: FREQUENCY=" << frequency
		      << " is too small." << endl;
    return 0;
  }
  if (arrival < MJD (40000,0,0.0)) {
    if (verbose) cerr << "Tempo::toa::load(char*) Error: MJD=" << arrival 
		      << " is too small" << endl;
    return 0;
  }
  if ((error <= 0.0) || (error > 999999999.0)) {
    if (verbose) cerr << "Tempo::toa::load(char*) Error: ERROR=" << error 
		      << " is weird." << endl;
    return 0;
  }

  // passed all tests, return true
  return 1;
}

