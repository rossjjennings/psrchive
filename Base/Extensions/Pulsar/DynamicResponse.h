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

    //! Assignment operator
    const DynamicResponse& operator= (const DynamicResponse&);

    //!Destructor
    ~DynamicResponse ();

    //! Clone method
    DynamicResponse* clone () const 
    { return new DynamicResponse( *this ); }

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

    //! Set the minimum epoch of the dynamic response
    void set_minimum_epoch (const MJD& epoch) { minimum_epoch = epoch; }
    //! Get the minimum epoch of the dynamic response
    MJD get_minimum_epoch () const { return minimum_epoch; }

    //! Set the maximum epoch of the dynamic response
    void set_maximum_epoch (const MJD& epoch) { maximum_epoch = epoch; }
    //! Get the maximum epoch of the dynamic response
    MJD get_maximum_epoch () const { return maximum_epoch; }

    //! Set the minimum radio frequency of the dynamic response
    void set_minimum_frequency (double freq) { minimum_frequency = freq; }
    //! Get the minimum radio frequency of the dynamic response
    double get_minimum_frequency () const { return minimum_frequency; }

    //! Set the maximum radio frequency of the dynamic response
    void set_maximum_frequency (double freq) { maximum_frequency = freq; }
    //! Get the maximum radio frequency of the dynamic response
    double get_maximum_frequency () const { return maximum_frequency; }

    //! Resize the data array according to nchan, ntime, and npol
    void resize_data ();
    
    //! Get the response matrix data
    std::vector<std::complex<double>>& get_data () { return response; }
    
    //! Get the response matrix data
    const std::vector<std::complex<double>>& get_data () const { return response; }

    //! Set the response matrix data
    /* @pre vector must have size=ntime*nchan*npol */
    void set_data(const std::vector<std::complex<double>>&);

  protected:

    unsigned nchan = 0;
    unsigned ntime = 0;
    unsigned npol = 0;

    //! The minimum epoch
    MJD minimum_epoch;
    //! The maximum epoch
    MJD maximum_epoch;

    //! The minimum frequency in MHz
    double minimum_frequency = 0;
    //! The maximum frequency in MHz
    double maximum_frequency = 0;

    std::vector<std::complex<double>> response;            
  };

}

#endif

