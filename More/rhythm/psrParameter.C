#include "psrParameter.h"
#include "string_utils.h"
#include "ephio.h"

// //////////////////////////////////////////////////////////////////////////
// qt_psrParameter static variables

psrParameter psrParameter::null;
bool psrParameter::verbose = false;

// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////
//
// psrParameter
//
// Abstract base class defines the common methods for each element.
// Derived classes are basically an implementation detail that no code need
// worry about after using the psrParameter::factory to obtain a new instance
// of a derived type.
//
// //////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////

psrParameter::psrParameter (int ephind, bool in_fit, double err)
{
  ephio_index = check (ephind);
  infit = in_fit && fitable();
  error = 0.0;
  if (has_error())
    error = err;
}

// //////////////////////////////////////////////////////////////////////////
// checks that the index given is within the proper limits.
//    returns the value if ok
//    throws a string exception if not
int psrParameter::check (int ephind)
{
  if (ephind < 0 || ephind >= EPH_NUM_KEYS) {
    string err ("psrParameter::range error");
    cerr << err << endl;
    throw ( err );
  }
  return ephind;
}

bool psrParameter::fitable() { return parmTypes[ephio_index] != 0; };
bool psrParameter::has_error() { return parmError[ephio_index] == 1; };


// return the value
string psrParameter::getString ()
{
  string retval;
  psrString* me = dynamic_cast<psrString*>(this);
  if (me)
    retval = me -> getString();

  return retval;
}

double psrParameter::getDouble ()
{
  double retval = 0.0;
  psrDouble* me = dynamic_cast<psrDouble*>(this);
  if (me)
    retval = me -> getDouble();

  return retval;
}

MJD psrParameter::getMJD ()
{
  MJD  retval;
  psrMJD* me = dynamic_cast<psrMJD*>(this);
  if (me)
    retval = me -> getMJD ();

  return retval;
}

Angle psrParameter::getAngle ()
{
  Angle retval;
  psrAngle* me = dynamic_cast<psrAngle*>(this);
  if (me)
    retval = me -> getAngle();

  return retval;
}


// set value
void psrParameter::setString (const string& val)
{
  psrString* me = dynamic_cast<psrString*>(this);
  if (me)
    me -> setString(val);
}

void psrParameter::setDouble (double val)
{
  psrDouble* me = dynamic_cast<psrDouble*>(this);
  if (me)
    me -> getDouble();
}

void psrParameter::setMJD (const MJD& val)
{
  psrMJD* me = dynamic_cast<psrMJD*>(this);
  if (me)
    me -> setMJD(val);
}

void psrParameter::setAngle (const Angle& val)
{
  psrAngle* me = dynamic_cast<psrAngle*>(this);
  if (me)
    me -> setAngle(val);
}

void psrParameter::unload (string* str)
{
  static const int buflen = 80;
  static char* buffer = NULL;
  static string newline ("\n");

  if (ephio_index < 0 || ephio_index >= EPH_NUM_KEYS)
    return;

  if (buffer == NULL)
    buffer = new char [buflen];

  int    parmStatus = getFit() ? 2 : 1;
  char   value_str [EPH_STR_LEN];
  double value   = 0.0;
  int    integer = 0;

  MJD   mjd;

  switch ( parmTypes[ephio_index] ) {

  case 0:  // other strings (JNAME, TZRSITE, etc.)
    if (verbose)
      cerr << "psrParameter::unload String:" << parmNames[ephio_index];
    strncpy (value_str, dynamic_cast<psrString*>(this) -> getString().c_str(),
	     EPH_STR_LEN);
    break;
    
  case 1:  // any double
    if (verbose)
      cerr << "psrParameter::unload Double:" << parmNames[ephio_index];
    value = dynamic_cast<psrDouble*>(this) -> getDouble();
    break;

  case 2:  // RAs
  case 3:  // DECs
    if (verbose)
      cerr << "psrParameter::unload Angle :" << parmNames[ephio_index];
    value = dynamic_cast<psrAngle*>(this) 
      -> getAngle().getradians() / (2.0 * M_PI);
    break;

  case 4:  // MJDs
    if (verbose)
      cerr << "psrParameter::unload MJD   :" << parmNames[ephio_index];
    mjd = dynamic_cast<psrMJD*>(this) -> getMJD();
    integer = mjd.intday();
    value   = mjd.fracday();
    break;

  default:
    return;
  }

  wr_eph_str (buffer, buflen, ephio_index, parmStatus, 
	      value_str, value, integer, error);

  if (verbose)
    cerr << "   '" << buffer << "'" << endl;
  *str += buffer + newline;
}

psrParameter* psrParameter::duplicate ()
{
  if (ephio_index < 0 || ephio_index >= EPH_NUM_KEYS)
    return NULL;

  switch ( parmTypes[ephio_index] ) {

  case 0:  // other strings (JNAME, TZRSITE, etc.)
    return new psrString ( *dynamic_cast<psrString*>(this) );

  case 1:  // any double
    return new psrDouble ( *dynamic_cast<psrDouble*>(this) );

  case 2:  // RAs
  case 3:  // DECs
    return new psrAngle ( *dynamic_cast<psrAngle*>(this) );

  case 4:  // MJDs
    return new psrMJD ( *dynamic_cast<psrMJD*>(this) );

  default:
    return NULL;
  }
}

bool psrParameter::equal (psrParameter* p1, psrParameter* p2)
{
  if ( (p1 -> ephio_index != p2 -> ephio_index) ||
       (p1 -> error != p2 -> error) || (p1 -> infit != p2 -> infit) )
    return false;

  if (p1 -> ephio_index < 0 || p1 -> ephio_index >= EPH_NUM_KEYS)
    return true;

  switch ( parmTypes[p1 -> ephio_index] ) {

  case 0:  // other strings (JNAME, TZRSITE, etc.)
    return dynamic_cast<psrString*>(p1) -> getString()
      == dynamic_cast<psrString*>(p2) -> getString();

  case 1:  // any double
    return dynamic_cast<psrDouble*>(p1) -> getDouble() 
      == dynamic_cast<psrDouble*>(p2) -> getDouble();

  case 2:  // RAs
  case 3:  // DECs
    return dynamic_cast<psrAngle*>(p1) -> getAngle() 
      == dynamic_cast<psrAngle*>(p2) -> getAngle();

  case 4:  // MJDs
    return dynamic_cast<psrMJD*>(p1) -> getMJD()
      == dynamic_cast<psrMJD*>(p2) -> getMJD();

  default:
    return false;
  }
}


