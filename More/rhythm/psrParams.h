//-*-C++-*-

/* ///////////////////////////////////////////////////////////////////////
   psrParams --- light-weight object that can store TEMPO parameter set

   Author: Willem van Straten 
   /////////////////////////////////////////////////////////////////////// */

#ifndef __PSRPARAMS_H
#define __PSRPARAMS_H

#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>

#include "Angle.h"
#include "psr_cpp.h"

class psrParameter;  // implementation detail defined in psrParameter.h
class psrephem;      // the psrephem class which this class reflects

class psrParams
{
  friend class qt_psrephem;

 public:
  psrParams () {};
  psrParams (const psrParams& p) { *this = p; }
  psrParams& operator = (const psrParams& p);

  void set_psrephem (const psrephem& eph);
  void get_psrephem (psrephem& eph);

  // static members and methods
  static bool verbose;

 protected:
  vector <psrParameter*> params;    // pulsar parameters
  void destroy ();
};

#endif
