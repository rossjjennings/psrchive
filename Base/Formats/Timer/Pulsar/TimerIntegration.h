//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/TimerIntegration.h,v $
   $Revision: 1.2 $
   $Date: 2003/04/19 20:27:00 $
   $Author: straten $ */

/*
  
  Pulsar::Integration - base class for pulsar observations

*/

#ifndef __Timer_Integration_h
#define __Timer_Integration_h

#include <stdio.h>
#include <vector>

#include "Pulsar/Integration.h"
#include "mini.h"

namespace Pulsar {
  
  class TimerIntegration : public Integration {

  public:

    friend class TimerArchive;

    //! Null constructor simply intializes defaults
    TimerIntegration () { init(); }

    //! Copy constructor
    TimerIntegration (const TimerIntegration& t_subint,
		      int npol=-1, int nchan=-1);

    //! General copy constructor
    TimerIntegration (const Integration& subint,
		      int npol=-1, int nchan=-1);

    //! Destructor deletes data area
    virtual ~TimerIntegration ();

    //! Copy an Integration
    virtual void copy (const Integration& subint, int npol=-1, int nchan=-1);

    //! Return the pointer to a new copy of self
    virtual Integration* clone (int npol=-1, int nchan=-1) const;

    //! Get the number of chans
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_nchan () const { return nchan; }

    //! Get the number of polarization measurements
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_npol () const { return npol; }

    //! Get the number of bins in each profile
    /*! This attribute may be set only through Integration::resize */
    virtual unsigned get_nbin () const { return nbin; }
 
    //! Get the epoch of the rising edge of bin zero
    virtual MJD get_epoch () const;
    //! Set the epoch of the rising edge of bin zero
    virtual void set_epoch (const MJD& mjd);

    //! Get the total time integrated (in seconds)
    virtual double get_duration () const;
    //! Set the total time integrated (in seconds)
    virtual void set_duration (double seconds);
        
   //! Get the centre frequency (in MHz)
    virtual double get_centre_frequency() const { return centrefreq; }
    //! Set the centre frequency (in MHz)
    virtual void set_centre_frequency (double MHz) { centrefreq = MHz; }
    
    //! Get the bandwidth (in MHz)
    virtual double get_bandwidth() const { return bw; }
    //! Set the bandwidth (in MHz)
    virtual void set_bandwidth (double MHz) { bw = MHz; }

    //! Get the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    virtual double get_dispersion_measure () const { return dm; }
    //! Set the dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    virtual void set_dispersion_measure (double pc_cm3) { dm = pc_cm3; }
    
    //! Get the folding period (in seconds)
    virtual double get_folding_period () const;
    //! Set the folding period (in seconds)
    virtual void set_folding_period (double seconds);

    //! Get the feed configuration of the receiver
    virtual Signal::Basis get_basis () const { return type; }
    //! Set the feed configuration of the receiver
    virtual void set_basis (Signal::Basis _type) { type = _type; }

    //! Get the polarimetric state of the profiles
    virtual Signal::State get_state () const { return state; }
    //! Set the polarimetric state of the profiles
    virtual void set_state (Signal::State _state);

    //! load the subint from file
    void load (FILE * fptr, int wts_and_bpass, bool big_endian=true);

    //! unload the subint to file
    void unload (FILE * fptr) const;

    //! Resizes the dimensions of the data area
    virtual void resize (unsigned npol, unsigned nchan, unsigned nbin);

  protected:

    struct mini mini;

    //! number of polarization measurments
    unsigned npol;

    //! number of sub-chans
    unsigned nchan;
    
    //! number of bins
    unsigned nbin;

    //! centre frequency (in MHz)
    double centrefreq;

    //! bandwidth (in MHz)
    double bw;

    //! dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    double dm;

    //! polarimetric state of profiles
    Signal::State state;

    //! receiver feed type
    Signal::Basis type;

    //! weight of each channel/poln
    vector<float> wts;

    //! median of average bandpass
    vector< vector<float> > med;

    //! bandpass averaged over bins
    vector< vector<float> > bpass;

    //! Set the number of pulsar phase bins
    /*! Called by Integration methods to update sub-class attribute */
    virtual void set_nbin (unsigned numbins) { nbin = numbins; }

    //! Set the number of frequency channels
    /*! Called by Integration methods to update sub-class attribute */
    virtual void set_nchan (unsigned numchan) { nchan = numchan; }

    //! Set the number of polarization measurements
    /*! Called by Integration methods to update sub-class attribute */
    virtual void set_npol (unsigned numpol) { npol = numpol; }

    //! Set all values to null
    void init ();
  };

  //! load a Profile from a timer archive
  void TimerProfile_load (FILE* fptr, Profile* profile, bool big_endian=true);

  //! unload a Profile to a timer archive
  void TimerProfile_unload (FILE* fptr, const Profile* profile);

  //! return the integer poln code for a timer archive profile
  int TimerProfile_poln (Signal::Component state);
}

#endif
