#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

#include "string_utils.h"
#include "poly.h"

string polyco::any_psr; // the empty string!!

void polynomial::init(){
  dm = 0;
  doppler_shift = 0;
  log_rms_resid = 0;
  f0 = 0;
  telescope = 0;
  nspan_mins = 0;
  freq = 0;
  binph = 0;
  binfreq = 0;
  binary = 0;
  tempov11 = 0;
}

polynomial::polynomial(){
  this->init();
}

polynomial::polynomial(const polynomial & in_poly){
  
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
}

polynomial & polynomial::operator = (const polynomial & in_poly) {

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

  psrname = stringtok (&line, whitespace);
  if (psrname.length() < 1)
    return -1;

  date = stringtok (&line, whitespace);
  if (date.length() < 1)
    return -1;

  utc = stringtok (&line, whitespace);
  if (utc.length() < 1)
    return -1;

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
#ifdef sun
  sscanf(refphstr.c_str(), "%lld %lf\n", &turns, &fracturns);
#else
  sscanf(refphstr.c_str(), "%ld %lf\n", &turns, &fracturns);
#endif
  if(turns>0) ref_phase = Phase(turns, fracturns);
  else ref_phase = Phase(turns, -fracturns);

  int ncoeftmp=0;
  scanned = sscanf (line.c_str(), "%lf %d %lf %d %lf %lf %lf\n",
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
  for (int i = 0;i<ncoeftmp;i++){
    line = stringtok (instr, whitespace);
    size_t letterd = line.find('D');
    if (letterd == line.npos)  {
      fprintf (stderr, "polynomial::load(string*) no 'D' found in '%s'\n",
	line.c_str());
      return -1;
    }
    line[letterd] = 'e';
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

  if(tempov11)
    bytes += sprintf(numstr, "%-10.9s%9.9s%12.12s%22s%19f%7.3lf%7.3lf\n",
          psrname.c_str(), date.c_str(), utc.c_str(), reftime.strtempo(),
          dm, doppler_shift, log_rms_resid);
  else
    bytes += sprintf(numstr, "%-10.9s%9.9s%12.12s%22s%19f\n",
          psrname.c_str(), date.c_str(), utc.c_str(), reftime.strtempo(),dm); 

  *outstr += numstr;
  
  if(binary)
    bytes += sprintf(numstr, "%20s%18.12lf%5d%5.0lf%5d%10.3f%7.4f%9.4f\n", 
	    ref_phase.strprint(6).c_str(), f0, telescope, nspan_mins, 
	    coefs.size(), freq, binph, binfreq);
  else 
    bytes += sprintf(numstr, "%20s%18.12lf%5d%5.0lf%5d%10.3f\n", 
	    ref_phase.strprint(6).c_str(), f0, telescope, nspan_mins, 
	    coefs.size(), freq);

  *outstr += numstr;

  int nrows = (int)(coefs.size()/3);
  if(nrows*3 < coefs.size()) nrows++;

  char* newline = "\n";
  for(int i=0; i<nrows; ++i){
    for(int j=0; j<3 && (i*3+j)<coefs.size(); ++j){
      double ord = coefs[i*3+j];
      int exp = 0;
      while(fabs(ord)<.1){
	ord *= 10.0; exp--;
      }
      while(fabs(ord)>1){
	ord /= 10.0; exp++;
      }
      bytes += sprintf(numstr, "%21.17lfD%+03d", ord, exp);
      *outstr += numstr;
    }
    *outstr += newline;
    bytes += 1;
  }
  return bytes;
}

int polynomial::unload(ostream &ostr) const
{
  string out;
  if (unload(&out) < 0)
    return -1;

  long int start_pos = ostr.tellp();
  ostr << out;

  if(!ostr.good()){
    fprintf(stderr, "polynomial::unload error: bad stream state detected\n");
    ostr.seekp(start_pos);
    ostr.flush();
    return(-1);
  }
  return(ostr.tellp() - start_pos);
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

Phase polynomial::phase(const MJD& t) const { 

   Phase p = Phase(0,0.0);
   MJD dt = t - reftime;
   double tm = dt.in_minutes();

   double poweroft = 1.0;
   for (int i=0;i<coefs.size();i++) {
      p += (coefs[i]*poweroft);
      poweroft *= tm;
   }
   p += tm*f0*60.0;
   return(ref_phase+p);
}

double polynomial::frequency(const MJD& t) const {

   double dp = 0;                    // dphase/dt starts as phase per minute.
   MJD dt = t - reftime;
   double tm = dt.in_minutes();

   double poweroft = 1.0;
   for (int i=1;i<coefs.size();i++) {
     dp+=(double)(i)*coefs[i]*poweroft;
     poweroft *= tm;
   }
   dp /= (double) 60.0;          // Phase per second
   dp += f0;
   return(dp);
}  

Phase polynomial::phase(const MJD& t, float obs_freq) const {
  float dm_delay_in_secs = dm/2.41e-4*(1.0/(obs_freq*obs_freq)-1.0/(freq*freq));
  return(this->phase(t) - dm_delay_in_secs/this->period(t));
}

double polynomial::period(const MJD& t) const {
   return(1.0/frequency(t));               // Seconds per turn = period of pulsar
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
  for(int i=0; i<coefs.size(); ++i) 
    cout << "\tCoeff  " << i+1 << "\t\t" << coefs[i] << endl;
}

/******************************************/

polyco::polyco(){
  pollys.clear();
}

polyco::polyco(const polyco & in_poly){
  pollys = in_poly.pollys;
}

polyco & polyco::operator = (const polyco & in_poly){
  if(this == &in_poly)
    return(*this);
  pollys = in_poly.pollys;
  return(*this);
} 

polyco::polyco(const string filename)
{
  if(load (filename) < 1) { 
    fprintf (stderr, "polyco::polyco - failed to construct from %s\n", filename.c_str());
    string error = "polyco construct error";
    throw(error);
  }
}
 
polyco::polyco(const char * filename)
{
  string s = filename;
  if (load (s) < 1) {
    fprintf (stderr, "polyco::polyco - failed to construct from %s\n", filename);
    string error = "polyco construct error";
    throw(error);
  }
}

int polyco::load(const string polyco_filename, size_t nbytes){
  ifstream file(polyco_filename.c_str());
  return(this->load(file,nbytes));
}

int polyco::load(const char * polyco_filename, size_t nbytes)
{
  ifstream file(polyco_filename);
  return(this->load(file,nbytes));
}

int polyco::load (istream &istr, size_t nbytes)
{
  string total;
  string line;
  size_t bytes = 0;

  char* newline = "\n";
  size_t start_pos = (int) istr.tellg();
  while (!istr.eof())  {
    getline (istr, line, newline[0]);
    if (line.length())  {
      line += newline; // put back the NEWLINE (useful delimiter)
      total += line;
      bytes += line.length();
    }
    if (nbytes && bytes>=nbytes)
      break;
  }
  if (nbytes) istr.seekg (start_pos+nbytes);
  return load (&total);
}

int polyco::load (FILE* fptr, size_t nbytes)
{
  string total;
  if (stringload (&total, fptr, nbytes) < 0)  {
    fprintf (stderr, "polyco::load error\n");
    return -1;
  }
  return load (&total);
}

int polyco::load (string* instr)
{
  int npollys = 0;
  pollys.clear();

  polynomial tst;
  while(instr->length() && tst.load(instr)==0){
    pollys.push_back(tst);      
    npollys++;
  }
  return(npollys);
}

int polyco::unload(const string filename) const {
  return(this->unload(filename.c_str()));
}

int polyco::unload(const char *filename) const {
  ofstream ostr(filename);
  return(this->unload(ostr));
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
  for (int i=0; i<pollys.size(); ++i) {
    bytes += pollys[i].unload(outstr);
  }
  return bytes;
}

int polyco::unload(ostream &ostr) const
{
  string outline;
  if (unload(&outline) < 0)
    return -1;

  long int start_pos = ostr.tellp();
  ostr << outline;

  if(!ostr.good()){
    fprintf(stderr, "polyco::unload error: bad stream state detected\n");
    ostr.seekp(start_pos);
    ostr.flush();
    return(-1);
  }
  return(ostr.tellp() - start_pos);
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

// int polyco::print(char * chpolly) const{
//   // Terrible terrible terrible
//   // use stringstreams
//   this->unload("polyco.dat.tmp");
//   FILE * fp;
//   if((fp=fopen("polyco.dat.tmp", "r"))==NULL){
//     fprintf(stderr, "polyco::print error - could not open file\n");
//     return(-1);
//   }
//   if(fread(chpolly, this->size_in_bytes(), 1, fp)!=1){
//     fprintf(stderr, "polyco::print error reading file\n");
//     return(-1);
//   }
//   fclose(fp);
//   remove("polyco.dat.tmp");
//   return(0);
// } 

void polyco::prettyprint() const {
  for(int i=0; i<pollys.size(); ++i) 
    pollys[i].prettyprint();
}

// returns a pointer to the best polynomial for use over the period
// defined by t1 to t2

const polynomial* polyco::nearest (const MJD &t, const string& in_psrname) const
{
  int ipolly = i_nearest (t, in_psrname);

  if (ipolly < 0)  {
    string failed = "polyco::nearest_polly no polynomial";
    throw(failed);
  }
  return &pollys[ipolly];
}

polynomial polyco::nearest_polly (const MJD &t, const string& in_psrname) const
{
  int ipolly = i_nearest (t, in_psrname);

  if (ipolly < 0)  {
    string failed = "polyco::nearest_polly no polynomial";
    throw(failed);
  }
  return pollys[ipolly];
}

int polyco::i_nearest (const MJD &t, const string& in_psr) const
{
  for (int ipolly=0; ipolly<pollys.size(); ipolly ++)  {
    MJD t1=pollys[ipolly].reftime - (double) pollys[ipolly].nspan_mins*60.0/2.0;
    MJD t2=pollys[ipolly].reftime + (double) pollys[ipolly].nspan_mins*60.0/2.0;
    
    if (t>=t1 && t<=t2 && 
	(in_psr==any_psr || pollys[ipolly].psrname==in_psr)) {
      return ipolly;
    }
  }
  fprintf (stderr, "polyco::i_nearest - no polynomial for MJD %s\n", 
	t.strtempo());
  return -1;
}

Phase polyco::phase(const MJD& t, const string& in_psrname) const {
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(t, in_psrname);}
  catch(...) {
    fprintf (stderr, "polyco::phase no polynomial for PSR:'%s'  MJD:'%s'\n",
	in_psrname.data(), t.printdays());
    throw("no polynomial");
  }
  return(nearest_polly.phase(t));
}

Phase polyco::phase(const MJD& t, float obs_freq, const string& in_psrname) const {
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(t,in_psrname);}
  catch(...) {
    cerr << "polyco::phase - error finding polynomial\n";
    throw("no polynomial");}
  return(nearest_polly.phase(t, obs_freq));
} 

double polyco::period(const MJD& t, const string& in_psrname) const{
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(t,in_psrname);}
  catch(...) {
    cerr << "polyco::period - error finding polynomial\n";
    throw("no polynomial");}
  return(nearest_polly.period(t));
}

double polyco::frequency(const MJD& t, const string& in_psrname) const {
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(t,in_psrname);}
  catch(...) {
    cerr << "polyco::frequency - error finding polynomial\n";
    throw("no polynomial");}  
  return(nearest_polly.frequency(t));
}

