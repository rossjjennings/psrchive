
/* ///////////////////////////////////////////////////////////////////////
   psrParams --- object that can read/write/manipulate TEMPO parameter set

   Author: Willem van Straten 
   /////////////////////////////////////////////////////////////////////// */

#ifndef __QT_PSRPARAMS_H
#define __QT_PSRPARAMS_H

#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>

#include "psr_cpp.h"

class psrParameter;  // implementation detail defined in psrParameter.h

class psrParams
{
  friend class qt_psrParams;

 public:
  psrParams () {};
  psrParams (const psrParams &);

  psrParams (const string& psr_name, int use_cwd)
    { create (psr_name, use_cwd); };
  psrParams (const char* psr_name, int use_cwd)
    { create (psr_name, use_cwd); };
  psrParams (const string& filename)
    { load (filename); };
  psrParams (const char* filename)
    { load (filename); };

  psrParams& operator = (const psrParams &);

  ~psrParams () { destroy (); };

  // string class "wrapper" functions
  void create (const string& psr_name, bool use_cwd)
    { create (psr_name.c_str(), use_cwd); };
  void load   (const string& filename)
    { load (filename.c_str()); };
  void unload (const string& filename) const
    { unload (filename.c_str()); };

  // Create TEMPO parameters, given the pulsar name
  void create (const char* psr_name, bool use_cwd);
 
  // Load TEMPO parameters from file
  void load   (const char* filename);
  void unload (const char* filename) const;

  void load   (istream &istr, size_t nbytes = 0);
  void unload (ostream &ostr) const;

  void load   (FILE* instream, size_t nbytes = 0);
  void unload (FILE* outstream) const;

  // Load TEMPO parameters from a string (strips chars as it goes)
  void load   (string* instr);
  void unload (string* outstr) const;

  void   nofit();
  void   fitall();

  int index (int eph_index) const;
  const psrParameter& operator [] (int eph_index) const;

  double p() const;
  double p_err() const;

  // return some values
  string psrname() const;
  friend bool operator == (const psrParams &, const psrParams &);
  friend bool operator != (const psrParams &, const psrParams &);

  double dm() const;
  /*
  double jra() const;
  double jdec() const;
  */

  // static members and methods
  static bool verbose;
  static vector<string> extensions();
  static string par_lookup (const char* name, bool use_cwd);


 protected:
  static char* tempo_pardir;

  void zero ();
  void destroy ();

  vector <psrParameter*> params;    // pulsar parameters

  psrParameter* element (int eph_index) const;
};

#endif
