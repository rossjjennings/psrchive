//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/psrephem.h,v $
   $Revision: 1.10 $
   $Date: 2001/02/15 05:15:28 $
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

  // update all parameters to the new_epoch
  void   set_epoch (const MJD& new_epoch, bool binary = false);

  // multiply all errors by efac
  void   efac (float efac);

  double p() const;
  double p_err() const;

  // return some values
  string psrname() const;
  friend int operator == (const psrephem &, const psrephem &);
  friend int operator != (const psrephem &, const psrephem &);

  double dm() const;
  double jra() const;
  double jdec() const;

  // these functions return 0 if no error, -1 on error
  //
  // returns the sine of the inclination angle
  int sini (double& si, double& si_err) const;
  // returns the simple calculation based on Pb and x
  int mass_function (double& mf, double& mf_err) const;
  // returns m1 mass based on mass function, sin(i), and m2
  int m1 (double& m1, double& m1_err) const;
  // inverts the above to return m2, given m1
  int m2 (double& m2, double m1) const;

  // returns the composite proper motion
  int pm (double& pm, double& pm_err) const;
  // returns the proper motion celestial position angle
  int phi (double& phi, double& phi_err) const;

  // returns the orbital period in seconds
  int P (double& p, double& p_err) const;
  // returns the orbital period derivative in seconds
  int P_dot (double& p, double& p_err) const;
  
  // returns the quadratic Doppler shift due to apparent acceleration along
  // the line of sight that arises from proper motion
  int Shklovskii (double& beta, double& beta_err) const;

  // //////////////////////////////////////////////////////////////////////
  // to use the following two functions, you will need to link with 
  // -lpsrinfo

  // returns a block of LaTeX formatted text suitable for use in tables
  string tex () const;
  // returns the LaTeX formatted pulsar name
  string tex_name () const;
  // returns the a LaTeX formatted string for the parameter at ephind
  string tex_val (int ephind, double fac=1.0, unsigned precision=1) const;

  // returns a LateX formatted string suitable for labelling the parameter
  static const char* tex_descriptor (int ephind);


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

