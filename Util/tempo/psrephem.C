#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "psrephem.h"
#include "string_utils.h"

char* psrephem::tempo_pardir = NULL;
int   psrephem::verbose = 0;
char  psrephem::ephemstr [EPH_NUM_KEYS][EPH_STR_LEN];

static const char* psrephem_tmp_fname = "psrephem_tmp.eph";

void psrephem::init()
{
  nontempo11.erase();
  parmStatus    = NULL;
  value_str     = NULL;
  value_double  = NULL;
  value_integer = NULL;
  error_double  = NULL;
}

void psrephem::size_dataspace()
{
  if (!tempo11)  {
    destroy ();
    return;
  }

  if (parmStatus)  {
    zero();
    return;
  }

  parmStatus    = new int    [EPH_NUM_KEYS];
  value_double  = new double [EPH_NUM_KEYS];
  value_str     = new string [EPH_NUM_KEYS];
  value_integer = new int    [EPH_NUM_KEYS];
  error_double  = new double [EPH_NUM_KEYS];

  zero();
}

void psrephem::zero()
{
  char* tpo_safe = " ";
  static string tempo_safe_string (tpo_safe);

  for (int i=0;i<EPH_NUM_KEYS;i++) {
    parmStatus   [i] = 0;
    value_double [i] = 0.0;
    value_integer[i] = 0;
    value_str    [i] = tempo_safe_string;
    error_double[i]  = 0.0;
  }
}

void psrephem::destroy()
{
  if (parmStatus)  {
    delete [] parmStatus;
    delete [] value_double;
    delete [] value_integer;
    delete [] error_double;
    delete [] value_str;
  }
  init ();
}

psrephem::psrephem (const char* psr_name, int use_cwd)
{
  if (create (psr_name, use_cwd) < 0) {
    fprintf (stderr, "psrephem::error creating epemeris for %s.\n",
	     psr_name);
    throw ("psrephem::construction error");
  }
}

psrephem::psrephem (const char* filename)
{
  if (load (filename) < 0) {
    fprintf (stderr, "psrephem::error loading %s.\n", filename);
    throw ("psrephem::construction error");
  }
}

int psrephem::create (const char* psr_name, int use_cwd)
{
  string filename = par_lookup (psr_name, use_cwd);
  if (filename.empty()) {
    fprintf (stderr, "psrephem::create no ephemeris file for %s found.\n",
	     psr_name);
    return -1;
  }
  if (load (filename.c_str()) < 0) {
    fprintf (stderr, "psrephem::create error loading %s.\n", filename.c_str());
    return -1;
  }
  return 0;
}

int psrephem::load (const char* filename)
{
  tempo11 = 1;
  size_dataspace();

  rd_eph (filename, parmStatus, ephemstr, value_double,value_integer,error_double);
  int all_zero = 1;
  for (int i=0;i<EPH_NUM_KEYS;i++)  {
    if (parmStatus[i] == 1) {
      value_str[i] = ephemstr [i];
      all_zero = 0;
    }
  }
  if (all_zero) {
    if(verbose)
      fprintf (stderr,
	       "psrephem::load WARNING tempo11-style load of '%s' failed\n", filename);
    return old_load (filename);
  }

  if (verbose) {
    fprintf(stderr,"psrephem::load tempo11-style loaded '%s' ok\n", filename);
  }
  return 0;
}

int psrephem::old_load (const char* filename)
{
  tempo11 = 0;
  size_dataspace();

  FILE* fptr = fopen(filename,"r");
  if (fptr == NULL) {
    fprintf (stderr, "psrephem::old_load error fopen(%s)", filename);
    perror (":");
  }
  if (stringload (&nontempo11, fptr) < 0)  {
    fprintf (stderr, "psrephem::old_load error\n");
    return -1;
  }
  return 0;
}

int psrephem::unload (const char* filename) const
{
  if (tempo11)  {
    for (int i=0;i<EPH_NUM_KEYS;i++)
      strcpy (ephemstr[i], value_str[i].data());
    int istat = wr_eph (filename, parmStatus, ephemstr, value_double,
		        value_integer, error_double);
    if (!istat) {
      fprintf(stderr,"psrephem::unload error wr_eph %s\n", filename);
      return -1;
    }
  }
  else  {
    return old_unload (filename);
  }
  if (verbose) {
    fprintf(stderr,"psrephem::unload unloaded %s ok\n", filename);
  }
  return 0;
}

int psrephem::old_unload (const char* filename) const
{
  if (tempo11)
    return -1;

  FILE* fptr = fopen (filename,"w");
  if (fptr == NULL) {
    fprintf (stderr, "psrephem::old_unload error fopen(%s)", filename);
    perror (":");
    return -1;
  }
  if (fwrite (nontempo11.data(), 1, nontempo11.length(), fptr) 
	< nontempo11.length())  {
    perror ("psrephem::old_unload error");
    return -1;
  }
  fclose (fptr);
  return 0;
}

string psrephem::par_lookup (const char* name, int use_cwd)
{
  string filename;
  string psr_name;
  struct stat finfo;

  if (name[0] == 'J')
    psr_name = name + 1;
  else
    psr_name = name;

  // these string literals are assigned to char* to work around a
  // bug in the Sun C4.2 compiler
  char* cwd = "./";
  char* eph = ".eph";
  char* par = ".par";
  char* tempo_cfg = "/tempo.cfg";
  char* psrinfo_cmd = "psrinfo -e ";

  if (use_cwd) {
    /* Look for jname.eph in current directory */
    filename = cwd + psr_name + eph;
    if (stat (filename.c_str(), &finfo) == 0) {
      if (verbose) {
	fprintf (stderr, "psrephem::Using %s from cwd.\n", filename.c_str());
      }
      return filename;
    }
    filename = cwd + psr_name + par;
    if (stat (filename.c_str(), &finfo) == 0) {
      if (verbose) {
	fprintf (stderr, "psrephem::Using %s from cwd.\n", filename.c_str());
      }
      return filename;
    }
  }

  if (tempo_pardir == NULL) {
    /* Find PARDIR - the TEMPO directory for name.par files */
    char* tpodir = (char *) getenv("TEMPO");
    if (tpodir == NULL) {
      fprintf(stderr,"psrephem::TEMPO environment variable not defined\n");
    }
    else {
      filename = tpodir;
      filename += tempo_cfg;
      FILE* fptr = fopen(filename.c_str(),"r");
      if (fptr == NULL) {
	fprintf (stderr, "psrephem::par_lookup error fopen(%s)",
		 filename.c_str());
	perror ("");
      }
      else {
	char* readline = new char[100];
	char* whitespace = " \t\n";
	while (fgets (readline, 100, fptr) != NULL) {
	  char* token = strtok (readline, whitespace);
	  if (strcmp (token, "PARDIR") == 0) {
	    token = strtok (NULL, whitespace);
	    if (token)
	      tempo_pardir = strdup (token);
	  }
	} // end while reading new lines from tempo.cfg
	delete [] readline;
	if (tempo_pardir == NULL) {
	  if (verbose)
	    fprintf (stderr,
		     "psrephem:: PARDIR not defined in $TEMPO/tempo.cfg\n");
	}
      } // else if $TEMPO/tempo.cfg file opened successfully
    } // else if $TEMPO environment variable is defined
  } // end if tempo_pardir == NULL

  if (tempo_pardir != NULL) {
    filename = tempo_pardir + psr_name + par;
    if (stat (filename.c_str(), &finfo) == 0) {
      if (verbose)
	printf("psrephem:: Using %s from PARDIR\n", filename.c_str());
      return filename;
    }
  }

  /* Create name.eph in local directory */ 

  filename = psrinfo_cmd + psr_name;
  if (verbose)
    fprintf (stderr, "psrephem:: Creating %s.eph using psrinfo.\n",
	     psr_name.c_str());

  if (system(filename.c_str()) != 0) {
    fprintf (stderr, "psrephem:: Error executing system (%s)\n",
	     filename.c_str());
    filename.erase();
    return filename;
  }
  filename = cwd + psr_name + eph;
  if (stat (filename.c_str(), &finfo) == 0) {
    if (verbose)
      printf("psrephem:: Using %s from PARDIR\n", filename.c_str());
    return filename;
  }

  if (verbose) {
    fprintf (stderr, "psrephem:: Cannot find %s after call to psrinfo.\n", 
	     filename.c_str());
  }
  filename.erase();
  return filename;
}

string psrephem::psrname() const
{
  string empty;

  if (!tempo11)
    return empty;

  if (parmStatus[EPH_PSRJ]==1)
    return value_str[EPH_PSRJ];
  else if (parmStatus[EPH_PSRB]==1)
    return value_str[EPH_PSRJ];
  else if (verbose)
    fprintf(stderr, "psrephem::psrname() Error determining pulsar name\n");

  return empty;
}

double psrephem::dm() const
{
  if (!tempo11)
    return -1;

  if (parmStatus[EPH_DM]==1)
    return value_double[EPH_DM];

  if (verbose)
    fprintf(stderr, "psrephem::dm() DM not specified.\n");

  return -1.0;
}

void psrephem::nofit()
{
  if (!tempo11)
    return;

  for (int i=0;i<EPH_NUM_KEYS;i++) {
    if (parmStatus[i]==2) parmStatus[i]=1;
  }
}

int psrephem::load (FILE* fptr, size_t nbytes)
{
  string total;
  if (stringload (&total, fptr, nbytes) < 0)  {
    fprintf (stderr, "polyco::load error\n");
    return -1;
  }
  return load (&total);
}

int psrephem::unload (FILE* fptr) const
{
  string out;
  if (!tempo11)
    out = nontempo11;
  else if (unload(&out) < 0)
    return -1;

  size_t size = out.length();
  size_t bout = fwrite (out.data(), 1, size, fptr);
  if (bout < size)  {
    fprintf (stderr, "psrephem::unload(FILE*) ERROR fprintf only %lu/%lu",
        bout, size);
    perror ("");
    return -1;
  }
  fflush (fptr);
  return (int) bout;
}

int psrephem::load (string* instr)
{
  FILE* temp = fopen (psrephem_tmp_fname, "w");
  if (temp == NULL) {
    fprintf (stderr, "psrephem::load error fopen(%s)", psrephem_tmp_fname);
    perror ("");
    return -1;
  }
  ssize_t bytes = instr->length();

  ssize_t bio = fwrite (instr->c_str(), 1, bytes, temp);
  if (bio < bytes) {
    if (ferror (temp) != 0)
      perror ("psrephem::load error fwrite");
    fprintf (stderr, "psrephem::load fwrite only %d/%d bytes\n", bio,bytes);
    remove (psrephem_tmp_fname);
    return -1;
  }
  fclose (temp);
  if(this->load(psrephem_tmp_fname)!=0) return(-1);
  remove(psrephem_tmp_fname);
  return 0;
}

int psrephem::unload (string* outstr) const
{
  if (!tempo11)  {
    *outstr += nontempo11;
    return 0;
  }
  for (int i=0;i<EPH_NUM_KEYS;i++)
    strcpy (ephemstr[i], value_str[i].data());

  int istat = wr_eph (psrephem_tmp_fname, parmStatus, ephemstr, value_double,
		      value_integer, error_double);
  if (!istat) {
    fprintf(stderr,"psrephem::unload error unloading %s\n",psrephem_tmp_fname);
    remove (psrephem_tmp_fname);
    return -1;
  }

  FILE* temp = fopen (psrephem_tmp_fname, "r");
  if (temp == NULL) {
    fprintf (stderr, "psrephem::unload error fopen(%s)", psrephem_tmp_fname);
    perror ("");
    remove (psrephem_tmp_fname);
    return -1;
  }

  return (int) stringload (outstr, temp);
}

double psrephem::p(void)
{
  if (!tempo11)
    return -1;

  if ((value_double[EPH_F])!=0.0)  {
    return (1.0/value_double[EPH_F]);
  } 
  else
    fprintf (stderr,"psrephem::p warning rotation frequency 0.0\n");
  return(1.0);
}

double psrephem::p_err(void)
{
  if (!tempo11)
    return -1;

  if ((value_double[EPH_F])!=0.0){
    return (-1.0/value_double[EPH_F]/value_double[EPH_F]
	    *error_double[EPH_F]);
  } else
    fprintf(stderr,"psrephem::p_err warning rotation frequency 0.0\n");
  return(1.0);
}

psrephem::psrephem (const psrephem & p2)
{
  init ();
  *this = p2;
}

psrephem & psrephem::operator = (const psrephem & p2)
{
  if (this == &p2)
    return *this;

  tempo11 = p2.tempo11;
  size_dataspace();
  
  if (tempo11) {
    for (int i=0;i<EPH_NUM_KEYS;i++){
      parmStatus[i]=p2.parmStatus[i];
      value_double[i]=p2.value_double[i];
      value_integer[i]=p2.value_integer[i];
      error_double[i]=p2.error_double[i];
      value_str[i]=p2.value_str[i];
    }
  }
  else {
    nontempo11 = p2.nontempo11;
  }
  return *this;
}

polyco psrephem::mkpolyco (MJD m1, MJD m2, double nspan, int ncoeff, 
			   int maxha, int tel, double centrefreq) const
{
  // Write tz.in
  FILE* fptr = fopen ("tz.in","w");
  if (fptr == NULL) {
    perror ("psrephem::polyco Could not open tz.in\n");
    fprintf (stderr, "psrephem::polyco PWD:");
    system ("pwd");
    throw ("psrephem::polyco construct error");
  }
  fprintf (fptr,"%d %d %g %d %8g\n", 
	tel, maxha, nspan, ncoeff, centrefreq);
  fprintf(fptr,"\n");
  fprintf(fptr,"\n");
  fprintf(fptr,"%s\n",psrname().c_str());
  fclose(fptr);

  this->unload (psrephem_tmp_fname);

  // this is not at all funny.
  char* tempo_call = "tempo -z -f ";
  char* devnull = " > /dev/null";
  string syscall = tempo_call;
  syscall += psrephem_tmp_fname;
  syscall += devnull;

  if (verbose)  {
    fprintf (stderr, "psrephem::mkpolyco Calling '%s'\n", 
	syscall.c_str());
    fprintf (stderr, "psrephem::mkpolyco MJD1: '%g'\n", m1.in_days());
  }
  fptr = popen (syscall.c_str(), "w");
  if (fptr == NULL) {
    fprintf (stderr, "psrephem::mkpolyco Error calling '%s'", 
	syscall.c_str());
    perror ("");
    remove ("tz.in");
    throw ("psrephem::mkpolyco construct error");
  }
  if (verbose)  {
    fprintf (stderr, "psrephem::mkpolyco Successful popen tempo\n");
  }

  if (m1 == MJD(0.0,0.0,0.0)) {
    fprintf(fptr,"\n");
  }
  else {
    if (verbose)  {
      fprintf (stderr, "psrephem::mkpolyco Entering MJD1 '%g'\n", 
		m1.in_days());
    }
    fprintf (fptr, " %g", m1.in_days());
    if (verbose)  {
      fprintf (stderr, "psrephem::mkpolyco Entering MJD2 '%g'\n", 
		m2.in_days());
    }
    fprintf (fptr, " %g\n", m2.in_days());
  }
  fflush (fptr);
  pclose (fptr);
  remove ("tz.in");
  remove ("tz.tmp");
  remove ("tempo.lis");
  remove (psrephem_tmp_fname);

  if (verbose)  {
    fprintf (stderr, "psrephem::mkpolyco loading polyco.dat\n");
  }
  polyco poly;
  char* polyco_dat = "polyco.dat";
  if (poly.load(polyco_dat) < 1) {
    fprintf (stderr, "psrephem::polyco loaded less than 1 polyco\n");
  }
  remove ("polyco.dat");
  return poly;
}

