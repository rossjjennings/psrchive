#ifndef __PSREPHEM_H
#define __PSREPHEM_H

#include <stdio.h>
#include <string>
#include "psr_cpp.h"
#define PSREPHEM_TMP_FNAME ".psrephem_XXXXXX"

class psrephem 
{
  friend class gtk_psrephem;
  friend class qt_psrephem;

 public:

  // this string needs to be long enough to hold the above-defined MACRO
  static char tmp_fname[25];

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
  int load (const char* filename);
  int unload (const char* filename) const;

  int load (FILE* instream, size_t bytes);
  int unload (FILE* outstream) const;

  int load (string* str);
  int unload (string* str) const;

  void   nofit();
  void   fitall();
  double p();
  double p_err();

  // return some values
  string psrname() const;
  friend int operator == (const psrephem &, const psrephem &);
  friend int operator != (const psrephem &, const psrephem &);

  double dm() const;
  double jra() const;
  double jdec() const;

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

#endif

