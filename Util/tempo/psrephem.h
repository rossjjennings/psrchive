//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/psrephem.h,v $
   $Revision: 1.6 $
   $Date: 2001/01/30 06:52:15 $
   $Author: straten $ */

#ifndef __PSREPHEM_H
#define __PSREPHEM_H

#include <stdio.h>

#include <vector>
#include <string>

#include "MJD.h"

class psrephem 
{
  friend class gtk_psrephem;
  friend class qt_psrephem;

 public:

  // this string needs to be long enough to hold the above-defined MACRO
  static vector<string> extensions();

  int*    parmStatus;
  string* value_str;
  double* value_double;
  int*    value_integer;
  double* error_double;

  bool tempo11;
  string nontempo11;

  psrephem() { init (); };
  ~psrephem() { destroy (); };

  psrephem (const psrephem &);

  psrephem (const char* psr_name, int use_cwd);
  psrephem (const char* filename);

  psrephem& operator = (const psrephem &);

  int create (const char* psr_name, int use_cwd);
  int load   (const char* filename);
  int unload (const char* filename) const;

  int load   (FILE* instream, size_t bytes);
  int unload (FILE* outstream) const;

  int load   (string* str);
  int unload (string* str) const;

  void   nofit();
  void   fitall();

  void   set_epoch (const MJD& new_epoch, bool binary = false);

  double p() const;
  double p_err() const;

  // return some values
  string psrname() const;
  friend int operator == (const psrephem &, const psrephem &);
  friend int operator != (const psrephem &, const psrephem &);

  double dm() const;
  double jra() const;
  double jdec() const;

  // returns the simple calculation based on Pb and x
  void mass_function (double& mf, double& mf_err) const;
  // returns m1 mass based on mass function, sin(i), and m2
  void m1 (double& m1, double& m1_err) const;
  // inverts the above to return m2, given m1
  void m2 (double& m2, double m1) const;

  string par_lookup (const char* name, int use_cwd);
  static char* tempo_pardir;
  static int   verbose;

 protected:
  void init ();
  void size_dataspace();
  void zero ();
  void destroy ();

  int old_load (const char* filename);
  int old_unload (const char* filename) const;

  // static char ephemstr [EPH_NUM_KEYS][EPH_STR_LEN];
};

ostream& operator<< (ostream& ostr, const psrephem& eph);

#endif

