//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/psrephem.h,v $
   $Revision: 1.22 $
   $Date: 2003/01/10 14:27:49 $
   $Author: straten $ */

#ifndef __PSREPHEM_H
#define __PSREPHEM_H

#include <stdio.h>

#include <vector>
#include <string>

#ifdef PSRFITS
#include <fitsio.h>
#endif

#include "MJD.h"
#include "Angle.h"
#include "Reference.h"

class psrephem : public Reference::Able
{
  friend class psrParams;

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

  void zero ();

  int create (const char* psr_name, int use_cwd=0);
  int create (const string& psr_name, int use_cwd=0)
    { return create (psr_name.c_str(), use_cwd); }

  int load   (const char* filename);
  int load   (const string& filename) { return load (filename.c_str()); }

  int unload (const char* filename) const;
  int unload (const string& fname) const { return unload (fname.c_str()); }

  int load   (FILE* instream, size_t bytes);
  int unload (FILE* outstream) const;

  int load   (string* str);
  int unload (string* str) const;

#ifdef PSRFITS
  void load (fitsfile* fptr, long row=0);
#endif

  void   nofit();
  void   fitall();

  // update all parameters to the new_epoch
  void   set_epoch (const MJD& new_epoch, bool binary = false);

  // multiply all errors by efac
  void   efac (float efac);

  // return the value
  string get_string  (int ephind);
  double get_double  (int ephind);
  MJD    get_MJD     (int ephind);
  Angle  get_Angle   (int ephind);
  int    get_integer (int ephind);

  // set value
  void set_string  (int ephind, const string&);
  void set_double  (int ephind, double );
  void set_MJD     (int ephind, const MJD&);
  void set_Angle   (int ephind, const Angle&);
  void set_Integer (int ephind, int);

  double p() const;
  double p_err() const;

  // return some values
  string psrname() const;
  friend int operator == (const psrephem &, const psrephem &);
  friend int operator != (const psrephem &, const psrephem &);

  double get_dm() const;
  void set_dm (double dm);

  double jra() const;
  double jdec() const;

  // these functions return 0 if no error, -1 on error
  //
  // return galactic latitude and longitude in radians (slalib)
  int galactic (double& l, double& b);
  // return galactic height (z) in parsec
  int galactic_z (double& z);

  // returns the acceleration along the line of sight arising
  // from differential galactic rotation in km/s
  int acc_diffrot (double& beta);

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

  // returns the proper motion contribution to xdot (Kopeikin 1996)
  int pm_x_dot (double& xdot, double& xdot_err) const;
  // returns the proper motion contribution to omdot in degrees per year
  int pm_omega_dot (double& omdot, double& omdot_err) const;

  // returns the orbital period in seconds
  int P (double& p, double& p_err) const;
  // returns the orbital period derivative
  int P_dot (double& pdot, double& pdot_err) const;
    // returns the second orbital period derivative in seconds^-1
  int P_ddot (double& pddot, double& pddot_err) const;

  // returns the transverse quadratic Doppler shift due to the apparent
  // acceleration along the line of sight that arises from proper motion
  int quadratic_Doppler (double& beta, double& beta_err) const;

  int cubic_Doppler (double& gamma, double& gamma_err,
		     double pmrv, double pmrv_err) const;

  int Doppler_P_dotdot (double& P_dotdot, double& P_dotdot_err,
			double pmrv, double pmrv_err) const;

  int Doppler_F2 (double& f2, double& f2_err,
		  double pmrv, double pmrv_err) const;

  int intrinsic_P_dotdot (double& P_dotdot, double& P_dotdot_err,
			  float braking_index=3) const;

  // returns the intrinsic x_dot due to gravitational wave emission
  int GR_x_dot (double& x_dot) const;

  // returns the intrinsic Pb_dot due to gravitational wave emission
  int GR_Pb_dot (double& Pb_dot) const;

  // returns the intrinsic Pb_dot due to gravitational wave emission
  int GR_omega_dot (double& w_dot) const;

  // given omega_dot from par file, returns the total mass
  int GR_omega_dot_mtot (double& mtot, double& mtot_err) const;

  // uses the above and sin(i) and f(M) to derive m2
  int GR_omega_dot_m2 (double& m2, double& m2_err) const;

  // returns the general relativistic prediction of gamma
  int GR_gamma (double& gamma) const;

  // used by the above
  int GR_f_e (double& f_e) const;

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

  // returns a block of LaTeX formatted text suitable for use in tables
  static string tex (vector<psrephem>& vals, bool dots=false);

  string par_lookup (const char* name, int use_cwd);
  static char* tempo_pardir;
  static int   verbose;

 protected:
  void init ();
  void size_dataspace();
  void destroy ();
  void zero_work ();

  int old_load (const char* filename);
  int old_unload (const char* filename) const;

};

ostream& operator<< (ostream& ostr, const psrephem& eph);

#endif

