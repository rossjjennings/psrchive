#include <iostream>

#include <stdio.h> 
#include <stdlib.h>
#include <values.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

#include "string_utils.h"
#include "poly.h"

string polyco::anyPsr;
bool   polyco::verbose = false;
bool   polyco::debug = false;

// not going to get > ns precision out of a routine based in minutes
double polyco::precision = 1e-10;

void polynomial::init() {
  dm = 0;
  doppler_shift = 0;
  log_rms_resid = 0;
  f0 = 0;
  telescope = '\0';
  nspan_mins = 0.0;
  freq = 0;
  binph = 0;
  binfreq = 0;
  binary = 0;
  tempov11 = 0;
}


polynomial & polynomial::operator = (const polynomial & in_poly)
{
  if(this == &in_poly) 
    return(*this);

  psrname = in_poly.psrname;
  date = in_poly.date;
  utc = in_poly.utc;
  reftime = in_poly.reftime;
  dm = in_poly.dm;
  doppler_shift = in_poly.doppler_shift;
  log_rms_resid = in_poly.log_rms_resid;
  ref_phase = in_poly.ref_phase;
  f0 = in_poly.f0;
  telescope = in_poly.telescope;
  nspan_mins = in_poly.nspan_mins;
  freq = in_poly.freq;
  binph = in_poly.binph;
  binfreq = in_poly.binfreq;
  coefs = in_poly.coefs; 
  binary = in_poly.binary;
  tempov11 = in_poly.tempov11;

  return(*this);
}  

/* ************************************************************************
   polynomial::load

   Loads from a string formatted as described below (taken from TEMPO
   reference manual:

   POLYCO.DAT
   
   The polynomial ephemerides are written to file 'polyco.dat'.  Entries
   are listed sequentially within the file.  The file format is:
   
   Line  Columns     Item
   ----  -------   -----------------------------------
     1      1-10   Pulsar Name
           11-19   Date (dd-mmm-yy)
	   20-31   UTC (hhmmss.ss)
	   32-51   TMID (MJD)
	   52-72   DM
	   74-79   Doppler shift due to earth motion (10^-4)
	   80-86   Log_10 of fit rms residual in periods
     2      1-20   Reference Phase (RPHASE)
           21-38   Reference rotation frequency (F0)
	   39-43   Observatory number 
	   44-49   Data span (minutes)
	   50-54   Number of coefficients
	   55-75   Observing frequency (MHz)
	   76-80   Binary phase
     3*     1-25   Coefficient 1 (COEFF(1))
           26-50   Coefficient 2 (COEFF(2))
	   51-75   Coefficient 3 (COEFF(3))
   
   * Subsequent lines have three coefficients each, up to NCOEFF
   ************************************************************************ */

int polynomial::load(string* instr)
{
  this->init();    

  if (instr->length() < 160)
    return -1;

  string whitespace (" \t\n");
  string line;

  line = stringtok (instr, "\n");
  if (line.length() < 1)
    return -1;

  // rte: this doesn't work for pulsars with letter suffices
  //  psrname = stringtok (&line, whitespace);
  // if (psrname.length() < 1)
  //    return -1;

  // wvs: substr(0,9) didn't work on Sun :(
  psrname = line.substr(0,10);
  psrname = stringtok(&psrname, whitespace);
  line.erase(0,10);

  date = stringtok (&line, whitespace);
  if (date.length() < 1)
    return -1;

  utc = stringtok (&line, whitespace);
  if (utc.length() < 1)
    return -1;

  if (polyco::verbose)
    cerr << "polynomial::load scanned name='"<<psrname<<"' date="<<date
         << " UTC:"<<utc<<endl;

  int mjd_day_num;
  double frac_mjd;
  int scanned = sscanf (line.c_str(), "%d %lf %f %lf %lf\n",
		&mjd_day_num, &frac_mjd, &dm, &doppler_shift, &log_rms_resid);
  if (scanned < 3)  {
    fprintf (stderr, "polynomial::load(string*) error stage 1 parsing '%s'",
		line.c_str());
    if (scanned < 1) {
      perror ("::");
    }
    else {
      fprintf (stderr, "\npolynomial::load(string*) scanned %d/5 values\n",
	       scanned);
    }
    return -1;
  }
  if (polyco::verbose)
    cerr << "polynomial::load scanned mjd=" << mjd_day_num 
	 << "+" << frac_mjd << " dm=" << dm << " dop=" << doppler_shift
	 << " rms=" << log_rms_resid << endl;

  reftime = MJD(mjd_day_num, frac_mjd);

  if (scanned < 5)
    tempov11 = 0;
  else
    tempov11 = 1;

  line = stringtok (instr, "\n");
  if (line.length() < 1)
    return -1;

  string refphstr = stringtok (&line, whitespace);
  if (refphstr.length() < 1)
    return -1;

  int64 turns;
  double fracturns;

  sscanf (refphstr.c_str(), I64" %lf\n", &turns, &fracturns);
  if (polyco::verbose)
    cerr << "polynomial::load scanned phase=" << turns
	 << "+" << fracturns << endl;

  if(turns>0) ref_phase = Phase(turns, fracturns);
  else ref_phase = Phase(turns, -fracturns);

  if (polyco::verbose)
    cerr << "polynomial::load Phase=" << ref_phase << endl;

  int ncoeftmp=0;
  scanned = sscanf (line.c_str(), "%lf %c %lf %d %lf %lf %lf\n",
  	    &f0, &telescope, &nspan_mins, &ncoeftmp, &freq, &binph, &binfreq);
  if (scanned < 5) {
    fprintf (stderr, "polynomial::load(string*) error stage 2 parsing '%s'\n",
                line.c_str());
    return -1;
  }
  if (scanned < 7)
    binary = 0;
  else
    binary = 1;

  coefs.clear();
  coefs.resize(ncoeftmp);  
  // Read in the coefficients 
  int i;
  for (i = 0;i<ncoeftmp;i++){
    line = stringtok (instr, whitespace);
    size_t letterd = line.find('D');
    if (letterd != string::npos) line[letterd] = 'e';
    if (sscanf (line.c_str(), "%lf", &coefs[i]) != 1)  {
      fprintf (stderr, "polynomial::load(string*) did not parse '%s'\n",
        line.c_str());
      return -1;
    }
  }
  size_t endline = instr->find('\n');
  if (endline)  {
    instr->erase(0, endline);
  }

  if (polyco::verbose)
    cerr << "polynomial::load parsed " << i << " coefficients" << endl;
  return 0;
}

// ///////////////////////////////////////////////////////////////////
// polynomial::unload (string*)
//
// Adds characters to the string to which outstr points.  
// The text added is the tempo formatted text of this polynomial.
// Return value: the number of characters added (not including the \0)
// ///////////////////////////////////////////////////////////////////
int polynomial::unload (string* outstr) const
{
  char numstr[100];  // max length of string set by princeton at 86...
  int bytes = 0;

  if (tempov11)  {
    if (polyco::verbose)
      cerr << "polynomial::unload tempo11" << endl;

    bytes += sprintf(numstr, "%-10.10s %9.9s%12.12s%22s%19f%7.3lf%7.3lf\n",
          psrname.c_str(), date.c_str(), utc.c_str(), reftime.strtempo(),
          dm, doppler_shift, log_rms_resid);
  }
  else  {
    if (polyco::verbose)
      cerr << "polynomial::unload not tempo11" << endl;
    bytes += sprintf(numstr, "%-10.9s%9.9s%12.12s%22s%19f\n",
          psrname.c_str(), date.c_str(), utc.c_str(), reftime.strtempo(),dm); 
  }

  *outstr += numstr;
  
  if(binary)  {

    if (polyco::verbose)
      cerr << "polynomial::unload binary" << endl;

    bytes += sprintf(numstr, "%20s%18.12lf    %c%5.0lf%5d%10.3f%7.4f%9.4f\n", 
	    ref_phase.strprint(6).c_str(), f0, telescope, nspan_mins, 
	    coefs.size(), freq, binph, binfreq);
  }
  else 
    bytes += sprintf(numstr, "%20s%18.12lf    %c%5.0lf%5d%10.3f\n", 
	    ref_phase.strprint(6).c_str(), f0, telescope, nspan_mins, 
	    coefs.size(), freq);

  *outstr += numstr;

  unsigned nrows = (int)(coefs.size()/3);
  if(nrows*3 < coefs.size()) nrows++;

  if (polyco::verbose)
    cerr << "polynomial::unload nrows=" << nrows << endl;

  char* newline = "\n";

  for (unsigned i=0; i<nrows; ++i){

    if (polyco::verbose)
      cerr << "polynomial::unload row=" << i << endl;

    for (unsigned j=0; j<3 && (i*3+j)<coefs.size(); ++j) {

      bytes += sprintf (numstr, " %+22.17le", coefs[i*3+j]);
      char* ep = strchr (numstr, 'e');
      if (ep)
        *ep = 'D';

      *outstr += numstr;
    }
    *outstr += newline;
    bytes += 1;
  }
  return bytes;
}

int polynomial::unload (FILE* fptr) const
{
  string out;
  if (unload(&out) < 0)
    return -1;

  int size = (int) out.length();
  int bout = fprintf (fptr, out.c_str());
  if (bout < size)  {
    fprintf (stderr, "polynomial::unload(FILE*) ERROR fprintf only %d/%d",
	bout, size);
    perror ("");
    return -1;
  }
  fflush (fptr);
  return bout;
}

Phase polynomial::phase(const MJD& t) const
{
  Phase dp (0.0);
  MJD dt = t - reftime;
  double tm = dt.in_minutes();

  double poweroft = 1.0;
  for (unsigned i=0;i<coefs.size();i++) {
    dp += (coefs[i]*poweroft);
    poweroft *= tm;
  }
  dp += tm*f0*60.0;

  return ref_phase + dp;
}

// /////////////////////////////////////////////////////////////////////////
// polynomial::iphase
//
// uses the Newton-Raphson method to very quickly solve:
// PHASE = RPHASE + DT*60*F0 + COEFF(1) + DT*COEFF(2) + DT^2*COEFF(3) + ....
// for DT, given PHASE.  Where:
//
// DT    = time - reftime;
// PHASE = p;
// 
// RETURN value is time
//
MJD polynomial::iphase(const Phase& p) const
{
  MJD guess = reftime + (p - ref_phase) / f0;
  MJD dt;

  int gi = 0;
  double converge_faster = 1.0;  // kludge!!
  double converge_factor = 0.5;

  double lprecision = max (polyco::precision, MJD::precision);

  for (gi=0; gi<10000; gi++) {
    dt = (phase(guess) - p) / frequency(guess);

    guess -= dt * converge_faster;

    // every six iterations, give the convergence a little bump
    if (gi && !(gi % 6))
      converge_faster *= converge_factor;

    if (fabs (dt.in_seconds()) < lprecision)
      return guess;
  }

  cerr << "polynomial::iphase maximum iterations exceeded - error="
       << dt.in_seconds() * 1e6 << "us" << endl;
  
  return guess;
}

double polynomial::frequency(const MJD& t) const
{  
  double dp = 0;                    // dphase/dt starts as phase per minute.
  MJD dt = t - reftime;
  double tm = dt.in_minutes();

  double poweroft = 1.0;
  for (unsigned i=1; i<coefs.size(); i++) {
    dp+=(double)(i)*coefs[i]*poweroft;
    poweroft *= tm;
  }
  dp /= (double) 60.0;          // Phase per second
  dp += f0;
  return dp;
}  

double polynomial::chirp(const MJD& t) const
{
  double d2p = 0;                    // d^2phase/dt^2 starts as phase/minute^2
  MJD dt = t - reftime;
  double tm = dt.in_minutes();

  double poweroft = 1.0;
  for (unsigned i=2; i<coefs.size(); i++) {
    d2p+=(double)(i)*(i-1)*coefs[i]*poweroft;
    poweroft *= tm;
  }
  d2p /= (double) 60.0*60.0;          // Phase per second^2

  return d2p;
}  



Phase polynomial::phase(const MJD& t, float obs_freq) const
{
  float dm_delay_in_secs = 
    dm/2.41e-4*(1.0/(obs_freq*obs_freq)-1.0/(freq*freq));
  return phase(t) - dm_delay_in_secs/period(t);
}

// Seconds per turn = period of pulsar
double polynomial::period (const MJD& t) const {
   return(1.0/frequency(t));
}

void polynomial::prettyprint() const {

  cout << "PSR Name\t\t\t" << psrname << endl;
  cout << "Date\t\t\t\t" << date << endl;
  cout << "UTC\t\t\t\t" << utc << endl;
  cout << "Ref. Time\t\t\t" << reftime.strtempo() << endl;
  cout << "DM\t\t\t\t" << dm << endl;
  if(tempov11){
    cout << "Doppler Shift\t\t\t" << doppler_shift << endl;
    cout << "Log RMS Residuals\t\t" << log_rms_resid << endl;
  }
  cout << "Ref. Phase\t\t\t" << ref_phase.intturns() << ref_phase.fracturns() << endl;
  cout << "Ref. Rotation Frequency\t\t" << f0 << endl;
  cout << "Telescope\t\t\t" << telescope << endl;
  cout << "Span in Minutes\t\t\t" << nspan_mins << endl;
  cout << "Number of Coefficients\t\t" << coefs.size() << endl;
  cout << "Reference Frequency\t\t" << freq << endl;
  if(binary){
    cout << "Binary Phase\t\t\t" << binph << endl;
    cout << "Binary Frequency\t\t" << binfreq << endl;
  }
  for(unsigned i=0; i<coefs.size(); ++i) 
    cout << "\tCoeff  " << i+1 << "\t\t" << coefs[i] << endl;
}

int operator == (const polynomial & p1, const polynomial & p2){
  if(p1.dm != p2.dm ||
     p1.doppler_shift != p2.doppler_shift ||
     p1.log_rms_resid != p2.log_rms_resid ||
     p1.f0 != p2.f0 ||
     p1.telescope != p2.telescope ||
     p1.nspan_mins != p2.nspan_mins ||
     p1.freq != p2.freq ||
     p1.binph != p2.binph ||
     p1.binfreq != p2.binfreq ||
     p1.binary != p2.binary ||
     p1.tempov11 != p2.tempov11) return(0);
  return(1);
}

int operator != (const polynomial & p1, const polynomial & p2){
  if(p1==p2) return(0);
  return(1);
}


/******************************************/

polyco & polyco::operator = (const polyco & in_poly)
{
  if (this == &in_poly)
    return *this;

  pollys = in_poly.pollys;

  return *this;
} 

polyco::polyco (const string filename)
{
  if (load (filename) < 1) { 
    fprintf (stderr, "polyco::polyco - failed to construct from %s\n", filename.c_str());
    string error = "polyco construct error";
    throw(error);
  }
}
 
polyco::polyco (const char * filename)
{
  if (load (filename) < 1) {
    fprintf (stderr, "polyco::polyco - failed to construct from %s\n", filename);
    string error = "polyco construct error";
    throw(error);
  }
}

int polyco::load (const char* polyco_filename, size_t nbytes)
{
  if (verbose)
    cerr << "polyco::load '" << polyco_filename << "'" << endl;

  FILE* fptr = fopen (polyco_filename, "r");
  if (!fptr)  {
    cerr << "polyco::load cannot open '" << polyco_filename << "' - "
	<< strerror (errno) << endl;
    return -1;
  }

  int ret = load (fptr, nbytes);
  fclose (fptr);

  return ret;
}

int polyco::load (FILE* fptr, size_t nbytes)
{
  string total;
  if (stringload (&total, fptr, nbytes) < 0)  {
    fprintf (stderr, "polyco::load stringload error\n");
    return -1;
  }
  return load (&total);
}

int polyco::load (string* instr)
{
  if (verbose)
    cerr << "polyco::load '\n" << *instr << "\n'" << endl;

  int npollys = 0;
  pollys.clear();

  polynomial tst;
  while(instr->length() && tst.load(instr)==0){
    pollys.push_back(tst);      
    npollys++;
  }
  return(npollys);
}

int polyco::unload (const char *filename) const
{
  FILE* fptr = fopen (filename, "w");
  if (!fptr)  {
    cerr << "polyco::unload cannot open '" << filename << "' - "
        << strerror (errno) << endl;
    return -1;
  }
  return unload (fptr);
}

// ///////////////////////////////////////////////////////////////////
// polyco::unload (string*)
//
// Adds characters to the string to which outstr points.  
// The text added is the tempo formatted text of this polynomial.  
// Return value: the number of characters added (not including the \0)
// ///////////////////////////////////////////////////////////////////
int polyco::unload (string* outstr) const {
  int bytes = 0;

  if (verbose)
    cerr << "polyco::unload " << pollys.size() << " polynomials" << endl;

  for (unsigned i=0; i<pollys.size(); ++i) {
    if (verbose)
      cerr << "polyco::unload " << i << endl;
    bytes += pollys[i].unload(outstr);
  }
  return bytes;
}

int polyco::unload (FILE* fptr) const
{
  string out;
  if (unload(&out) < 0)
    return -1;

  int size = (int) out.length();
  int bout = fprintf (fptr, out.c_str());
  if (bout < size)  {
    fprintf (stderr, "polyco::unload(FILE*) ERROR fprintf only %d/%d",
        bout, size);
    perror ("");
    return -1;
  }
  fflush (fptr);
  return bout;
}

void polyco::append (const polyco& poly)
{
  for (unsigned i=0; i<poly.pollys.size(); ++i) 
    pollys.push_back (poly.pollys[i]);
}


void polyco::prettyprint() const 
{
  for(unsigned i=0; i<pollys.size(); ++i) 
    pollys[i].prettyprint();
}

// returns a pointer to the best polynomial for use over the period
// defined by t1 to t2
const polynomial* polyco::nearest (const MJD &t, const string& psr) const
{
  int ipolly = i_nearest (t, psr);

  if (ipolly < 0)  {
    string failed = "polyco::nearest_polly no polynomial";
    throw(failed);
  }
  return &pollys[ipolly];
}

const polynomial& polyco::best (const MJD &t, const string& psr) const
{
  int ipolly = i_nearest (t, psr);

  if (ipolly < 0)  {
    string failed = "polyco::best no polynomial";
    throw(failed);
  }
  return pollys[ipolly];
}

const polynomial& polyco::best (const Phase& p, const string& psr) const
{
  int ipolly = i_nearest (p, psr);

  if (ipolly < 0)  {
    string failed = "polyco::best no polynomial";
    throw(failed);
  }
  return pollys[ipolly];
}

int polyco::i_nearest (const MJD &t, const string& in_psr) const
{
  float min_dist = MAXFLOAT;
  int imin = -1;

  for (unsigned ipolly=0; ipolly<pollys.size(); ipolly ++)  {
    if (in_psr==anyPsr || pollys[ipolly].psrname==in_psr) {      
      float dist = fabs ( (pollys[ipolly].reftime - t).in_minutes() );
      if (dist < min_dist) {
	imin = ipolly;
	min_dist = dist;
      }
    }
  }
  // check if any polynomial matched
  if (imin < 0) {
    cerr << "polyco::i_nearest - no polynomial found for pulsar: '"
	 << in_psr << "'\n";
    return -1;
  }

#ifdef IDEAL_WORLD
  // return if the time is within the range of the matched polynomial
  if ( (t > pollys[imin].start_time()) && (t < pollys[imin].end_time()) )
    return imin;
#else
  // TEMPO sometimes leaves holes between its polynomials.
  // Let's just be happy if it is within the range of the polyco
  if ( (t > start_time()) && (t < end_time()) )
    return imin;
#endif

  // the time is out of range of the nearest polynomial
  cerr << "polyco::i_nearest - no polynomial for MJD " << t.printdays(15)
       << "\npolyco::i_nearest - range " << start_time().printdays(5) 
       << " - " << end_time().printdays(5) << endl;
  return -1;
}

int polyco::i_nearest (const Phase& phase, const string& in_psr) const
{
  float min_dist = MAXFLOAT;
  int imin = -1;

  for (unsigned ipolly=0; ipolly<pollys.size(); ipolly ++)  {
    if (in_psr==anyPsr || pollys[ipolly].psrname==in_psr) {      
      float dist = fabs ( (pollys[ipolly].ref_phase - phase).in_turns() );
      if (dist < min_dist) {
	imin = ipolly;
	min_dist = dist;
      }
    }
  }
  // check if any polynomial matched
  if (imin < 0) {
    cerr << "polyco::i_nearest - no polynomial found for pulsar: '"
	 << in_psr << "'\n";
    return -1;
  }

  if ( (phase > start_phase()) && (phase < end_phase()) )
    return imin;

  // the time is out of range of the nearest polynomial
  cerr << "polyco::i_nearest - no polynomial for MJD " << phase.strprint(15)
       << "\npolyco::i_nearest - range " << start_phase().strprint(15)
       << " - " << end_phase().strprint(15) << endl;

  return -1;
}

bool polyco::is_tempov11() const {
  for(unsigned i=0; i<pollys.size(); ++i)
    if(!pollys[i].is_tempov11()) return(pollys[i].is_tempov11());
  return(pollys[0].is_tempov11());
}

int operator == (const polyco & p1, const polyco & p2){
  if(p1.pollys.size()!=p2.pollys.size()) return(0);
  for(unsigned i=0; i<p1.pollys.size(); ++i)
    if(p1.pollys[i]!=p2.pollys[i]) return(0);
  return(1);
}

int operator != (const polyco & p1, const polyco & p2){
  if(p1==p2) return(0);
  return(1);
}











