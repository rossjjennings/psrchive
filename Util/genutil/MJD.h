//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 1998 by Matthew Britton
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/MJD.h

#ifndef __GENUTIL_MJD_H
#define __GENUTIL_MJD_H

#include <inttypes.h>
#include "utc.h"

#include <iostream>
#include <limits>
#include <string>
#include <stdio.h>

//! Class for representing modified Julian dates with high precision.
class MJD {

 private:
  int    days;
  int    secs;
  double fracsec;

  void add_day (double dd);
  void settle();

 public:
  static const MJD zero;

  static double   precision;
  static unsigned ostream_precision;

  static int verbose;

  //! null constructor
  MJD () { days=0;secs=0;fracsec=0.0; };

  //! construct from standard C types
  MJD (double dd);                        // mjd given as day
  MJD (int intday, double fracday);       // mjd given with separate fracday
  MJD (double dd, double ss, double fs);  // mjd with seconds and fracsec
  MJD (int dd, int ss, double fs);        // again

  //! construct from a C string of the form "50312.4569"
  MJD (const char* mjdstring);

  //! some standard C time formats
  MJD (time_t time);                // returned by time()
  MJD (const struct tm& greg);      // returned by gmtime()
  MJD (const struct timeval& tp);   // returned by gettimeofday()

  //! simple little struct invented in S2 days
  MJD (const utc_t& utc);

  //! construct from a C++ string
  MJD (const std::string& mjd);

  //! construct from a string of the form 51298.45034 ish
  int Construct (const char* mjdstr);

  //! construct from the unix time_t
  int Construct (time_t time);

  //! construct from the unix struct tm
  int Construct (const struct tm& greg);

  //! construct from another UNIX time standard
  int Construct (const struct timeval& tp);

  //! constructs from the home-grown utc_t
  int Construct (const utc_t& utc);

  //! construct from a BAT (binary atomic time), ie MJD in microseconds
  /*! @param bat time stored in two 32 bit unsigned integers, as returned by the AT clock */
  int Construct (uint64_t bat);

  double in_seconds() const;
  double in_days()    const;
  double in_minutes() const;

  // cast into other forms
  int UTC       (utc_t* utc, double* fsec=NULL) const;
  int gregorian (struct tm* gregdate, double* fsec=NULL) const;

  MJD & operator = (const MJD &);
  MJD & operator += (const MJD &);
  MJD & operator -= (const MJD &);
  MJD & operator += (const double &);
  MJD & operator -= (const double &);
  MJD & operator *= (const double &);
  MJD & operator /= (const double &);
  friend const MJD operator + (const MJD &, const MJD &);
  friend const MJD operator - (const MJD &, const MJD &);
  friend const MJD operator + (const MJD &, double);  // Add seconds to an MJD
  friend const MJD operator - (const MJD &, double);  // Take seconds from MJD
  friend const MJD operator * (const MJD &, double);  
  friend const MJD operator * (double a, const MJD& m) { return m * a; }
  friend const MJD operator / (const MJD &, double);  

  // return the -ve of m
  friend const MJD operator - (MJD m);

  friend int operator > (const MJD &, const MJD &);
  friend int operator < (const MJD &, const MJD &);
  friend int operator >= (const MJD &, const MJD &);
  friend int operator <= (const MJD &, const MJD &);
  friend int operator == (const MJD &, const MJD &);
  friend int operator != (const MJD &, const MJD &);

  //! return plotable value to xyplot template class
  float plotval() const { return float (in_days()); };
  
  // These bits are useful for tempo
  int    intday()  const;                // To access the integer day
  double fracday() const;                // To access fractional day
  int    get_secs() const {return(secs);};
  double get_fracsec() const {return(fracsec);};

  //! return Local Sidereal Time in hours
  /*! @param (longitude given in degrees East of Greenwich) */
  double LST (double longitude) const;

  //! return Greenwich Mean Sidereal Time in radians
  double GMST () const;
  
  std::string printdays (unsigned precision) const;

  int print (FILE *stream);
  int println (FILE *stream);
  std::string printall()    const;
  std::string printdays()   const;
  std::string printhhmmss() const;
  std::string printfs()     const;
  std::string strtempo() const; 

  //! return a string formatted by strftime
  /*! @param format as used by strftime; e.g. "%Y-%m-%d %H:%M:%S" */
  char* datestr (char* dstr, int len, const char* format) const;

  //! Convenient interface to datestr, with optional fractional seconds
  std::string datestr (const char* format, unsigned fractional_second_digits = 0) const;

  //! Parse MJD from the given datestr using strptime and the specified format
  void from_datestr (const std::string& datestr, const std::string& format = "%Y-%m-%d-%H:%M:%S");

 protected:
  friend bool equal (const MJD &m1, const MJD &m2);

};

inline double cast_double(const MJD&m) {return m.in_days();}

std::ostream& operator<< (std::ostream& ostr, const MJD& mjd);
std::istream& operator>> (std::istream& istr, MJD& mjd);

// Enable use of the MJD class with std C++ numeric_limits traits
namespace std {
  template<>
  class numeric_limits<MJD> {
    public:
    static const int digits10 = 15;
  };
}

#endif  /* not __MJD_H defined */

