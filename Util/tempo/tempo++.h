//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/tempo++.h,v $
   $Revision: 1.20 $
   $Date: 2006/11/09 18:03:41 $
   $Author: straten $ */

#ifndef __TEMPO_PP_H
#define __TEMPO_PP_H

#include "polyco.h"
#include "psrephem.h"
#include "toa.h"

#include <vector>

namespace Tempo {
  
  // get the tempo version
  float  get_version ();

  // get the specified tempo configuration parameter
  std::string get_configuration (const std::string& parameter);

  // set the system call used to run tempo
  void   set_system (const std::string& system_call);
  // get the system call used to run tempo
  std::string get_system ();

  // set the directory in which tempo system calls will be made
  void   set_directory (const std::string& directory);
  // get the directory in which tempo system calls will be made
  std::string get_directory ();

  // get the name of the file used to lock the tempo directory
  std::string get_lockfile ();

  // lock the tempo working directory
  void lock ();
  // unlock the tempo working directory
  void unlock ();

  // convenience overloads
  void   set_system (const std::string& system_call);
  void   set_directory (const std::string& directory);

  // verbosity flag of functions working in the Tempo namespace
  extern bool verbose;
  extern bool debug;

  // extension added to temporary Model filenames when working
  extern std::string extension;

  // default value passed to predict functions
  extern MJD unspecified;

  // base directory in which tempo will work
  extern std::string tmpdir;

  // file to which tempo stderr output will be redirected
  extern std::string stderr_filename;

  // file to which tempo ephemeris files are written
  extern std::string ephem_filename;

  //! Run tempo using the given arguments and input
  /*! tempo is run with a working directory given by
    Tempo::get_directory, so input files should be written here and
    output files (created by tempo) can be found here. */
  void tempo (const std::string& arguments, const std::string& input);

  // given a tempo ephemeris, generate toas over the range of MJD given
  // with characteristics specififed by rms and error.
  //   model      - tempo ephemeris
  //   toas       - return value
  //   start,end  - delimit epoch
  //   interval   - separation between points in minutes
  //   rms        - gaussian noise specified in microseconds
  //   error      - width of normal distribution of errors (not implemented)
  void fake (std::vector<toa>& toas, const psrephem& model,
	     const MJD& start, const MJD& end, double interval,
	     float rms = 0.0, float error = 0.0, float lst_range = 8.0);

  // given pulsar parameters and times of arrival, calls TEMPO to determine
  // the residual TOA.
  void fit (const psrephem& model, std::vector<toa>& toas,
	    psrephem* postfit = NULL, bool track=false,
	    Tempo::toa::State min_state = Tempo::toa::Normal);
    
  polyco span (const polyco& first_poly, const polyco& second_poly,
	       const psrephem& pephem);
  
}

#endif

