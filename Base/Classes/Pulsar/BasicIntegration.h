//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/BasicIntegration.h,v $
   $Revision: 1.2 $
   $Date: 2002/04/22 06:59:07 $
   $Author: ahotan $ */

#ifndef __BasicIntegration_h
#define __BasicIntegration_h

#include "Integration.h"
  
namespace Pulsar {

  class BasicIntegration : public Integration {

  public:

    //! null constructor
    BasicIntegration () { init(); }

    //! copy constructor
    BasicIntegration (const Integration& subint, int _npol=0, int _nchan=0);

    //! destructor
    virtual ~BasicIntegration () { }

    //! Return the pointer to a new copy of self
    virtual Integration* clone (int npol=0, int nchan=0) const;

    //! Get the number of chans
    /*! This attribute may be set only through Integration::resize */
    virtual int get_nchan () const { return nchan; }

    //! Get the number of polarization measurements
    /*! This attribute may be set only through Integration::resize */
    virtual int get_npol () const { return npol; }

    //! Get the number of bins in each profile
    /*! This attribute may be set only through Integration::resize */
    virtual int get_nbin () const { return nbin; }
 
    //! Get the MJD at the beginning of the integration
    virtual MJD get_mid_time() const { return mid_time; }
    //! Set the MJD at the beginning of the integration
    virtual void set_mid_time (const MJD& mjd) { mid_time = mjd; }

    //! Get the total time integrated (in seconds)
    virtual double get_duration() const { return duration; }
    //! Set the total time integrated (in seconds)
    virtual void set_duration (double seconds) { duration = seconds; }

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
    virtual double get_folding_period() const { return pfold; }
    //! Set the folding period (in seconds)
    virtual void set_folding_period (double seconds) { pfold = seconds; }

    //! Get the feed configuration of the receiver
    virtual Feed::Type get_feed_type () const { return type; }
    //! Set the feed configuration of the receiver
    virtual void set_feed_type (Feed::Type _type) { type = _type; }

    //! Get the polarimetric state of the profiles
    virtual Poln::State get_poln_state () const { return state; }
    //! Set the polarimetric state of the profiles
    virtual void set_poln_state (Poln::State _state) { state = _state; }


  protected:

    //! Set the number of pulsar phase bins
    /*! Called by Integration methods to update child attribute */
    virtual void set_nbin (int numbins) { nbin = numbins; }

    //! Set the number of frequency channels
    /*! Called by Integration methods to update child attribute */
    virtual void set_nchan (int numchan) { nchan = numchan; }

    //! Set the number of polarization measurements
    /*! Called by Integration methods to update child attribute */
    virtual void set_npol (int numpol) { npol = numpol; }

    //! number of polarization measurments
    int npol;

    //! number of sub-chans
    int nchan;
    
    //! number of bins
    int nbin;

    //! time at the middle of the observation
    MJD mid_time;

    //! duration of integration
    double duration;

    //! centre frequency (in MHz)
    double centrefreq;

    //! bandwidth (in MHz)
    double bw;

    //! folding period (in seconds)
    double pfold;

    //! dispersion measure (in \f${\rm pc cm}^{-3}\f$)
    double dm;

    //! polarimetric state of profiles
    Poln::State state;

    //! receiver feed type
    Feed::Type type;

    //! initialize all attributes to null
    void init();
  };

}

#endif
