#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

#include "psrParameter.h"
#include "psrParams.h"
#include "string_utils.h"
#include "ephio.h"

// //////////////////////////////////////////////////////////////////////////
//
// STATIC MEMBERS
//
// //////////////////////////////////////////////////////////////////////////

bool  psrParams::verbose = false;
char* psrParams::tempo_pardir = NULL;

// defines the recognized filename extensions used for pulsar ephemeris files
vector<string> psrParams::extensions ()
{
  vector <string> retval;
  
  retval.push_back (string (".eph"));
  retval.push_back (string (".par"));
  
  return retval;
}

string psrParams::par_lookup (const char* name, bool use_cwd)
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
  char* tempo_cfg = "/tempo.cfg";
  char* psrinfo_cmd = "psrinfo -e ";
  
  if (use_cwd) {
    vector <string> exts = extensions ();
    for (unsigned iext=0; iext < exts.size(); iext++) {
      /* Look for jname.ext in current directory */
      filename = cwd + psr_name + exts[iext];
      if (stat (filename.c_str(), &finfo) == 0) {
	if (verbose)
	  cerr << "psrParams::Using '" << filename << "' from cwd." << endl;
	return filename;
      }
    }
  }
  
  if (tempo_pardir == NULL) {
    /* Find PARDIR - the TEMPO directory for name.par files */
    char* tpodir = (char *) getenv("TEMPO");
    if (tpodir == NULL) {
      cerr << "psrParams::TEMPO environment variable not defined" << endl;
    }
    else {
      filename = tpodir;
      filename += tempo_cfg;
      FILE* fptr = fopen(filename.c_str(),"r");
      if (fptr == NULL) {
	cerr << "psrParams::par_lookup error fopen(" << filename << ")";
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
	    cerr << "psrParams:: PARDIR not defined in $TEMPO/tempo.cfg"
		 << endl;
	}
      } // else if $TEMPO/tempo.cfg file opened successfully
    } // else if $TEMPO environment variable is defined
  } // end if tempo_pardir == NULL
  
  if (tempo_pardir != NULL) {
    filename = tempo_pardir + psr_name + ".par";
    if (stat (filename.c_str(), &finfo) == 0) {
      if (verbose)
	cerr << "psrParams:: Using '" << filename << "' from PARDIR" << endl;
      return filename;
    }
  }
  
  /* Create name.eph in local directory */ 
  
  filename = psrinfo_cmd + psr_name;
  if (verbose)
    cerr << "psrParams:: Creating " 
	 << psr_name << ".eph using psrinfo." << endl;

  if (system(filename.c_str()) != 0) {
    cerr << "psrParams:: Error executing system (" << filename << ")" << endl;
    filename.erase();
    return filename;
  }
  filename = cwd + psr_name + ".eph";
  if (stat (filename.c_str(), &finfo) == 0) {
    if (verbose)
      cerr << "psrParams:: Using '" << filename << "' from PARDIR" << endl;
    return filename;
  }
  
  if (verbose)
    cerr << "psrParams:: Cannot find '"
	 << filename << "' after call to psrinfo." << endl; 
  
  filename.erase();
  return filename;
}

// //////////////////////////////////////////////////////////////////////////
//
// CONSTRUCTORS / DESTRUCTORS
//
// //////////////////////////////////////////////////////////////////////////

void psrParams::create (const char* psr_name, bool use_cwd)
{
  string filename = par_lookup (psr_name, use_cwd);
  if (filename.empty()) {
    string error ("psrParams::create no ephemeris file found for ");
    error += psr_name;
    cerr << error << endl;
    throw (error);
  }
  load (filename);
}

void psrParams::load (const char* filename)
{
  ifstream istr (filename);
  load (istr);
}

void psrParams::load (istream &istr, size_t nbytes)
{
  string total;
  if (stringload (&total, istr, nbytes) < 0)  {
    string error ("psrParams::load ERROR");
    cerr << error << endl;
    throw (error);
  }
  load (&total);
}

void psrParams::load (FILE* fptr, size_t nbytes)
{
  string total;
  if (stringload (&total, fptr, nbytes) < 0)  {
    string error ("psrParams::load ERROR");
    cerr << error << endl;
    throw (error);
  }
  load (&total);
}

static int*    parmStatus    = NULL;
static double* value_double  = NULL;
static int*    value_integer = NULL;
static double* error_double  = NULL;
static int*    correct       = NULL;
static char*   value_str     = NULL;
static int     isOldEphem = 0;

void prepare_static_load_area ()
{
  if (parmStatus == NULL) {
    parmStatus    = new int    [EPH_NUM_KEYS];
    value_double  = new double [EPH_NUM_KEYS];
    value_integer = new int    [EPH_NUM_KEYS];
    error_double  = new double [EPH_NUM_KEYS];
    correct       = new int    [EPH_NUM_KEYS];
  }
  value_str = new char [EPH_NUM_KEYS * EPH_STR_LEN];
  for (int i=0; i<EPH_NUM_KEYS; i++) {
    parmStatus   [i] = 0;
    value_double [i] = 0.0;
    value_integer[i] = 0;
    error_double [i] = 0.0;
    correct      [i] = 0;
  }
  for (int c=0; c<EPH_NUM_KEYS*EPH_STR_LEN; c++)
    value_str[c] = ' ';

  isOldEphem = 0;
}

void psrParams::load (string* instr)
{
  if (verbose)
    cerr << "psrParams::load *****" << endl << *instr << "***** END" << endl;

  prepare_static_load_area ();
  while (instr -> length() > EPH_STR_LEN) {

    // get the next line from the incoming text
    string line ( stringtok (instr, "\n") );
    if (verbose)
      cerr << "psrParams::load '" << line << "'" << endl;

    if (line.length() < 1)
      continue;

    // cut out the first string up to newline character
    rd_eph_str (parmStatus, value_str, value_double, value_integer,
		error_double, correct, &isOldEphem, 
		const_cast<char*>( line.c_str() ));
  }

  // convertUnits_ defined in ephio.f
  convertunits_ (value_double, error_double, parmStatus, correct);
  if (verbose)
    cerr << "psrParams::load units converted" << endl;

  // remove any old psrParameters
  destroy ();

  // load up the new ones
  if (verbose)
    cerr << "psrParams::loading new points" << endl;

  for (int ieph=0; ieph < EPH_NUM_KEYS; ieph++) {

    int length = 0;
    char* strval = NULL;
    psrParameter* newparm = NULL;

    if (parmStatus[ieph] == 0)
      continue;


    // parmTypes defined in ephio.h
    switch (parmTypes[ieph]) {

    case 0:  // string value
      if (verbose)
	cerr << "psrParams::load new String:" << parmNames[ieph] << endl;
      // length_ defined in ephio.f
      strval = value_str + ieph * EPH_STR_LEN;
      length = length_ (strval, EPH_STR_LEN);
      strval[length] = '\0';
  
      newparm = new psrString (ieph, strval);
      break;

    case 1:  // double
      if (verbose)
	cerr << "psrParams::load new Double:" << parmNames[ieph] << endl;
      newparm = new psrDouble (ieph, value_double[ieph], error_double[ieph]);
      break;

    case 2:  // RAs
    case 3:  // DECs
      if (verbose)
	cerr << "psrParams::load new Angle :" << parmNames[ieph] << endl;
      newparm = new psrAngle (ieph, value_double[ieph], error_double[ieph]);
      break;

    case 4:  // MJDs
      if (verbose)
	cerr << "psrParams::load new MJD   :" << parmNames[ieph] << endl;
      newparm = new psrMJD (ieph, value_integer[ieph], value_double[ieph],
			    error_double[ieph]);
      break;
    }

    if (newparm != NULL) {
      newparm -> setFit (parmStatus[ieph] == 2);
      params.push_back (newparm);
    }
  }
}

void psrParams::unload (const char* filename) const
{
  ofstream ostr (filename);
  unload (ostr);
}

void psrParams::unload (FILE* fptr) const
{
  string text;
  unload (&text);
  fprintf (fptr, text.c_str());
}

void psrParams::unload (ostream& ostr) const
{
  string text;
  unload (&text);
  ostr << text;
}

void psrParams::unload (string* str) const
{
  for (unsigned iparm=0; iparm < params.size(); iparm++)
    params[iparm] -> unload( str );
}

int psrParams::index (int eph_index) const
{
  for (unsigned iparm=0; iparm < params.size(); iparm++)
    if (params[iparm] -> getEphind() == eph_index)
      return iparm;
  return -1;
}

psrParameter* psrParams::element (int eph_index) const
{
  int iparm = index (eph_index);
  if (iparm < 0)
    return NULL;
  else
    return const_cast<psrParameter*> (params[iparm]);
}

const psrParameter& psrParams::operator [] (int eph_index) const
{
  int iparm = index (eph_index);
  if (iparm < 0)
    return psrParameter::null;
  else
    return *(params[iparm]);
}

string psrParams::psrname() const
{
  psrParameter* el = element (EPH_PSRJ);
  if (el)
    return el -> getString();
  el = element (EPH_PSRB);
  if (el)
    return el -> getString();

  if (verbose)
    cerr << "psrParams::psrname() pulsar name not specified." << endl;

  return string();
}

double psrParams::dm() const
{
  psrParameter* el = element (EPH_DM);
  if (el)
    return el -> getDouble();

  if (verbose)
    cerr << "psrParams::dm() DM not specified." << endl;

  return -1.0;
}

Angle psrParams::jra() const
{
  psrParameter* el = element (EPH_RAJ);
  if (el)
    return el -> getAngle();

  string error ("psrParams::jra() JRA not specified.");
  if (verbose)
    cerr << error << endl;
  throw error;
}

Angle psrParams::jdec() const
{
  psrParameter* el = element (EPH_DECJ);
  if (el)
    return el -> getDouble();
  
  string error ("psrParams::jdec() JDEC not specified.");
  if (verbose)
    cerr << error << endl;
  throw error;  
}

double psrParams::p() const
{
  psrParameter* el = element (EPH_F);
  if (el) {
    double freq = el -> getDouble();
    if (freq != 0.0)
      return 1.0 / freq;
  }
  if (verbose)
    cerr << "psrParams::p() rotation frequency not specified." << endl;
  return -1.0;
}

double psrParams::p_err() const
{
  psrParameter* el = element (EPH_F);
  if (el) {
    double freq = el -> getDouble();
    double err  = el -> getError ();

    if (freq != 0.0)
      return -1.0 / freq / freq * err;
  }
  if (verbose)
    cerr << "psrParams::p_err() rotation frequency not specified." << endl;
  return -1.0;
}

void psrParams::nofit()
{
  for (unsigned iparm=0; iparm < params.size(); iparm++)
    params[iparm] -> setFit (false);
}

void psrParams::fitall()
{
  for (unsigned iparm=0; iparm < params.size(); iparm++)
    params[iparm] -> setFit (true);
}

psrParams::psrParams (const psrParams & p)
{
  *this = p;
}

psrParams& psrParams::operator = (const psrParams & p)
{
  if (this != &p) {
    destroy();

    for (unsigned iparm=0; iparm < p.params.size(); iparm++)
      params.push_back (p.params[iparm] -> duplicate());
  }
  return *this;
}

bool operator == (const psrParams &e1, const psrParams &e2)
{
  if (e1.params.size() != e2.params.size())
    return 0;

  for (unsigned iparm=0; iparm < e1.params.size(); iparm++) {
    int ind = e2.index ( e1.params[iparm] -> getEphind() );
    if (ind < 0)
      return 0;

    if (! psrParameter::equal( e1.params[iparm], e2.params[ind] ))
      return 0;
  }

  return 1;
}

bool operator != (const psrParams &e1, const psrParams &e2)
{ 
  return !(e1==e2);
}

void psrParams::destroy ()
{
  for (unsigned iparm=0; iparm < params.size(); iparm++)
    delete params[iparm];
  params.erase( params.begin(), params.end() );
}

void psrParams::zero()
{
  /*
  for (unsigned iparm=0; iparm < params.size(); iparm++)
    params[iparm] -> zero();
    */
  cerr << "psrParams::zero() not implemented." << endl;
}
