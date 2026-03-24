//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2024 Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/DynamicResponse.h

#ifndef __DynamicResponse_h
#define __DynamicResponse_h

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/Profile.h"

#include <vector> 
#include <complex>

namespace Pulsar {

  class DynamicResponse : public Pulsar::Archive::Extension
  {

  public:
 
    //! Default constructor
    DynamicResponse ();

    //! Copy constructor
    DynamicResponse (const DynamicResponse& extension);

    //!Destructor
    ~DynamicResponse ();

    //! Clone method
    DynamicResponse* clone () const { return new DynamicResponse(*this); }

    //! Set the number of channels in each frequency response
    void set_nchan (unsigned);
    
    //! Get the number of channels in each frequency response
    unsigned get_nchan () const;

    //! Set the number of time slices
    void set_ntime (unsigned);

    //! Get the number of time slices
    unsigned get_ntime () const;

    //! Set the number of polarizations
    void set_npol (unsigned);
    
    //! Get the number of polarizations
    unsigned get_npol () const;

    //! Set the epoch at the centre of the first response
    void set_minimum_epoch (const MJD& epoch) { minimum_epoch = epoch; }
    //! Set the epoch at the centre of the first response
    MJD get_minimum_epoch () const { return minimum_epoch; }

    //! Set the epoch at the centre of the last response
    void set_maximum_epoch (const MJD& epoch) { maximum_epoch = epoch; }
    //! Set the epoch at the centre of the last response
    MJD get_maximum_epoch () const { return maximum_epoch; }

    //! Set the centre radio frequency of the dynamic response in MHz
    void set_centre_frequency (double freq) { centre_frequency = freq; }
    //! Get the centre radio frequency of the dynamic response in MHz
    double get_centre_frequency () const { return centre_frequency; }

    //! Set the bandwidth of the dynamic response in MHz
    void set_bandwidth (double freq) { bandwidth = freq; }
    //! Get the bandwidth of the dynamic response in MHz
    double get_bandwidth () const { return bandwidth; }

    //! Set the number of non-zero impulse response function values at positive delay
    void set_impulse_pos (unsigned nsamp) { impulse_pos = nsamp; }

    //! Get the number of non-zero impulse response function values at positive delay
    unsigned get_impulse_pos () const { return impulse_pos; }

    //! Set the number of non-zero impulse response function values at negative delay
    void set_impulse_neg (unsigned nsamp) { impulse_neg = nsamp; }

    //! Get the number of non-zero impulse response function values at negative delay
    unsigned get_impulse_neg () const { return impulse_neg; }

    //! Resize the data array according to nchan, ntime, and npol
    void resize_data ();
    
    //! Get the response matrix data
    std::vector<std::complex<double>>& get_data () { return response; }
    
    //! Get the response matrix data
    const std::vector<std::complex<double>>& get_data () const { return response; }

    //! Set the response matrix data
    /* @pre vector must have size=ntime*nchan*npol */
    void set_data (const std::vector<std::complex<double>>&);

  protected:

    //! Number of frequency channels in each frequency response
    unsigned nchan = 0;
    //! Number of time slices
    unsigned ntime = 0;
    //! Number of polarizations
    unsigned npol = 0;

    //! Number of non-zero impulse response function values at positive delay
    unsigned impulse_pos = 0;

    //! Number of non-zero impulse response function values at negative delay
    unsigned impulse_neg = 0;

    //! The epoch at the centre of the first response
    MJD minimum_epoch;
    //! The epoch at the centre of the last response
    MJD maximum_epoch;

    //! The centre frequency in MHz
    double centre_frequency = 0;
    //! The bandwidth in MHz
    double bandwidth = 0;

    //! Time-major order two-dimensional array of frequency response functions
    std::vector<std::complex<double>> response;            
  };

}

#endif

