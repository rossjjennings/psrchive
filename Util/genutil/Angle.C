// redwards 17 Mar 99 -- Time for a definitive C++ suite of
// angle and sky coordinate functions

#include "Angle.h"
#include "Cartesian.h"
#include "coord_parse.h"

#include <string>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <limits.h>
#include <float.h>

bool Angle::verbose = false;
Angle::Type Angle::default_type = Angle::Radians;

Angle::Angle (const double & rad) { init(); setradians(rad); }

void Angle::wrap()
{
  //while(radians>wrap_point) radians-=2.0*wrap_point;
  //while(radians<-wrap_point) radians+=2.0*wrap_point;

  // first cut the radian down to size

  while (fabs(radians) > 4.0*M_PI)  {
    double irad = floor(0.5*radians/M_PI);
    irad *= 2.0*M_PI;
    radians -= irad;
  }

  while (radians>wrap_point) radians-=2.0*M_PI;
  while (radians<wrap_point-2.0*M_PI) radians+=2.0*M_PI;
}

char * Angle::getHMS(char *str, int places) const
{
  int hours, minutes;
  double seconds;

  getHMS (hours, minutes, seconds);

  sprintf(str, "%02d:%02d:%0*.*f", hours, minutes, 
	  3+places, places, seconds);

  return str;
}

static char angle_str [100];
string Angle::getHMS (int places) const
{
  ra2str (angle_str, 100, radians, places);
  return angle_str;
}

int Angle::setHMS(const char *str)
{
  return str2ra(&radians, str);
}

int Angle::setDMS(const char *str)
{
  return str2dec2(&radians, str);
}

void Angle::setHMS (int hours, int minutes, double seconds)
{
  radians = M_PI * ( hours/12.0 + minutes/720.0 + seconds/43200.0 );
}

void Angle::getHMS (int& hours, int& minutes, double& seconds) const
{
  double posradians = radians;
  if (posradians < 0.0)
     posradians += 2.0*M_PI;

  hours = (int)floor(posradians * 12.0/M_PI); 
  minutes = (int)floor(posradians * 720.0/M_PI) % 60;
  seconds = posradians * 43200.0/M_PI - ((hours*60.0)+minutes)*60.0;
}

void Angle::setDMS (int degrees, int minutes, double seconds)
{
  radians = M_PI * ( degrees/180.0 + minutes/720.0 + seconds/43200.0 );
}

void Angle::getDMS (int& degrees, int& minutes, double& seconds) const
{
  degrees = (int)floor(fabs(radians) * 180.0/M_PI); 
  minutes = (int)floor(fabs(radians) * 10800.0/M_PI) % 60;
  seconds = fabs(radians) * 648000.0/M_PI - ((degrees*60.0)+minutes)*60.0;
}

char * Angle::getDMS(char *str,int places) const
{
  int degrees, minutes;
  double seconds;

  getDMS (degrees, minutes, seconds);

  if (radians < 0.0)
    sprintf(str, "-%02d:%02d:%0*.*f", degrees, minutes, 
	  3+places, places, seconds);
  else 
    sprintf(str, "+%02d:%02d:%0*.*f", degrees, minutes, 
	    3+places, places, seconds);

  return str;
}

string Angle::getDMS(int places) const
{
  dec2str2 (angle_str, 100, radians, places);
  return angle_str;
}


Angle& Angle::operator= (const Angle & a)
{
  radians = a.radians;
  wrap();
  return *this;
}

Angle& Angle::operator= (const double &rad){
  setradians(rad);
  return *this;
}

Angle& Angle::operator+= (const Angle & a1)
{
  *this = *this + a1;
  return(*this);
}

Angle& Angle::operator-= (const Angle & a1)
{
  *this = *this - a1;
  return(*this);
}

Angle& Angle::operator+= (const double & d)
{
  *this = *this + d;
  return(*this);
}

Angle& Angle::operator-= (const double & d)
{
  *this = *this - d;
  return(*this);
}

Angle& Angle::operator*= (const double & d)
{
  *this = *this * d;
  return(*this);
}

Angle& Angle::operator/= (const double & d)
{
  *this = *this / d;
  return(*this);
}

const Angle operator + (const Angle &a1, const Angle &a2) {

  // what is this for ?? I think I mistakenly thought it was
  // necessary to add in cartesian space.. only need this for means
  //  double real = cos(a1.getradians())*cos(a2.getradians()) - 
  //   sin(a1.getradians())*sin(a2.getradians());
  //  double imag = cos(a1.getradians())*sin(a2.getradians()) +
  ///   sin(a1.getradians())*cos(a2.getradians());
  //  return Angle(atan2(imag,real));
  Angle a;
  a.setWrapPoint((a1.wrap_point > a2.wrap_point ? 
		 a1.wrap_point: a2.wrap_point));
  a.setradians(a1.radians+a2.radians);
  return a;
}

// returns the negative
const Angle operator - (const Angle& a)
{
  Angle ret(a);
  ret.setradians(-a.radians);
  return ret;
}

const Angle operator - (const Angle &a1, const Angle &a2) {
  Angle a3 = a2*-1.0;
  return (a1+a3);
}

const Angle operator + (const Angle &a1, double d) {
  Angle a(a1);
  a.setradians(a.radians+d);
  return a;
}

const Angle operator - (const Angle &a1, double d) {
  Angle a(a1);
  a.setradians(a.radians-d);
  return a;
}

const Angle operator * (const Angle &a1, double d) {
  Angle a(a1);
  a.setradians(a.radians*d);
  return a;
}

// NOTE : this returns double, not angle, since angle^2 is not
// limited to <pi radians as angle is
// it returns the product of the angles.radians
double operator * (const Angle &a1, const Angle &a2) {
  return a1.getradians()*a2.getradians();
}

const Angle operator / (const Angle &a1, double d) {
  Angle a(a1);
  a.setradians(a.radians/d);
  return a;
  
}

int operator > (const Angle &a1, const Angle &a2) {
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if(fabs(a1.getradians()-a2.getradians())<precision_limit) return(0);
  else return (a1.getradians()>a2.getradians());
}

int operator >= (const Angle &a1, const Angle &a2) {
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if(fabs(a1.getradians()-a2.getradians())<precision_limit) return(1);
  else return (a1.getradians()>a2.getradians());
}

int operator < (const Angle &a1, const Angle &a2) {
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if(fabs(a1.getradians()-a2.getradians())<precision_limit) return(0);
  else return (a1.getradians()<a2.getradians());
}

int operator <= (const Angle &a1, const Angle &a2) {
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if(fabs(a1.getradians()-a2.getradians())<precision_limit) return(1);
  else return (a1.getradians()<a2.getradians());
}

int operator == (const Angle &a1, const Angle &a2){
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if ((fabs(a1.getradians()-a2.getradians())<precision_limit)) 
      return (1);
  else
      return (0);  
}

int operator != (const Angle &a1, const Angle &a2){
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if ((fabs(a1.getradians()-a2.getradians())>precision_limit)) 
      return (1);
  else
      return (0);  
}



ostream& operator << (ostream& os, const Angle& angle)
{
  switch (Angle::default_type) {
  case Angle::Radians:
    // the previous default operation
    return os << angle.getradians();
  case Angle::Degrees:
    return os << angle.getDegrees() << "deg";
  case Angle::Turns:
    return os << angle.getTurns() << "trn";
  }
}

istream& operator >> (istream& is, Angle& angle)
{
  double value;
  is >> value;
  std::streampos pos = is.tellg();

  string unit;
  is >> unit;
  if (unit == "deg")
    angle.setDegrees (value);
  else if (unit == "rad")
    angle.setRadians (value);
  else if (unit == "trn")
    angle.setTurns (value);
  else {
    // replace the text
    is.seekg (pos);
    switch (Angle::default_type) {
    case Angle::Radians:
      angle.setRadians (value); break;
    case Angle::Degrees:
      angle.setDegrees(value); break;
    case Angle::Turns:
      angle.setTurns(value); break;
    }
  }

  return is;
}





// AnglePair stuff for convenience

AnglePair::AnglePair(const Angle & a1, const Angle & a2){
  angle1 = a1;
  angle2 = a2;
}

AnglePair::AnglePair(const double d1, const double d2){
  angle1 = d1;
  angle2 = d2;
}

AnglePair::AnglePair (const char *astr)
{ 
  if (str2coord (&angle1.radians, &angle2.radians, astr) < 0) {
    string error ("AnglePair::AnglePair str2coord failure");
    cerr << error;
    throw error;
  }
}

int AnglePair::setHMSDMS(const char *astr)
{ 
  return str2coord (&angle1.radians, &angle2.radians, astr);
}

int AnglePair::setHMSDMS(const char *s1, const char *s2)
{
  if (angle1.setHMS(s1) < 0)
    return -1;
  return angle2.setDMS(s2);
}

void AnglePair::getHMSDMS(char *s1, char *s2, int places1, int places2) const
{
  s1 = angle1.getHMS(s1,places1);
  s2 = angle2.getDMS(s2,places2);
}

string
AnglePair::getHMSDMS(int places1, int places2) const
{ 
  return angle1.getHMS(places1) + ' ' + angle2.getDMS(places2);
}

void 
AnglePair::setDegrees(double d1, double d2)
{
  angle1.setDegrees(d1);
  angle2.setDegrees(d2);
}

void 
AnglePair::getDegrees(double *d1, double *d2) const
{
  *d1 = angle1.getDegrees();
  *d2 = angle2.getDegrees();
}

string AnglePair::getDegrees() const
{
  sprintf (angle_str, "(%8.4f,%8.4f)",
	   angle1.getDegrees(), angle2.getDegrees());
  return string (angle_str);
}

void 
AnglePair:: setRadians(double r1, double r2)
{
  angle1.setradians(r1);
  angle2.setradians(r2);
}

void 
AnglePair:: getRadians(double *r1, double *r2) const
{
  *r1 = angle1.getradians();
  *r2 = angle2.getradians();
}

string AnglePair::getRadians() const
{
  sprintf(angle_str,
	  "(%8.4f,%8.4f)", angle1.getradians(),angle2.getradians());
  return string (angle_str);
}

void
AnglePair:: setRadMS(long int az, long int ze)
{
  angle1.setRadMS(az);
  angle2.setRadMS(ze);
}

void
AnglePair:: getRadMS(long int *az, long int *ze)
{
  *az = angle1.getRadMS();
  *ze = angle2.getRadMS();
}


// redwards 10Aug99 function to compute angular separation of a pair
// of spherical coordinates
// straten 05Oct99 got rid of references to slalib
Angle AnglePair::angularSeparation(const AnglePair& other) const
{
  // convert the angles in spherical coordinates to unit Cartesian vectors
  Cartesian u1 (*this);
  Cartesian u2 (other);

  return Cartesian::angularSeparation (u1, u2);
}

AnglePair & AnglePair::operator*= (const double mult){
  angle1 *= mult;
  angle2 *= mult;
  return *this;
}

AnglePair operator* (const AnglePair & pair, const double mult){
  return AnglePair(pair.angle1*mult, pair.angle2*mult);
}

AnglePair &
AnglePair::operator= (const AnglePair & a)
{
  angle1 = a.angle1;
  angle2 = a.angle2;

  return *this;
}

ostream& operator<< (ostream & os, const AnglePair & pair)
{
  return os << "(" << pair.angle1 
	    << "," << pair.angle2 << ")";
}

int operator == (const AnglePair &a1, const AnglePair &a2){
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  double ra1, ra2, dec1, dec2;
  a1.getRadians(&ra1, &dec1);
  a2.getRadians(&ra2, &dec2);
  if (fabs(ra1-ra2)<precision_limit && fabs(dec1-dec2)<precision_limit) 
      return (1);
  else
      return (0);  
}

int operator != (const AnglePair &a1, const AnglePair &a2){
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  double ra1, ra2, dec1, dec2;
  a1.getRadians(&ra1, &dec1);
  a2.getRadians(&ra2, &dec2);
  if (fabs(ra1-ra2)>precision_limit || fabs(dec1-dec2)>precision_limit)
      return (1);
  else
      return (0);  
}

// redwards : trig functions. Not members since we are accustomed to
// calling normal functions for these
double sin(const Angle& a) { return sin(a.getRadians());}
double cos(const Angle& a) { return cos(a.getRadians());}
double tan(const Angle& a) { return tan(a.getRadians());}
Angle arctan(double g) { return Angle(atan(g));}
Angle arctan(double y, double x) { return Angle(atan2(y,x));}
