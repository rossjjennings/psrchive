#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include "stdrio.h"
#include "poly.h"

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

int polynomial::load(istream &istr){
  
  int mjd_day_num;
  double frac_mjd;
  int ncoeftmp=0;

  this->init();    

  istr >> psrname;
  if(!istr.good()) return(-1);  // a quick peek to see if we're wasting time
  istr >> date;
  istr >> utc;
  istr >> mjd_day_num;
  istr >> frac_mjd;
  istr >> dm;
  while(istr.peek()==' ') istr.get();
  if(istr.peek() == '\n') tempov11 = 0;
  else {
    istr >> doppler_shift;
    istr >> log_rms_resid;  
    tempov11 = 1;
  }
  string refphstr;
  istr >> refphstr;  
  istr >> f0; 
  istr >> telescope;
  istr >> nspan_mins;
  istr >> ncoeftmp;
  istr >> freq;
  while(istr.peek()==' ') istr.get();
  if(istr.peek() == '\n') binary = 0;
  else {
    istr >> binph;
    istr >> binfreq;
    binary = 1;
  }

  // if input unsuccessful, exit with error 
  // before constructing any dependent objects
  if(!istr.good()) return(-1);  
  reftime = MJD(mjd_day_num, frac_mjd);
  int64 turns;
  double fracturns;
#ifdef sun
  sscanf(refphstr.c_str(), "%lld %lf\n", &turns, &fracturns);
#else
  sscanf(refphstr.c_str(), "%ld %lf\n", &turns, &fracturns);
#endif
  if(turns>0) ref_phase = Phase(turns, fracturns);
  else ref_phase = Phase(turns, -fracturns);

  coefs.clear();
  coefs.resize(ncoeftmp);  
  // Read in the coefficients 
  for (int i = 0;i<ncoeftmp;i++){
    string strexp;
    int exp;
    double ord;
    istr >> ord;
    if(istr.get()!='D') return(-1);
    istr >> strexp;               // Did it this way because of
    exp = atoi(strexp.c_str());   // cryptic solaris bug....
    coefs[i] = ord * pow(10, exp);
  }
  if(!istr.good()) return(-1);

  return(0);
}

// This should be converted to use stringstreams
// when we move to solaris CC 5.0
size_t polynomial::size_in_bytes() const {
return(0);
}

int polynomial::unload(ostream &ostr){

  char numstr[100];  // max length of string set by princeton at 86...

  if(tempov11)
    sprintf(numstr, "%-10.9s%9.9s%12.12s%22s%19f%7.3lf%7.3lf", 
	    psrname.c_str(), date.c_str(), utc.c_str(), reftime.strtempo(),
	    dm, doppler_shift, log_rms_resid);
  else
    sprintf(numstr, "%-10.9s%9.9s%12.12s%22s%19f", 
	    psrname.c_str(), date.c_str(), utc.c_str(), reftime.strtempo(),dm);    
  ostr << numstr << endl;
  
  if(binary)
    sprintf(numstr, "%20s%18.12lf%5d%5.0lf%5d%10.3f%7.4f%9.4f", 
	    ref_phase.strprint(6).c_str(), f0, telescope, nspan_mins, 
	    coefs.size(), freq, binph, binfreq);
  else 
    sprintf(numstr, "%20s%18.12lf%5d%5.0lf%5d%10.3f", 
	    ref_phase.strprint(6).c_str(), f0, telescope, nspan_mins, 
	    coefs.size(), freq);
  ostr << numstr << endl;
  
  int nrows = (int)(coefs.size()/3);
  if(nrows*3 < coefs.size()) nrows++;

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
      sprintf(numstr, "%21.17lfD%+03d", ord, exp);
      ostr << numstr;
    }
    ostr << endl;
  }
  if(!ostr.good()){
    fprintf(stderr, "polynomial::unload error: bad stream state detected\n");
    return(-1);
  }
  return(0);
}

Phase polynomial::phase(const MJD& t) const
{ 
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

Phase polynomial::phase(const MJD& t, float obs_freq) const {
  float dm_delay_in_secs = dm/2.41e-4*(1.0/(obs_freq*obs_freq)-1.0/(freq*freq));
  return(this->phase(t) - dm_delay_in_secs/this->period(t));
}

double polynomial::period(const MJD& t) const {
   return(1.0/frequency(t));               // Seconds per turn = period of pulsar
 }

double polynomial::frequency(const MJD& t) const {
   double dp;                    // dphase/dt starts as phase per minute.
   MJD dt = t - reftime;
   double tm = dt.in_minutes();

   dp = coefs[1];
   double poweroft = 1.0;
   for (int i=2;i<coefs.size();i++) {
     dp+=(double)(i)*coefs[i]*poweroft;
     poweroft *= tm;
   }
   dp /= (double) 60.0;          // Phase per second
   dp += f0;
   return(dp);
}  

void polynomial::prettyprint() {

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

polyco::polyco(string filename)
{
  if(load (filename) != 0) { 
    fprintf (stderr, "polyco::polyco - failed to construct from %s\n", filename.c_str());
    exit(-1);
  }
}
 
polyco::polyco(char * filename)
{
  string s = filename;
  if (load (s) != 0) {
    fprintf (stderr, "polyco::polyco - failed to construct from %s\n", filename);
    exit(-1);
  }
}

// Default arguments are those of polyco
int polyco::Construct (char* psr, char* parfile, const MJD& m1, const MJD& m2, 
	 	       double ns=960.0, int nc=12, int maxha=8, int tel=7, double centrefreq = 1400.0) { 
  char  syscom[120]; 
  string ephfile;
  string polyfile = "polyco.dat";
  int   rmeph = 0;

  if (parfile) {
    sprintf(syscom,"polyco -f %s %lf %lf %lf %d %d %d %lf > /tmp/polyco.stdout",
	    parfile, m1.in_days(), m2.in_days(), ns, nc, maxha, tel, centrefreq);
    
    ephfile = parfile;
  }
  else {
    sprintf(syscom,"polyco %s %lf %lf %lf %d %d %d %lf > /tmp/polyco.stdout",
	    psr, m1.in_days(), m2.in_days(), ns, nc, maxha, tel, centrefreq);

    if (psr[0] == 'J') ephfile = &(psr[1]);
    else ephfile = psr;
    ephfile += ".eph";
    rmeph = 1;
  }

  int retries = 3;
  while (retries > 0) {

    errno = 0;
    int status = system(syscom);
    if (status == 0) break;

    fprintf (stderr, "polyco::polyco - failed: %d\n", retries);
    if (status == -1)
      perror ("polyco::errno");
    else {
      fprintf (stderr, "polyco::status = %d ", status);
      if (status == 127)
	fprintf (stderr, 
		 "(indicating that the shell could not be executed).\n");
      else
	fprintf (stderr,  "(a return value of %d from polyco)\n",
		 WEXITSTATUS(status));
    }
    fprintf (stderr, "polyco::cmdline: %s\n", syscom);
    fprintf (stderr, "polyco::stdout:\n");
    system ("cat /tmp/polyco.stdout");
    retries --;
    
    if (retries == 0) return -1;

    if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
      sleep (5);
    }
  }

  if (this->load(polyfile)<=0) {
    fprintf (stderr, "polyco::polyco - failed to construct from %s and %s\n", 
	     polyfile.c_str(), ephfile.c_str());
  }
  if (rmeph) remove (ephfile.c_str());
  remove ("polyco.dat");
  remove ("dates.tmp");
  remove ("tz.in");
  remove ("tz.tmp");
  remove ("tztot.dat");
  remove ("tempo.lis");
  return 0;
}

polyco::polyco (char* psr, char* parfile, const MJD& m1, const MJD& m2,
		double ns, int nc, int maxha, int tel, double centrefreq)
{
  string errstr;
  if (Construct (psr, parfile, m1, m2, ns, nc, maxha, tel, centrefreq) != 0) {
    errstr = "polyco::polyco - failed to construct\n";
    throw (errstr);
  }
}

polyco::polyco (char* psr, const MJD& m1, const MJD& m2,
		double ns, int nc, int maxha, int tel, double centrefreq)
{
  string errstr;
  if (Construct (psr, NULL, m1, m2, ns, nc, maxha, tel, centrefreq) != 0) {
    errstr = "polyco::polyco - failed to construct\n";
    throw (errstr);
  }
}

int polyco::load(string polyco_filename){
  ifstream file(polyco_filename.c_str());
  return(this->load(file));
}

int polyco::load(char * polyco_filename)
{
  ifstream file(polyco_filename);
  return(this->load(file));
}

int polyco::load(istream &istr){
  int npollys = 0;
  polynomial tst;
  pollys.clear();
  while(tst.load(istr)==0){
    pollys.push_back(tst);
    npollys++;
  }
  return(npollys);
}

int polyco::load(FILE *fp){
  ifstream file(FD(fp));
  return(this->load(file));
}

int polyco::unload(string filename){
  return(this->unload(filename.c_str()));
}

int polyco::unload(char *filename){
  ofstream ostr(filename);
  return(this->unload(ostr));
}

int polyco::unload(ostream &ostr){
  for(int i=0; i<pollys.size(); ++i){
    if(pollys[i].unload(ostr)!=0){
      fprintf(stderr, "polyco::unload error - couldn't unload polynomial %d\n", i);
      return(-1);
    }
  }
  return(0);
}

int polyco::unload(FILE *fp){
  ofstream file(FD(fp));
  return(this->unload(file));
}

int polyco::print(char * chpolly) const{
  // Terrible terrible terrible
  // use stringstreams
  this->unload("polyco.dat.tmp");
  FILE * fp;
  if((fp=fopen("polyco.dat.tmp", "r"))==NULL){
    fprintf(stderr, "polyco::print error - could not open file\n");
    return(-1);
  }
  if(fread(chpolly, this->size_in_bytes(), 1, fp)!=1){
    fprintf(stderr, "polyco::print error reading file\n");
    return(-1);
  }
  fclose(fp);
  remove("polyco.dat.tmp");
  return(0);
} 

void polyco::prettyprint(){
  for(int i=0; i<pollys.size(); ++i) 
    pollys[i].prettyprint();
}

polynomial polyco::nearest_polly(const MJD &t) const {

  int ipolly=0;
  while (ipolly<pollys.size()) {
    MJD t1 = pollys[ipolly].reftime - (double) pollys[ipolly].nspan_mins*60.0/2.0;
    MJD t2 = pollys[ipolly].reftime + (double) pollys[ipolly].nspan_mins*60.0/2.0;
    
    if (t>=t1 && t<=t2) {
      return(pollys[ipolly]);
    }
    ipolly++;
  }
  fprintf(stderr, "polyco::nearest_polly error - could not find polynomial for MJD %s\n", t.printall());
  string failed = "no polynomial";
  throw(failed);
}  

polynomial polyco::nearest_polly(string in_psrname,const MJD &t) const {

  int ipolly=0;
  while (ipolly<pollys.size()) {
    MJD t1 = pollys[ipolly].reftime - (double) pollys[ipolly].nspan_mins*60.0/2.0;
    MJD t2 = pollys[ipolly].reftime + (double) pollys[ipolly].nspan_mins*60.0/2.0;
    
    if (t>=t1 && t<=t2 && pollys[ipolly].psrname==in_psrname) {
      return(pollys[ipolly]);
    }
    ipolly++;
  }
  fprintf(stderr, "polyco::nearest_polly error - could not find polynomial for MJD %s\n", t.printall());
  string failed = "no polynomial";
  throw(failed);
}  

Phase polyco::phase(const MJD& t) const{
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(t);}
  catch(...) {throw("no polynomial");}
  return(nearest_polly.phase(t));
}

Phase polyco::phase(string in_psrname, const MJD& t) const {
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(in_psrname, t);}
  catch(...) {throw("no polynomial");}
  return(nearest_polly.phase(t));
}

Phase polyco::phase(const MJD& t, float obs_freq) const {
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(t);}
  catch(...) {throw("no polynomial");}
  return(nearest_polly.phase(t, obs_freq));
} 

Phase polyco::phase(string in_psrname, const MJD& t, float obs_freq) const {
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(in_psrname, t);}
  catch(...) {throw("no polynomial");}
  return(nearest_polly.phase(t, obs_freq));
}

double polyco::period(const MJD& t) const{
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(t);}
  catch(...) {throw("no polynomial");}
  return(nearest_polly.period(t));
}

double polyco::period(string in_psrname, const MJD& t) const{
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(in_psrname, t);}
  catch(...) { throw("no polynomial");}
  return(nearest_polly.period(t));
}

double polyco::frequency(const MJD& t) const {
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(t);}
  catch(...) { throw("no polynomial");}  
  return(nearest_polly.frequency(t));
}

double polyco::frequency(string in_psrname, const MJD& t) const {
  polynomial nearest_polly;
  try{ nearest_polly = this->nearest_polly(in_psrname, t);}
  catch(...) { throw("no polynomial");}
  return(nearest_polly.frequency(t));
}

size_t polyco::size_in_bytes() const{
  // this is truly a terrible way, but solaris
  // does not yet support stringstreams
  struct stat filestat;
  string s = "polyco.dat.tmp";
  ofstream file(s.c_str());
  if(this->unload(file)!=0){
    fprintf(stderr, "polyco::size_in_bytes error: could not unload file %s\n", s.c_str());
    return(0);
  }
  if(stat(s.c_str(), &filestat)!=0){
    fprintf(stderr, "polyco::size_in_bytes error: could not stat file %s\n", s.c_str());
    return(0);
  }
  remove(s.c_str());

  return(filestat.st_size);

//   for(int i=0; i<pollys.size(); ++i) 
//     size += pollys[i].size_in_bytes();
  //  return(size);
}



