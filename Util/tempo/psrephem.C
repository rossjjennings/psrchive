#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include <iostream>

#include "ephio.h"
#include "psrephem.h"
#include "string_utils.h"
#include "genutil.h"
#include "dirutil.h"

psrephem::~psrephem(){ destroy(); }

// defines the recognized filename extensions used for pulsar ephemeris files
vector<string> psrephem::extensions ()
{
  vector <string> retval;

  retval.push_back (string (".eph"));
  retval.push_back (string (".par"));

  return retval;
}

char* psrephem::tempo_pardir = NULL;
int   psrephem::verbose = 0;

static char ephemstr [EPH_NUM_KEYS][EPH_STR_LEN];

void psrephem::init()
{
  nontempo11.erase();
  parmStatus    = NULL;
  value_str     = NULL;
  value_double  = NULL;
  value_integer = NULL;
  error_double  = NULL;

  tempo11 = false;
}

void psrephem::size_dataspace()
{
  if (!tempo11)  {
    destroy ();
    return;
  }

  if (parmStatus != NULL)  {
    // the arrays have already been initialized.  zero them
    if (verbose)
      fprintf (stderr, "psrephem::size_dataspace zero\n");
    zero_work();
    return;
  }
  
  parmStatus    = new int    [EPH_NUM_KEYS];
  value_double  = new double [EPH_NUM_KEYS];
  value_str     = new string [EPH_NUM_KEYS];
  value_integer = new int    [EPH_NUM_KEYS];
  error_double  = new double [EPH_NUM_KEYS];
  
  zero_work();
}

void psrephem::zero()
{
  tempo11 = true;
  size_dataspace ();
}


void psrephem::zero_work()
{
  static string tempo_safe_string (" ");
  
  for (int i=0;i<EPH_NUM_KEYS;i++) {
    parmStatus   [i] = 0;
    value_double [i] = 0.0;
    value_integer[i] = 0;
    value_str    [i] = tempo_safe_string;
    error_double [i]  = 0.0;
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
  init();
  if (create (psr_name, use_cwd) < 0) {
    fprintf (stderr, "psrephem::psrephem error creating epemeris for %s.\n",
	     psr_name);
    throw Error(FailedCall, "psrephem::psrephem");
  }
}

psrephem::psrephem (const char* filename)
{
  init();
  if (load (filename) < 0) {
    fprintf (stderr, "psrephem::psrephem error loading %s.\n", filename);
    throw Error(FailedCall, "psrephem::psrephem");
  }
}

int psrephem::create (const char* psr_name, int use_cwd)
{
  if (verbose)
    fprintf (stderr, "psrephem::create '%s'\n", psr_name);
 
  string filename = par_lookup (psr_name, use_cwd);
  if (filename.empty()) {
    fprintf (stderr, "psrephem::create no ephemeris file for %s found.\n",
	     psr_name);
    return -1;
  }
  if (verbose)
    fprintf (stderr, "psrephem::create loading '%s'\n", filename.c_str());
  if (load (filename.c_str()) < 0) {
    fprintf (stderr, "psrephem::create error loading %s.\n", filename.c_str());
    return -1;
  }
  return 0;
}

int psrephem::load (const char* filename)
{
  if (verbose) 
    cerr << "psrephem::load enter" << endl;

  tempo11 = 1;
  size_dataspace();
  
  if (verbose)
    cerr << "psrephem::load rd_eph (" << filename << ")" << endl;

  rd_eph (const_cast<char*>(filename), parmStatus, ephemstr, value_double, 
	  value_integer, error_double);

  int all_zero = 1;
  for (int i=0;i<EPH_NUM_KEYS;i++)  {
    if (parmStatus[i]) {
      value_str[i] = ephemstr [i];
      all_zero = 0;
    }
  }
  if (all_zero) {
    if (verbose)
      fprintf (stderr, "psrephem::load WARNING "
	       "tempo11-style load of '%s' failed\n", filename);
    return old_load (filename);
  }

  if (verbose)
    fprintf (stderr,"psrephem::load tempo11-style loaded '%s' ok\n", filename);

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
  fclose(fptr);
  return 0;
}

int psrephem::unload (const char* filename) const
{
  if (!tempo11)
    return old_unload (filename);

  for (int ieph=0; ieph<EPH_NUM_KEYS; ieph++)
    strcpy (ephemstr[ieph], value_str[ieph].c_str());

  int istat = wr_eph (const_cast<char*>(filename), parmStatus, ephemstr,
		      value_double, value_integer, error_double);
  if (!istat) {
    fprintf (stderr, "psrephem::unload error wr_eph '%s'\n", filename);
    return -1;
  }

  if (verbose)
    fprintf (stderr, "psrephem::unload unloaded %s ok\n", filename);

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
  if (fwrite (nontempo11.c_str(), 1, nontempo11.length(), fptr) 
	< nontempo11.length())  {
    perror ("psrephem::old_unload error");
    return -1;
  }
  fclose (fptr);
  return 0;
}

static string directory;

string psrephem::get_directory ()
{
  char* unknown = "unknown";

  if (!directory.length()) {
    char* userid = getenv ("USER");
    if (!userid)
      userid = unknown;

    directory = string ("/tmp/tempo/") + userid;
  }

  if (makedir (directory.c_str()) < 0)  {

    if (verbose)
      cerr << "get_directory failure creating '" << directory << "'" << endl;

    char* home = getenv ("HOME");

    if (home)
      directory = home;
    else
      directory = ".";

    directory += "/tempo.tmp";

    if (makedir (directory.c_str()) < 0)
      throw Error (InvalidState, "Tempo::get_directory",
		   "cannot create a temporary working directory");
  }

  return directory;
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
  char* tempo_cfg = "/tempo.cfg";
  char* psrinfo_cmd = "psrinfo -e ";

  if (use_cwd) {
    vector <string> exts = extensions ();
    for (unsigned iext=0; iext < exts.size(); iext++) {
      /* Look for jname.ext in current directory */
      filename = psr_name + exts[iext];
      if (stat (filename.c_str(), &finfo) == 0) {
	if (verbose)
	  cerr << "psrephem::Using " << filename << " from cwd" << endl;
	return filename;
      }
    }
  }

  if (tempo_pardir == NULL) {
    if (verbose)
      fprintf(stderr,"psrephem::par_lookup load tempo .par directory\n");
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
    if (verbose)
      fprintf(stderr,"psrephem::par_lookup using TEMPO .par = '%s'\n",
	      tempo_pardir);
    filename = tempo_pardir + psr_name + ".par";
    if (stat (filename.c_str(), &finfo) == 0) {
      if (verbose)
	cerr << "psrephem:: Using " << filename 
	     << " from PARDIR:" << tempo_pardir << endl;
      return filename;
    }
    if (verbose)
      fprintf(stderr,"psrephem::par_lookup using TEMPO .eph = '%s'\n",
	      tempo_pardir);
    filename = tempo_pardir + psr_name + ".eph";
    if (stat (filename.c_str(), &finfo) == 0) {
      if (verbose)
	cerr << "psrephem:: Using " << filename 
	     << " from PARDIR:" << tempo_pardir << endl;
      return filename;
    }
  }
  
  /* Create name.eph in local directory */ 

  filename = psrinfo_cmd + psr_name;

  if (verbose)
    cerr << "psrephem:: Creating ephemeris by psrinfo -e " << psr_name <<endl;

  // start with a clean working directory
  removedir (get_directory().c_str());

  char cwd[FILENAME_MAX];

  if (getcwd (cwd, FILENAME_MAX) == NULL)
    throw Error (FailedSys, "psrephem", "failed getcwd");
  
  if (chdir (get_directory().c_str()) != 0)
    throw Error (FailedSys, "psrephem",
		 "failed chdir(" + get_directory() + ")");


  int retval = system(filename.c_str());

  if (chdir (cwd) != 0)
    throw Error (FailedSys, "psrephem", "failed chdir(%s)", cwd);

  if (retval != 0) {

    cerr << "psrephem:: Error executing system (" + filename + ")" << endl;
    filename.erase();
    return filename;

  }

  vector<string> filenames;
  dirglob (&filenames, get_directory() + "/*.eph");

  if (filenames.size() != 1)
    throw Error (InvalidState, "psrephem", "psrinfo created %d files",
		 filenames.size());

  filename = filenames[0];

  if (stat (filename.c_str(), &finfo) == 0) {
    if (verbose)
      printf("psrephem:: Using '%s'\n", filename.c_str());
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
  if (tempo11 && parmStatus[EPH_PSRJ])
    return value_str[EPH_PSRJ];
  else if (tempo11 && parmStatus[EPH_PSRB])
    return value_str[EPH_PSRB];
  
  throw Error(InvalidParam, "psrephem::psrname",
	      "Error determining pulsar name");
}

double psrephem::get_dm() const
{
  if (tempo11 && parmStatus[EPH_DM])
    return value_double[EPH_DM];

  return 0;
}

void psrephem::set_dm( double dm )
{
  if (!tempo11)
    return;

  if (!parmStatus[EPH_DM])
    parmStatus[EPH_DM] = 1;

  value_double[EPH_DM] = dm;
}

double psrephem::jra() const
{
  if (tempo11 && parmStatus[EPH_RAJ])
    return value_double[EPH_RAJ];
  
  throw Error(InvalidParam, "psrephem::jra",
	      "Error determining pulsar RA");
}

double psrephem::jdec() const
{
  if (tempo11 && parmStatus[EPH_DECJ])
    return value_double[EPH_DECJ];
  
  throw Error(InvalidParam, "psrephem::jdec",
	      "Error determining pulsar DEC");
}

double psrephem::omega() const
{
  if (tempo11 && parmStatus[EPH_OM])
    return value_double[EPH_OM];
  
  throw Error(InvalidParam, "psrephem::omega",
	      "Error determining pulsar OMEGA");
}

double psrephem::omdot() const
{
  if (tempo11 && parmStatus[EPH_OMDOT])
    return value_double[EPH_OMDOT];
  
  throw Error(InvalidParam, "psrephem::omdot",
	      "Error determining pulsar OMDOT");
}

double psrephem::ecc() const
{
  if (tempo11 && parmStatus[EPH_E])
    return value_double[EPH_E];
  
  throw Error(InvalidParam, "psrephem::ecc",
	      "Error determining pulsar E");
}

double psrephem::t0() const
{
  if (tempo11 && parmStatus[EPH_T0]) {
    MJD current_epoch = MJD (value_integer[EPH_T0],
			     value_double [EPH_T0]);
    return current_epoch.in_days();
  }
  
  throw Error(InvalidParam, "psrephem::t0",
	      "Error determining pulsar T0");
}

double psrephem::a1() const
{
  if (tempo11 && parmStatus[EPH_A1])
    return (value_double[EPH_A1]);
  
  throw Error(InvalidParam, "psrephem::a1",
	      "Error determining pulsar A1");
}

double psrephem::pb() const
{
  if (tempo11 && parmStatus[EPH_PB])
    return (value_double[EPH_PB]);
  
  throw Error(InvalidParam, "psrephem::pb",
	      "Error determining pulsar PB");
}

void psrephem::nofit()
{
  if (!tempo11)
    return;

  for (int i=0;i<EPH_NUM_KEYS;i++) {
    if (parmStatus[i]==2) parmStatus[i]=1;
  }
}

void psrephem::fitall()
{
  if (!tempo11)
    return;

  for (int i=0;i<EPH_NUM_KEYS;i++) {
    if (parmStatus[i]==1) parmStatus[i]=2;
  }
}

void psrephem::efac (float fac)
{
  if (!tempo11)
    return;

  for (int i=0;i<EPH_NUM_KEYS;i++)
    if (parmStatus[i]==2) error_double[i]*=fac;
}

int psrephem::load (FILE* fptr, size_t nbytes)
{
  string total;
  if (stringload (&total, fptr, nbytes) < 0)  {
    fprintf (stderr, "psrephem::load(FILE*) error\n");
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
  size_t bout = fwrite (out.c_str(), 1, size, fptr);
  if (bout < size)  {
    cerr << "psrephem::unload(FILE*) ERROR fprintf " << bout << "/" << size;
    perror ("");
    return -1;
  }
  fflush (fptr);
  return (int) bout;
}

static char* ephemblock = NULL;
static int*  correct    = NULL;
static void prepare_static_load_area ()
{
  if (ephemblock == NULL) {
    ephemblock = new char [EPH_NUM_KEYS * EPH_STR_LEN];
    assert (ephemblock != NULL);
  }
  if (correct == NULL) {
    correct = new int [EPH_NUM_KEYS];
    assert (correct != NULL);
  }
  for (int i=0; i<EPH_NUM_KEYS; i++)
    correct [i] = 0;

  for (int c=0; c<EPH_NUM_KEYS*EPH_STR_LEN; c++)
    ephemblock[c] = ' ';
}

int psrephem::load (string* instr)
{
  if (verbose)
    cerr << "psrephem::load (string*) *****" << endl
	 << *instr << "***** END" << endl;

  tempo11 = 1;
  size_dataspace();
  prepare_static_load_area ();

  nontempo11 = *instr;  // just in case parsing fails
  int old_ephem = 0;

  while (instr -> length() > 1) {
    // get the next line from the incoming text
    string line ( stringtok (instr, "\n") );
    if (verbose)
      cerr << "psrParams::load '" << line << "' len=" << line.length() 
	   << " instr.len=" << instr -> length() << endl;

    if (line.length() < 1)
      continue;

    rd_eph_str (parmStatus, ephemblock, value_double, value_integer,
		error_double, correct, &old_ephem, 
		const_cast<char*>( line.c_str() ));
  }
  // convertUnits_ defined in ephio.f
  convertunits_ (value_double, error_double, parmStatus, correct);
  if (verbose)
    cerr << "psrParams::load units converted" << endl;

  bool all_zero = 1;
  for (int ieph=0; ieph<EPH_NUM_KEYS; ieph++)  {
    if (parmStatus[ieph] == 0)
      continue;

    char* strval = ephemblock + ieph * EPH_STR_LEN;
    // length_ defined in ephio.f
    int length = length_ (strval, EPH_STR_LEN);
    if (length != 0) {
      strval[length] = '\0';
      value_str[ieph] = strval;
    }
    all_zero = 0;
  }
  
  if (all_zero) {
    if (verbose)
      cerr << "psrephem::load WARNING "
	"tempo11-style load of '" << nontempo11 << "' failed" << endl;
    tempo11 = 0;
  }

  return 0;
}

int psrephem::unload (string* outstr) const
{
  if (!tempo11)  {
    *outstr += nontempo11;
    return nontempo11.size();
  }

  if (parmStatus == NULL)
    return 0;

  static const int buflen = 80;
  static char* buffer = NULL;
  static string newline ("\n");

  if (buffer == NULL)
    buffer = new char [buflen];

  char ephstr [EPH_STR_LEN];
  unsigned bytes_out = 0;
  for (int ieph=0; ieph < EPH_NUM_KEYS; ieph++) {
    if (!parmStatus[ieph])
      continue;

    strcpy (ephstr, value_str[ieph].c_str());

    wr_eph_str (buffer, buflen, ieph, parmStatus[ieph], 
		ephstr, value_double[ieph], value_integer[ieph],
		error_double[ieph]);

    // if (verbose)
    //cerr << "   '" << buffer << "'" << endl;

    string lineout = buffer + newline;
    *outstr += lineout;
    bytes_out += lineout.size();
  }

  return bytes_out;
}

double psrephem::p(void) const
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

double psrephem::p_err(void) const
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

  destroy();
  init();
  if(!p2.parmStatus)
    return(*this);

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

bool operator == (const psrephem &e1, const psrephem &e2)
{
  if (e1.parmStatus==NULL && e2.parmStatus==NULL)
    return 1;

  if ((e1.parmStatus==NULL && e2.parmStatus!=NULL) ||
      (e1.parmStatus!=NULL && e2.parmStatus==NULL))
    return 0;

  if (e1.tempo11 != e2.tempo11) {
    if (psrephem::verbose)
      cerr << "psrephem::operator== unequal tempo11 flags" << endl;
    return 0;
  }

  if (!e1.tempo11)
    return e1.nontempo11 == e2.nontempo11;

  for (int ieph=0;ieph<EPH_NUM_KEYS;ieph++) {

    if (e1.parmStatus[ieph] != e2.parmStatus[ieph]) {
      if (psrephem::verbose)
	cerr << "psrephem::operator== unequal parmStatus["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
    if (e1.value_double[ieph] != e2.value_double[ieph]) {
      if (psrephem::verbose)
	cerr << "psrephem::operator== unequal value_double["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
    if (e1.value_integer[ieph] != e2.value_integer[ieph]) {
      if (psrephem::verbose)
	cerr << "psrephem::operator== unequal value_integer["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
    if (e1.error_double[ieph] != e2.error_double[ieph]) {
      if (psrephem::verbose)
	cerr << "psrephem::operator== unequal value_integer["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
    if (e1.value_str[ieph] != e2.value_str[ieph])  {
      if (psrephem::verbose)
	cerr << "psrephem::operator== unequal value_integer["
	     << parmNames[ieph] << "]" << endl;
      return 0;
    }
    
  }

  return true;

}

bool operator != (const psrephem &e1, const psrephem &e2) {
  return ! operator == (e1,e2);
}

ostream& operator<< (ostream& ostr, const psrephem& eph)
{  
  string out;
  eph.unload (&out);
  ostr << out;
  return ostr;
}

// ///////////////////////////////////////////////////////////////////////
//
// set/get functions
//
// ///////////////////////////////////////////////////////////////////////

string psrephem::get_string  (int ephind)
{
  if (parmTypes[ephind] != 0)
    throw Error (InvalidParam, "psrephem::get_string",
                 "%s is not a string", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind])
    return value_str[ephind];
  else
    return "";
}

double psrephem::get_double  (int ephind)
{
  if (parmTypes[ephind] != 1)
    throw Error (InvalidParam, "psrephem::get_double",
                 "%s is not a double", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind])
    return value_double[ephind];
  else
    return 0.0;
}

MJD psrephem::get_MJD (int ephind)
{
  if (parmTypes[ephind] != 4)
    throw Error (InvalidParam, "psrephem::get_MJD",
                 "%s is not a MJD", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind])
    return MJD (value_integer[ephind], value_double[ephind]);
  else
    return MJD::zero;
}

Angle psrephem::get_Angle (int ephind)
{
  if (parmTypes[ephind] != 2 && parmTypes[ephind] != 3)
    throw Error (InvalidParam, "psrephem::get_Angle",
                 "%s is not an  Angle", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind]) {
    Angle ret;
    ret.setTurns (value_double[ephind]);
    return ret;
  }
  else
    return Angle();
}

int psrephem::get_integer (int ephind)
{
  if (parmTypes[ephind] != 5)
    throw Error (InvalidParam, "psrephem::get_integer",
                 "%s is not an integer", parmNames[ephind]);

  if (tempo11 && parmStatus[ephind])
    return value_integer[ephind];
  else
    return 0;
}

void psrephem::set_string (int ephind, const string& value)
{
  if (parmTypes[ephind] != 0)
    throw Error (InvalidParam, "psrephem::get_string",
                 "%s is not a string", parmNames[ephind]);

  value_str[ephind] = value;

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

void psrephem::set_double (int ephind, double value)
{
  if (parmTypes[ephind] != 1)
    throw Error (InvalidParam, "psrephem::get_double",
                 "%s is not a double", parmNames[ephind]);

  value_double[ephind] = value;

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

void psrephem::set_MJD (int ephind, const MJD& value)
{
  if (parmTypes[ephind] != 4)
    throw Error (InvalidParam, "psrephem::get_MJD",
                 "%s is not a MJD", parmNames[ephind]);

  value_integer[ephind] = value.intday();
  value_double[ephind] = value.fracday();

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

void psrephem::set_Angle (int ephind, const Angle& value)
{
  if (parmTypes[ephind] != 2 && parmTypes[ephind] != 3)
    throw Error (InvalidParam, "psrephem::get_Angle",
                 "%s is not an  Angle", parmNames[ephind]);

  value_double[ephind] = value.getTurns();

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

void psrephem::set_integer (int ephind, int value)
{
  if (parmTypes[ephind] != 5)
    throw Error (InvalidParam, "psrephem::get_integer",
                 "%s is not an integer", parmNames[ephind]);

  value_integer[ephind] = value;

  if (!parmStatus[ephind])
    parmStatus[ephind] = 1;
}

