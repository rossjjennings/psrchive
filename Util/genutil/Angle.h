//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Angle.h,v $
   $Revision: 1.14 $
   $Date: 2004/07/06 14:57:54 $
   $Author: straten $ */

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
  double wrap_point;
  virtual void wrap(); // make sure angle is +- pi radians
  void init() {radians=0.0;wrap_point=M_PI;}
 public:

  static bool verbose;

  Angle (const Angle& a)  
    {init(); radians=a.radians;wrap_point=a.wrap_point;}
  Angle (const double& rad = 0);

  void setWrapPoint(double wp){wrap_point=wp; wrap();}
  double getWrapPoint() {return wrap_point;}

  // WvS - this not only changes the wrap-point but also changes the scale
  // void makeArctangle() { setWrapPoint(0.5*M_PI); }

  void setHMS (int hours, int minutes, double seconds);
  void getHMS (int& hours, int& minutes, double& seconds) const;

  int     setHMS (const char *);
  char*   getHMS (char* str, int places=3) const;
  string  getHMS (int places = 3) const;

  void setDMS (int degrees, int minutes, double seconds);
  void getDMS (int& degrees, int& minutes, double& seconds) const;

  int     setDMS (const char *);
  char*   getDMS (char* str, int places=3) const;
  string  getDMS (int places = 3) const;

  // ms is given in milliseconds of an hour
  void  setRadMS(long int ms)
    { radians = double(ms) * M_PI / MilliSecin12Hours; };
  long  getRadMS() const
    { return long(radians * MilliSecin12Hours / M_PI); };

  void setDegrees(double deg)
    { //fprintf(stderr, "%f %f %p\n", wrap_point, deg, this);
      radians = deg * M_PI/180.0; wrap();};

  double getDegrees() const
    { return radians * 180.0/M_PI; };

  void setradians(double rad)
    { radians = rad; wrap();};

  double getradians() const
    { return radians; };

  // Argh some capitalization consistency would be nice.... redwards.
  void setRadians(double rad)
    { setradians(rad); };
  double getRadians() const
    { return radians; };
  


  Angle & operator= (const Angle & a);
  Angle & operator= (const double &val);

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
  const friend Angle operator - (const Angle &);

  friend double operator * (const Angle &, const Angle &);

  friend int operator > (const Angle &, const Angle &);
  friend int operator < (const Angle &, const Angle &);
  friend int operator >= (const Angle &, const Angle &);
  friend int operator <= (const Angle &, const Angle &);
  friend int operator == (const Angle &, const Angle &);
  friend int operator != (const Angle &, const Angle &);

  inline friend double cast_double(const Angle &a) {return a.radians;}
};

inline ostream& operator<< (ostream & os, const Angle & angle)
{  return os << angle.getradians(); }

// More Arctangle stuff... NOTE: remember the Angle = operator
// DOESN'T set wrap_point, so only use these to e.g. pass directly
// to a function
//inline Angle Arctangle() {Angle a; a.makeArctangle(); return a;}
//inline Angle Arctangle(const Angle&a1) {Angle a(a1); a.makeArctangle(); return a;}
//inline Angle Arctangle(double rad) {Angle a(rad); a.makeArctangle(); return a;}

 
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
  AnglePair (const char* astr);

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

