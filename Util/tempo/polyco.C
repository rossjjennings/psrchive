
#include <stdio.h> 
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

#include "MJD.h"
#include "polyco.h"

extern "C" { int scan_fortran_(char *,double *,double *, double *, int ); }

polynomial::polynomial(){
  coefs = NULL;
  ncoef = 0;
}

polynomial::~polynomial(){
  if (ncoef!=0) delete [] coefs;
  ncoef = 0;
}

int polynomial::telid() const
{
  return telescope;
}

double polynomial::period(const MJD& tp) const
{
  double dp;                    // dphase/dt starts as phase per minute.
  MJD dt = tp - reftime;
  double t = dt.in_minutes();
  dp = coefs[1];
  for (int i=2;i<ncoef;i++) dp+=(double)(i)*coefs[i]*pow(t,(double)(i-1));
  dp /= (double) 60.0;          // Phase per second
  dp += f0;
  return(1.0/dp);               // Seconds per turn = period of pulsar
}

double polynomial::phase(const MJD& tp) const
{ 
   double p;

//   printf("polynomial::phase input time is %s\n",tp.printall());
//   printf("polynomial::phase refer time is %s\n",reftime.printall());

   MJD dt = tp - reftime;
   double t = dt.in_minutes();

//   printf("polynomial::phase dt in some easier is %s\n",dt.printall());
//   printf("polynomial::phase dt in minutes is %18.12lf\n",t);
   p = 0.0;

   for (int i=0;i<ncoef;i++) {
      p+= coefs[i]*pow(t,(double)i);
//      printf("polynomial::phase coef[%d] is %lf\n",i,coefs[i]);
   }
   p += t*f0*60.0;
   return(p+ph0);
}

double polynomial::phi(const MJD& tp) const
{
   double p;

//   printf("polynomial::phase input time is %s\n",tp.printall());
//   printf("polynomial::phase refer time is %s\n",reftime.printall());

   MJD dt = tp - reftime;
   double t = dt.in_minutes();

//   printf("polynomial::phase dt in some easier is %s\n",dt.printall());
//   printf("polynomial::phase dt in minutes is %18.12lf\n",t);
   p = 0.0;

   for (int i=0;i<ncoef;i++) {
      p+= fmod (coefs[i]*pow(t,(double)i), 1.0);
//      printf("polynomial::phase coef[%d] is %lf\n",i,coefs[i]);
   }
   p += fmod (t*f0*60.0, 1.0);

   // NEW
   //fprintf (stderr, "Phi not: %30.28lf, p: %30.28lf\n", ph0, p);
   //fflush (stderr);

   double phi = fmod (ph0, 1.0);

   //fprintf (stderr, "Fractional turns in Phi not: %30.28lf\n", phi);
   //fflush (stderr);

   phi += p;

   //fprintf (stderr, "Turns in phi: %d\n", (int)phi);
   //fflush (stderr);

   phi = fmod(phi,1.0);

   //fprintf (stderr, "Phi: %30.28lf\n", phi);
   //fflush (stderr);

   return(phi);
}

int polynomial::load(FILE * fptr){
   static char aline[120];
   int mjd_day_num;
   double frac_mjd;
   int ncoeftmp;
   
   // Grab first line.
   if (fgets(aline,120,fptr)!=NULL) {
     sscanf(aline,"%s",psrname);
     sscanf(&aline[10],"%s",date);
     sscanf(&aline[34],"%5d",&mjd_day_num);
     sscanf(&aline[39],"%lf",&frac_mjd);
     sscanf(&aline[60],"%f",&dm);

     reftime = MJD(mjd_day_num,(int) (86400.0 * frac_mjd),
         (double) fmod(frac_mjd*86400.0,1.0));
   }
   else
     return(-1);

   //Grab second line
   if (fgets(aline,120,fptr)!='\0') {
     char* period = strchr (aline, '.');
     if (period == NULL) {
       fprintf (stderr, "polynomial::load no decimal found ?!?\n");
       return -1;
     }
     sscanf(period,"%lf %lf %d %d %d %f %f %f",&ph0,&f0,&telescope,
     &nspan,&ncoeftmp,&freq,&binph,&binp);
   }
   else
     return(-1);

   // Is there room? If not make space.
   if (ncoeftmp>ncoef) {
     if (coefs!=NULL) delete [] coefs;
     coefs = new double[ncoeftmp];
     if (coefs == NULL)  {
       perror ("polynomial::load could not allocate new double");
       return -1;
     }
     ncoef = ncoeftmp;
   }

   // Read in the coefficients  - they come in lots of threes
   for (int i = 0;i<ncoef/3;i++){
     fgets(aline,120,fptr);
     // eliminate scan_fortran_ in favour of C code for test of theory
     //scan_fortran_(aline,&coefs[3*i],&coefs[3*i+1],&coefs[3*i+2],strlen(aline));
     char* capdee = strchr (aline, 'D');
     while (capdee) {
       *capdee = 'e';
       capdee = strchr (capdee+1, 'D');
     }
     sscanf(aline, "%lf %lf %lf", &coefs[3*i],&coefs[3*i+1],&coefs[3*i+2]);
   }

   return(0);
}

const char *polynomial::pulsar () const
{
  return psrname;
}

polyco::polyco(){
  init ();
}

void polyco::init()
{
  npollys = 0;
  pollys = NULL;
  the_tztot = the_polyco = NULL;
  nbytes_in_polyco = nbytes_in_tztot = 0;
}

/* ************************************************************************
   polyco constructor from user-specified pulsar name (and other catalogue
                      parameters)
   ************************************************************************ */

polyco::polyco (const char* psr, const MJD& m1, const MJD& m2, 
		int ns, int nc, int maxha, int tel)
{
  static char* errstr = NULL;
  init ();
  if (Construct (psr, NULL, m1, m2, ns, nc, maxha, tel) != 0) {
    if (errstr) free (errstr);
    errstr = strdup ("polyco::polyco - failed to construct\n");
    throw (errstr);
  }
}

/* ************************************************************************
   polyco constructor from user-specified .par or .eph file
   ************************************************************************ */

polyco::polyco (const char* psr, const char* parfile, 
		const MJD& m1, const MJD& m2, 
		int ns, int nc, int maxha, int tel)
{
  static char* errstr = NULL;
  init ();
  if (Construct (psr, parfile, m1, m2, ns, nc, maxha, tel) != 0) {
    if (errstr) free (errstr);
    errstr = strdup ("polyco::polyco - failed to construct\n");
    throw (errstr);
  }
}


int polyco::Construct (const char* psr, const char* parfile, 
		       const MJD& m1, const MJD& m2, 
		       int ns, int nc, int maxha, int tel)
{
  char  syscom[120];
  char* ephfile = NULL;
  int   rmeph = 0;

  if (parfile) {
    sprintf(syscom,"polyco -f %s %lf %lf %d %d %d %d > /dev/null",
	    parfile, m1.in_days(), m2.in_days(), ns, nc, maxha, tel);

    ephfile = new char [strlen(parfile) + 1];
    strcpy (ephfile, parfile);
  }
  else {
    sprintf(syscom,"polyco %s %lf %lf %d %d %d %d > /dev/null",
	    psr, m1.in_days(), m2.in_days(), ns, nc, maxha, tel);

    ephfile = new char [strlen(psr) + 5];
    if (psr[0] == 'J')
      psr++;
    sprintf (ephfile, "%s.eph",psr);
    rmeph = 1;
  }

  int status = system(syscom);
  if (status != 0) {
    fprintf (stderr, "polyco::polyco - failed %s ???\n", syscom);
    delete [] ephfile;
    return -1;
  }

  int retval = file_Construct ("polyco.dat", ephfile);
  if (retval != 0) {
    fprintf (stderr, "polyco::polyco - failed to construct from %s and %s\n",
	     "polyco.dat", ephfile);
  }

  if (rmeph)
    remove (ephfile);
  delete [] ephfile;
  remove ("polyco.dat");
  remove ("dates.tmp");
  remove ("tz.in");
  remove ("tz.tmp");
  remove ("tztot.dat");
  remove ("tempo.lis");

  return retval;
}

/* ************************************************************************
   polyco constructor from user-specified .polyco and .tztot file
   ************************************************************************ */

polyco::polyco(const char * id)
{
  char* polyname;
  char* tztotname;

  int length = strlen (id) + 10;
  polyname  = new char [length];
  tztotname = new char [length];

  sprintf(polyname, "%s.polyco",id); 
  sprintf(tztotname,"%s.tztot", id);

  int retval = file_Construct (polyname, tztotname);

  delete [] polyname;
  delete [] tztotname;

  if (retval != 0) {
    char errstr[256];
    sprintf (errstr, "polyco::polyco - failed to construct from %s and %s\n",
	     polyname, tztotname);
    throw (errstr);
  }
}

/* ************************************************************************
   the guts of all constructors listed above
   ************************************************************************ */

int polyco::file_Construct (const char* polyco_filename, 
			    const char* tztot_filename)
{
  FILE *polly, *tztot;
  struct stat  file_info;
  polynomial tst;
  
  // first, simply count the number of polynomials in the polyco file
  polly = fopen (polyco_filename, "r");
  if (polly == NULL)  {
    fprintf (stderr, "polyco::file_Construct - could not open %s\n",
	     polyco_filename);
    perror ("polyco::error");
    fflush (stderr);
    return -1;
  }
  npollys = 0;
  
  while (tst.load(polly)==0){
    npollys++;
  }
  pollys = new polynomial * [npollys];
  assert (pollys != NULL);
  
  // then, load all of the polynomials in the polyco file
  if (fseek (polly, 0, SEEK_SET) < 0)  {
    perror ("polyco::file_Construct - error fseek");
    fclose(polly);
    return -1;
  }
  for (int i=0;i<npollys;i++) {
    pollys[i]=new polynomial(); 
    pollys[i]->load(polly);
  }
  
  // then, load the raw file into the_polyco buffer
  if (stat (polyco_filename, &file_info) < 0)  {
    perror ("polyco::file_Construct - stat");
    fclose(polly);
    return -1;
  }
  if (fseek (polly, 0, SEEK_SET) < 0)  {
    perror ("polyco::file_Construct - error fseek");
    fclose(polly);
    return -1;
  }
  int count=file_info.st_size;
  the_polyco = new char [count+1];
  assert (the_polyco != NULL);
  fread(the_polyco,count,1,polly);
  nbytes_in_polyco = count;
  fclose(polly);
  
  // then, load the raw tztot file into the_tztot buffer
  if (stat (tztot_filename, &file_info) < 0)  {
    fprintf (stderr, "polyco::file_Construct - err stat(%s)",tztot_filename);
    perror ("");
    return -1;
  }
  count=file_info.st_size;
  the_tztot = new char [count+1];
  assert (the_tztot != NULL);
  
  tztot = fopen (tztot_filename, "r");
  if (tztot == NULL)  {
    fprintf (stderr, "polyco::file_Construct - could not open %s\n",
	     tztot_filename);
    perror ("polyco::error");
    return -1;
  }
  fread(the_tztot,count,1,tztot);
  nbytes_in_tztot = count;
  fclose(tztot);
  
  return 0;
}


polyco::~polyco()
{
  if (npollys!=0) {
    for (int i=0;i<npollys;i++) delete pollys[i];
    delete [] pollys;
  }
  if (nbytes_in_polyco!=0) {
    delete [] the_polyco;
  }
  if (nbytes_in_tztot!=0) {
    delete [] the_tztot;
  }
}

int polyco::telid () const
{
  if (pollys == NULL)
    return -1;
  if (pollys[0] == NULL)
    return -1;
  return pollys[0]->telid();
}


/*
 * The reference frequency for polyco::phase is obtained from the tzpar
 *   file for the pulsar.
 */
double polyco::phase(const MJD& t, float obs_freq) const
{
  /* This static tzrfrq is dangerous if there is more than one polyco instance
   *   with different reference frequencies.  This is rarely ever required,
   *   and there is a considerable speed increase from not having to
   *   read from file again and again.
   */
  static float tzrfrq = 0; 

  if (tzrfrq != 0)
    return (this->phase (t, obs_freq, tzrfrq));

  char *tempo_dir = getenv ("TEMPO");
  assert (tempo_dir != NULL);

  char *tempo_cfg = new char[FILENAME_MAX];
  assert (tempo_cfg != NULL);
  strcpy (tempo_cfg, tempo_dir);
  strcat (tempo_cfg, "/tempo.cfg");

  FILE *cfgfptr;
  if ((cfgfptr = fopen (tempo_cfg, "r")) == NULL) {
    fprintf (stderr, "polyco::phase - could not open tempo config file %s\n",
             tempo_cfg);
    return -1;
  }
  delete [] tempo_cfg;

  char *tempo_par = new char[FILENAME_MAX];
  assert (tempo_par != NULL);

  const int ALINE_MAX = 80;
  char aline[ALINE_MAX];
  fgets(aline, ALINE_MAX, cfgfptr);
  while (sscanf(aline, "PARDIR %s", tempo_par) != 1) 
    fgets(aline, ALINE_MAX, cfgfptr);
  fclose(cfgfptr);

  assert (pollys[0] != NULL);
  strncat (tempo_par, pollys[0]->pulsar (), 9);
  strcat (tempo_par, ".par"); 
  FILE *parfptr;
  if ((parfptr = fopen (tempo_par, "r")) == NULL) {
    fprintf (stderr, "polyco::phase - could not open tempo par file %s\n", 
             tempo_par);
    return -1;
  } 
  delete [] tempo_par;

  char tzrfrq_str[30];
  fgets(aline, ALINE_MAX, parfptr);
  while (sscanf(aline, "TZRFRQ %s", tzrfrq_str) != 1) 
    fgets(aline, ALINE_MAX, parfptr);
  fclose(parfptr);

  /* For some strange reason, exponentials are represented by 'D' in the
   *   tzpar files. 
   */
  char *dptr;
  if ((dptr = strchr(tzrfrq_str, 'D')) != NULL)
    *dptr = 'E';
  sscanf(tzrfrq_str, "%f", &tzrfrq); 

  return (this->phase (t, obs_freq, tzrfrq));
}

double polyco::phase(const MJD& t, float obs_freq, float ref_freq) const {
   float dm_delay_in_secs = pollys[0]->dm/2.41e-4*
        (1.0/(obs_freq*obs_freq)-1.0/(ref_freq*ref_freq));
   double current_period = this->period(t);
   if (current_period>0) {
      float dm_delay_in_turns = dm_delay_in_secs/current_period;
      double current_phase = this->phase(t);
      return(current_phase - dm_delay_in_turns);
   }
   else
     return(0);
}

double polyco::phi(const MJD& t) const {
  // Find correct polynomial and use it
  int ipolly=0;
  while (ipolly<npollys) {
    MJD t1 = pollys[ipolly]->reftime - (double) pollys[ipolly]->nspan*30.0;
    MJD t2 = pollys[ipolly]->reftime + (double) pollys[ipolly]->nspan*30.0;

    if (t<t1) {
      fprintf(stderr,"MJD %s not found in polyco\n",t.printall());
      fprintf(stderr,"lower b is %s\n",t1.printall());
      fprintf(stderr,"reftime is %s\n",pollys[ipolly]->reftime.printall());
      fprintf(stderr,"upper b is %s\n",t2.printall());
      return(0);
    }

    if (t>t1 && t<t2) {
      return(pollys[ipolly]->phi(t));
    }
    ipolly++;
  }
  return(0.0);
}

double polyco::phase(const MJD& t) const {
  // Find correct polynomial and use it
  int ipolly=0;
  while (ipolly<npollys) {
    MJD t1 = pollys[ipolly]->reftime - (double) pollys[ipolly]->nspan*30.0;
    MJD t2 = pollys[ipolly]->reftime + (double) pollys[ipolly]->nspan*30.0;

    if (t<t1) {
      fprintf(stderr,"MJD %s not found in polyco\n",t.printall());
      fprintf(stderr,"lower b is %s\n",t1.printall());
      fprintf(stderr,"reftime is %s\n",pollys[ipolly]->reftime.printall());
      fprintf(stderr,"upper b is %s\n",t2.printall());
      return(0);
    }

    if (t>t1 && t<t2) {
      return(pollys[ipolly]->phase(t));
    }
    ipolly++;
  }
  return(0.0);
}

double polyco::period(const MJD& t) const {
  // Find correct polynomial and use it
     int ipolly=0;
     while (ipolly<npollys) {
       MJD t1 = pollys[ipolly]->reftime - (double)(pollys[ipolly]->nspan*30.0);
       MJD t2 = pollys[ipolly]->reftime + (double)(pollys[ipolly]->nspan*30.0);

       if (t<t1) {
	 fprintf(stderr,"MJD %s not found in polyco\n",t.printall());
	 return(0.0);
       }

       if (t>t1 && t<t2) {
	 return(pollys[ipolly]->period(t));
       }
       ipolly++;
     }
    fprintf(stderr,"polyco::period WARNING time %s not found in polyco\n",
	    t.printall());
    return(-1.0);
}

int polyco::writepolyco(char * uni) const {
  char *name = new char[strlen(uni) + 8];
  FILE *fptr;

  sprintf(name,"%s.polyco",uni);
  fptr = fopen(name,"w");
  fwrite(the_polyco,nbytes_in_polyco,1,fptr);
  fclose(fptr); 
  return(0);
}

int polyco::writetztot(char * uni) const {
  char *name = new char[strlen(uni) + 7];
  FILE *fptr;

  sprintf(name,"%s.tztot",uni);
  fptr = fopen(name,"w");
  fwrite(the_tztot,nbytes_in_tztot,1,fptr);
  fclose(fptr); 
  return(0);
}
