/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Angle.h,v $
   $Revision: 1.5 $
   $Date: 2000/08/21 01:19:56 $
   $Author: redwards $ */

// redwards 17 Mar 99 -- Time for a definitive C++ suite of
// angle and sky coordinate functions

#ifndef ANGLE_H
#define ANGLE_H

#include <string>
#include <iostream>
#include <math.h>
#include "psr_cpp.h"

static const double MilliSecin12Hours = 4.32e7;

class Angle
{

  friend class AnglePair;

 protected:
  double radians;  // angle in radians

 public:

  static bool verbose;

  Angle(const Angle & a) {radians=a.radians;};
  Angle(const double & d = 0.0);

  int     setHMS (const char *);
  char*   getHMS (char* str, int places=3) const;
  string  getHMS (int places = 3) const;
  int     setDMS (const char *);
  char*   getDMS (char* str, int places=3) const;
  string  getDMS (int places = 3) const;

  // ms is given in milliseconds of an hour
  void  setRadMS(long int ms)
    { radians = double(ms) * M_PI / MilliSecin12Hours; };
  long  getRadMS() const
    { return long(radians * MilliSecin12Hours / M_PI); };

  void setDegrees(double deg)
    { radians = deg * M_PI/180.0;};

  double getDegrees() const
    { return radians * 180.0/M_PI; };

  void setradians(double rad)
    { radians = rad; };

  double getradians() const
    { return radians; };

  // Argh some capitalization consistency would be nice.... redwards.
  void setRadians(double rad)
    { radians = rad; };
  double getRadians() const
    { return radians; };
  


  Angle & operator= (const Angle & a);
  Angle & operator= (const double & val);

  Angle & operator+= (const Angle & a);
  Angle & operator-= (const Angle & a);
  Angle & operator+= (const double & d);
  Angle & operator-= (const double & d);
  Angle & operator*= (const double & d);
  Angle & operator/= (const double & d);
  const friend Angle operator + (const Angle &, const Angle &);
  const friend Angle operator - (const Angle &, const Angle &);
  const friend Angle operator + (const Angle &, double);
  const friend Angle operator - (const Angle &, double);
  const friend Angle operator / (const Angle &, double);
  const friend Angle operator * (const Angle &, double);
  friend int operator > (const Angle &, const Angle &);
  friend int operator < (const Angle &, const Angle &);
  friend int operator >= (const Angle &, const Angle &);
  friend int operator <= (const Angle &, const Angle &);
  friend int operator == (const Angle &, const Angle &);
  friend int operator != (const Angle &, const Angle &);

  friend ostream& operator<< (ostream&, const Angle&);
};
 
// AnglePair class : useful for sky positions.
// Sign convention should be as per usual output, that is in general
// angle 1 is 0 to 2pi and angle 2 is -pi to pi
class AnglePair
{
 public:
  Angle angle1, angle2;

  // both HMS and DMS in one string
  int setHMSDMS (const char* coordstr);

  // HMS and DMS in separate strings
  int    setHMSDMS (const char *, const char *);
  void   getHMSDMS (char* s1, char* s2, int places1=3, int places2=2) const;
  string getHMSDMS (int places1 = 3, int places2 = 2) const;

  void   setDegrees (double, double);
  void   getDegrees (double *, double *) const;
  string getDegrees () const;
  void   setRadians (double, double);
  void   getRadians (double *, double *) const;
  string getRadians () const;

  Angle angularSeparation(const AnglePair& other);

  // set the angles in radians from 
  // az and ze in milliseconds of an hour
  void setRadMS(long int , long int );
  void getRadMS(long int*, long int*);

  AnglePair & operator=  (const AnglePair &);
  AnglePair & operator*= (const double);
  AnglePair (const Angle &, const Angle &);
  AnglePair (const double = 0.0, const double = 0.0);

  friend AnglePair operator * (const AnglePair&, const double);

  friend int operator == (const AnglePair &, const AnglePair &);
  friend int operator != (const AnglePair &, const AnglePair &);

  friend ostream& operator<< (ostream&, const AnglePair&);
};


// redwards : trig functions
double sin(const Angle&);
double cos(const Angle&);
double tan(const Angle&);
// Unfortunately can't overload atan as only the return type differs
// from the standard library function
Angle arctan(double);
Angle arctan(double y, double x); // returns atan y/x





#endif //ANGLE_H
