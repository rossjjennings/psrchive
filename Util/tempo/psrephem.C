#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "psrephem.h"

char* psrephem::tempo_pardir = NULL;
int   psrephem::verbose = 0;

psrephem::~psrephem(){
}

psrephem::psrephem()
{
  for (int i=0;i<EPH_NUM_KEYS;i++) {
    parmStatus   [i] = 0;
    value_double [i] = 0.0;
    value_integer[i] = 0;
    error_double[i]  = 0.0;
    value_str[i][0]  = ' ';
    value_str[i][1]  = '\0';
  }
}

psrephem::psrephem (char* psr_name, int use_cwd)
{
  if (create (psr_name, use_cwd) < 0) {
    fprintf (stderr, "psrephem::error creating epemeris for %s.\n",
	     psr_name);
    throw ("psrephem::construction error");
  }
}

psrephem::psrephem (char* filename)
{
  if (load (filename) < 0) {
    fprintf (stderr, "psrephem::error loading %s.\n", filename);
    throw ("psrephem::construction error");
  }
}

int psrephem::create (char* psr_name, int use_cwd)
{
  string filename = par_lookup (psr_name, use_cwd);

  if (filename == "") {
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
  int istat = rd_eph (filename, parmStatus, value_str, value_double,
		      value_integer, error_double);
  if (!istat) {
    fprintf(stderr,"psrephem::load error rd_eph %s\n", filename);
    return -1;
  }
  if (verbose) {
    fprintf(stderr,"psrephem::load loaded %s ok\n", filename);
  }
  return 0;
}

int psrephem::unload (const char* filename) const
{
  int istat = wr_eph (filename, parmStatus, value_str, value_double,
		      value_integer, error_double);
  if (!istat) {
    fprintf(stderr,"psrephem::unload error wr_eph %s\n", filename);
    return -1;
  }
  if (verbose) {
    fprintf(stderr,"psrephem::unload unloaded %s ok\n", filename);
  }
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

  if (use_cwd) {
    /* Look for jname.eph in current directory */
    filename = "./" + psr_name + ".eph";
    if (stat (filename.c_str(), &finfo) == 0) {
      if (verbose) {
	fprintf (stderr, "psrephem::Using %s from cwd.\n", filename.c_str());
      }
      return filename;
    }
    filename = "./" + psr_name + ".par";
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
      filename += "/tempo.cfg";
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
    filename = tempo_pardir + psr_name + ".par";
    if (stat (filename.c_str(), &finfo) == 0) {
      if (verbose)
	printf("psrephem:: Using %s from PARDIR\n", filename.c_str());
      return filename;
    }
  }

  /* Create name.eph in local directory */ 

  filename = "psrinfo -e " + psr_name;
  if (verbose)
    fprintf (stderr, "psrephem:: Creating %s.eph using psrinfo.\n",
	     psr_name.c_str());

  if (system(filename.c_str()) != 0) {
    fprintf (stderr, "psrephem:: Error executing system (%s)\n",
	     filename.c_str());
    filename = "";
    return filename;
  }
  filename = "./" + psr_name + ".eph";
  if (stat (filename.c_str(), &finfo) == 0) {
    if (verbose)
      printf("psrephem:: Using %s from PARDIR\n", filename.c_str());
    return filename;
  }

  if (verbose) {
    fprintf (stderr, "psrephem:: Cannot find %s after call to psrinfo.\n", 
	     filename.c_str());
  }
  filename = "";
  return filename;
}

string psrephem::psrname()
{
  string ret = "";

  if (parmStatus[EPH_PSRJ]==1)
    ret = value_str[EPH_PSRJ];
  else if (parmStatus[EPH_PSRB]==1)
    ret = value_str[EPH_PSRJ];
  else if (verbose)
    fprintf(stderr, "psrephem::psrname() Error determining pulsar name\n");

  return ret;
}

void psrephem::nofit(){
  for (int i=0;i<EPH_NUM_KEYS;i++) {
    if (parmStatus[i]==2) parmStatus[i]=1;
  }
}


static char*  psre_buffer = NULL;
static size_t psre_bufsz = 0;

int psrephem::load (FILE* instream, size_t bytes)
{
  if (bytes < 1)
    return -1;
  if (!instream)
    return -1;

  if (psre_bufsz < bytes) {
    if (psre_buffer) delete [] psre_buffer;
    psre_buffer = new char [bytes];
    if (psre_buffer == NULL) {
      fprintf (stderr, "psrephem::load could not allocate %d bytes", bytes);
      perror ("");
      throw ("psrephem::load memory allocation error");
    }
    psre_bufsz = bytes;
  }

  size_t bio = fread (psre_buffer, 1, bytes, instream);
  if (bio < bytes) {
    if (ferror (instream) != 0)
      perror ("psrephem::load error fread");
    fprintf (stderr, "psrephem::load fread only %d/%d bytes\n", bio,bytes);
    return -1;
  }

  char* filename = "psrephem_load.tmp";
  // mktemp (filename); // stdlib.h

  FILE* temp = fopen (filename, "w");
  if (temp == NULL) {
    fprintf (stderr, "psrephem::load error fopen(%s)", filename);
    perror ("");
    return -1;
  }
  bio = fwrite (psre_buffer, 1, bytes, temp);
  if (bio < bytes) {
    if (ferror (instream) != 0)
      perror ("psrephem::load error fwrite");
    fprintf (stderr, "psrephem::load fwrite only %d/%d bytes\n", bio,bytes);
    remove (filename);
    return -1;
  }
  fclose (temp);

  int istat = rd_eph (filename, parmStatus, value_str, value_double,
		      value_integer, error_double);
  remove (filename);
  if (!istat) {
    fprintf(stderr,"psrephem::load error loading %s\n",filename);
    return -1;
  }
  return 0;
}

//int psrephem::load (FILE* instream, size_t bytes)

int psrephem::unload (FILE* outstream, size_t* nbytes) const
{
  char* filename = "psrephem_unload.tmp";
  // mktemp (filename);  // stdlib.h

  int istat = wr_eph (filename, parmStatus, value_str, value_double,
		      value_integer, error_double);
  if (!istat) {
    fprintf(stderr,"psrephem::unload error unloading %s\n",filename);
    remove (filename);
    return -1;
  }

  FILE* temp = fopen (filename, "r");
  if (temp == NULL) {
    fprintf (stderr, "psrephem::unload error fopen(%s)", filename);
    perror ("");
    remove (filename);
    return -1;
  }

  if (fseek (temp, 0L, SEEK_END) != 0) {
    perror ("psrephem::unload error fseek()");
    remove (filename);
    return -1;
  }
  size_t bytes = ftell (temp);
  if (verbose)
    fprintf (stderr, "psrephem::unload size of ephemeris: %d bytes\n", bytes);

  if (psre_bufsz < bytes) {
    if (psre_buffer) delete [] psre_buffer;
    psre_buffer = new char [bytes];
    if (psre_buffer == NULL) {
      fprintf (stderr, "psrephem::unload could not allocate %d bytes", bytes);
      perror ("");
      remove (filename);
      throw ("psrephem::unload memory allocation error");
    }
    psre_bufsz = bytes;
  }

  if (fseek (temp, 0L, SEEK_SET) != 0) {
    perror ("psrephem::unload error fseek()");
    remove (filename);
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "psrephem::unload fread %d bytes\n", bytes);
#endif
  size_t bio = fread (psre_buffer, 1, bytes, temp);
  if (bio < bytes) {
    if (ferror (outstream) != 0)
      perror ("psrephem::unload error fread");
    fprintf (stderr, "psrephem::unload fread only %d/%d bytes\n", bio,bytes);
    remove (filename);
    return -1;
  }

  remove (filename);

#ifdef _DEBUG
  fprintf (stderr, "psrephem::unload fwrite %d bytes\n", bytes);
#endif
  bio = fwrite (psre_buffer, 1, bytes, outstream);
  if (bio < bytes) {
    if (ferror (outstream) != 0)
      perror ("psrephem::unload error fread");
    fprintf (stderr, "psrephem::unload fwrite only %d/%d bytes\n", bio,bytes);
    return -1;
  }

  *nbytes = bytes;
  return 0;
}

double psrephem::p(void)
{
    if ((value_double[EPH_F])!=0.0){
    return (1.0/value_double[EPH_F]);
  } else
  fprintf(stderr,"psrephem::p warning rotation frequency 0.0\n");
  return(1.0);
}

double psrephem::p_err(void)
{
  if ((value_double[EPH_F])!=0.0){
    return (-1.0/value_double[EPH_F]/value_double[EPH_F]
	    *error_double[EPH_F]);
  } else
    fprintf(stderr,"psrephem::p_err warning rotation frequency 0.0\n");
  return(1.0);
}

psrephem::psrephem (const psrephem & p2)
{
  *this = p2;
}

psrephem & psrephem::operator = (const psrephem & p2)
{
  if (this != &p2) {
    for (int i=0;i<EPH_NUM_KEYS;i++){
      parmStatus[i]=p2.parmStatus[i];
      value_double[i]=p2.value_double[i];
      value_integer[i]=p2.value_integer[i];
      error_double[i]=p2.error_double[i];
      for (int j=0;j<EPH_STR_LEN;j++)value_str[i][j]=p2.value_str[i][j];
    }
  }
  return *this;
}

// Default arguments are those of polyco

#define PSREPHEM_OUT "psrephem_tmp.eph"

//polyco psrephem::polyco (MJD m1=MJD(0.0,0.0,0.0), MJD m2=MJD(0.0,0.0,0.0), 
polyco psrephem::mkpolyco (MJD m1, MJD m2, double nspan, int ncoeff, 
			 int maxha, int tel, double centrefreq)
{
  // Write tz.in
  FILE* fptr = fopen ("tz.in","w");
  if (fptr == NULL) {
    perror ("psrephem::polyco Could not open tz.in\n");
    fprintf (stderr, "psrephem::polyco PWD:");
    system ("pwd");
    throw ("psrephem::polyco construct error");
  }
  fprintf(fptr,"%d %d %g %d %8g\n", tel, maxha, nspan, ncoeff, centrefreq);
  fprintf(fptr,"\n");
  fprintf(fptr,"\n");
  fprintf(fptr,"%s\n",psrname().c_str());
  fclose(fptr);

  this->unload (PSREPHEM_OUT);

  string syscall = "tempo -z -f ";
  syscall += PSREPHEM_OUT;

  fptr = popen (syscall.c_str(), "w");
  if (fptr == NULL) {
    fprintf (stderr, "psrephem::polyco Error calling '%s'", syscall.c_str());
    perror ("");
    remove ("tz.in");
    throw ("psrephem::polyco construct error");
  }
  if (m1 == MJD(0.0,0.0,0.0)) {
    fprintf(fptr,"\n");
  }
  else {
    fprintf (fptr, " %s", m1.in_days());
    fprintf (fptr, " %s\n", m2.in_days());
  }
  fflush (fptr);
  pclose (fptr);
  remove ("tz.in");
  remove ("tz.tmp");
  remove ("tempo.lis");
  remove (PSREPHEM_OUT);

  polyco poly;
  if (poly.load("polyco.dat") < 1) {
    fprintf (stderr, "psrephem::polyco loaded less than 1 polyco\n");
  }
  remove ("polyco.dat");
  return poly;
}

