//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/polyco.h,v $
   $Revision: 1.17 $
   $Date: 2003/01/22 20:07:06 $
   $Author: straten $ */

#ifndef __POLY_H
#define __POLY_H

#include <string>
#include <vector>

#ifdef MPI
#include <mpi.h>
#endif

#ifdef PSRFITS
#include <fitsio.h>
#endif

#include "Phase.h"
#include "MJD.h"
#include "ReferenceAble.h"

class polynomial {
  
  friend class polyco;
  
protected:
  
  //! The pulsar name
  string psrname;
  
  //! The calendar date string
  string date;

  //! The UTC string
  string utc;

  //! The pulsar phase at reftime
  Phase ref_phase;

  //! The epoch to which the polynomial is referenced
  MJD reftime;

  //! Reference rotation frequency, F0 (Hz)
  double f0;

  //! TEMPO telescope id code
  char telescope;

  //! Observing frequency
  double freq;

  //! true if the pulsar is in a binary system
  bool binary;

  //! if binary, binary phase
  double binph;

  //! if binary, binary frequency
  double binfreq;

  //! dispersion measure
  float dm;

  //! number of minutes spanned by polynomial
  double nspan_mins;

  //! Flag that the polyco was created using TEMPO v.11 or greater
  bool tempov11;

  //! Doppler shift due to Earth's motion along line of sight
  double doppler_shift;

  //! log of the r.m.s residual between polynomial and model
  double log_rms_resid;

  //! polynomial coefficients
  vector<double> coefs;

  //! intializes all values to null
  void init();

public:
  //! null constructor
  polynomial() { init(); }

  //! copy constructor
  polynomial (const polynomial& poly) { operator = (poly); }

  //! destructor
  ~polynomial() {}

  polynomial& operator = (const polynomial& poly);

  int load (string* instr);
  int unload (string *outstr) const;
  int unload (FILE* fptr) const;

  void prettyprint  () const;

  //! Returns the pulse period (in seconds) at the given time
  double period (const MJD &t) const;

  //! Returns the pulse phase (in turns) at the given time
  Phase  phase (const MJD &t) const;

  //! Returns the pulse phase at the given time and observing frequency
  Phase  phase (const MJD &t, float obs_freq) const;

  //! Returns the time at the given pulse phase
  MJD    iphase (const Phase& p) const;

  //! Returns the spin frequency (in Hz) at the given time
  double frequency (const MJD &t) const;

  //! Returns the spin frequency derivative (in Hz/s) at the given time 
  double chirp (const MJD &t) const;

  //! Returns the apparent acceleration toward observer (m s^-2)
  double accel (const MJD &t) const { return chirp(t)/frequency(t)*2.9979e8; }

  bool   is_tempov11       () const {return tempov11; }
  char   get_telescope     () const {return telescope; }
  double get_freq          () const {return freq; }
  MJD    get_reftime       () const {return reftime; }
  double get_nspan         () const {return nspan_mins; }
  float  get_dm            () const {return dm; }
  int    get_ncoeff        () const {return (int) coefs.size(); }
  double get_doppler_shift () const {return doppler_shift / 1e4; }
  string get_psrname       () const {return psrname; }

  static double flexibility;

  MJD start_time () const
    { return reftime - nspan_mins * (1.0+flexibility) * 60.0/2.0; };
  MJD end_time () const 
    { return reftime + nspan_mins * (1.0+flexibility) * 60.0/2.0; };

  Phase start_phase () const
    { return phase (start_time()); };
  Phase end_phase () const
    { return phase (end_time()); };
  
  friend int operator == (const polynomial &, const polynomial &);
  friend int operator != (const polynomial &, const polynomial &);

  // MPI functions
#ifdef MPI
  friend int mpiPack_size (const polynomial&, MPI_Comm comm, int* size);
  friend int mpiPack (const polynomial&, void* outbuf, int outcount,
		      int* position, MPI_Comm comm);
  friend int mpiUnpack (void* inbuf, int insize, int* position, 
			polynomial*, MPI_Comm comm);
#endif

#ifdef PSRFITS
  void load (fitsfile* fptr, long row);
  void unload (fitsfile* fptr, long row);
#endif

};

class polyco : public Reference::Able {

 protected:
  //! null value of pulsar name
  static string anyPsr;

 public:
  static bool verbose;
  static bool debug;
  static double precision;

  //! The polynomial sets
  vector<polynomial> pollys;

  //! null initializer
  polyco () {}
  polyco (const polyco& poly) { operator = (poly); }

  //! Load in polycos
  polyco (const char * id);
  polyco (const string id);
  polyco& operator = (const polyco& poly);

  ~polyco() {}

  //! these functions return the number of polynomials successfully loaded
  int load (const char* filename, size_t nbytes=0);
  int load (const string& filename, size_t nbytes=0)
	{ return load (filename.c_str(), nbytes); }
  int load (FILE * fp, size_t nbytes=0);
  int load (string* instr);

  // these functions return -1 upon error
  int unload (const char* filename) const ;
  int unload (const string& filename) const
        { return unload (filename.c_str()); }

  // these functions return the number of bytes unloaded (-1 on error)
  int unload (string *outstr) const;
  int unload (FILE* fptr) const;

  void append (const polyco& poly);

  void  prettyprint  () const;

  const polynomial* nearest (const MJD &t, 
			     const string& psrname = anyPsr) const;

  const polynomial& best (const MJD &t, const string& psr = anyPsr) const;
  const polynomial& best (const Phase &p, const string& psr = anyPsr) const;

  int i_nearest (const MJD &t,   const string& psrname = anyPsr) const;
  int i_nearest (const Phase &p, const string& psrname = anyPsr) const;

  double doppler_shift (const MJD& t, const string& psr = anyPsr) const
    { return best(t, psr).get_doppler_shift(); };

  Phase phase (const MJD& t, const string& psr = anyPsr) const
    { return best(t, psr).phase(t); };

  Phase phase (const MJD& t, float obs_freq, const string& psr = anyPsr) const
    { return best(t, psr).phase(t, obs_freq); };

  MJD iphase (const Phase& phase, const string& psr = anyPsr) const
    { return best(phase, psr).iphase(phase); };

  double period(const MJD& t, const string& psr = anyPsr) const
    { return best(t, psr).period(t); };

  double frequency(const MJD& t, const string& psr = anyPsr) const
    { return best(t, psr).frequency(t); };

  double chirp(const MJD& t, const string& psr = anyPsr) const
    { return best(t, psr).chirp(t); };

  double accel(const MJD& t, const string& psr = anyPsr) const
    { return best(t, psr).accel(t); };

  // bool   is_tempov11   () const { return pollys.front().is_tempov11(); };
  char   get_telescope () const { return pollys.front().get_telescope(); }
  double get_freq      () const { return pollys.front().get_freq(); }
  MJD    get_reftime   () const { return pollys.front().get_reftime(); }
  double get_refperiod () const { return 1.0/pollys.front().f0; }
  double get_nspan     () const { return pollys.front().get_nspan(); }
  float  get_dm        () const { return pollys.front().get_dm(); }
  int    get_ncoeff    () const { return pollys.front().get_ncoeff(); }
  string get_psrname   () const { return pollys.front().get_psrname(); }

  bool is_tempov11() const;

  MJD  start_time () const { return pollys.front().start_time(); };
  MJD  end_time ()   const { return pollys.back().end_time(); };

  Phase start_phase () const { return pollys.front().start_phase(); };
  Phase end_phase ()   const { return pollys.back().end_phase(); };

  friend int operator == (const polyco &, const polyco &);
  friend int operator != (const polyco &, const polyco &);

#ifdef MPI
  friend int mpiPack_size (const polyco&, MPI_Comm comm, int* size);
  friend int mpiPack   (const polyco&, void* outbuf, int outcount,
			int* position, MPI_Comm comm);
  friend int mpiUnpack (void* inbuf, int insize, int* position, 
			polyco*, MPI_Comm comm);
#endif

#ifdef PSRFITS
  void load (fitsfile* fptr, int back=0);
  void unload (fitsfile* fptr, int back=0);
#endif

};

inline ostream& operator<< (ostream& ostr, const polyco& p) {
  string out; p.unload(&out); return ostr << out; }

#endif

