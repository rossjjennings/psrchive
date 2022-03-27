//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/CalibratorStokes.h

#ifndef __CalibratorStokes_h
#define __CalibratorStokes_h

#include "Pulsar/ArchiveExtension.h"
#include "Stokes.h"
#include "Estimate.h"

namespace Pulsar {
  
  //! Stores the Stokes parameters of the reference source
  /*! This Archive::Extension class provides a simple container for storing
   the Stokes parameters of the artificial (reference) calibrator source. */
  class CalibratorStokes : public Pulsar::Archive::Extension {
    
  public:

    //! The point where the reference source signal is coupled    
    enum CouplingPoint
    {
     BeforeProjection, // e.g. satellite-borne
     BeforeBasis,      // e.g. radiated onto primary reflector
     BeforeFrontend,   // e.g. radiated into feedhorn
     BeforeIdeal,      // e.g. coupled after transducer
     BeforeBackend,    // not sure why
     Unknown           // don't know
    };
    
    //! Default constructor
    CalibratorStokes ();

    //! Copy constructor
    CalibratorStokes (const CalibratorStokes& extension);

    //! Assignment operator
    const CalibratorStokes& operator= (const CalibratorStokes& extension);

    //! Destructor
    ~CalibratorStokes ();

    //! Clone method
    CalibratorStokes* clone () const { return new CalibratorStokes( *this ); }

    //! Get the text ineterface 
    TextInterface::Parser* get_interface();

    //! Implements the text interface
    class Interface;

    //! Return a short name
    std::string get_short_name () const { return "ref"; }

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Set the point where the reference source signal is coupled
    void set_coupling_point (CouplingPoint);
    //! The point where the reference source signal is coupled
    CouplingPoint get_coupling_point () const;

    //! Remove the inclusive range of channels
    void remove_chan (unsigned first, unsigned last);

    //! Set the validity flag for the specified channel
    void set_valid (unsigned ichan, bool valid);
    //! Get the validity flag for the specified channel
    bool get_valid (unsigned ichan) const;

    //! Set the Stokes parameters of the specified frequency channel
    void set_stokes (unsigned ichan, const Stokes< Estimate<float> >& stokes);
    //! Get the Stokes parameters of the specified frequency channel
    Stokes< Estimate<float> > get_stokes (unsigned ichan) const;

    unsigned get_nparam () const { return 3; /* Stokes Q,U,V */ }
    Estimate<float> get_Estimate ( unsigned iparam, unsigned ichan ) const;
    void set_Estimate (unsigned iparam, unsigned ichan, Estimate<float>&);

    //! Append CalibratorStokes Extension data from another Archive
    void frequency_append (Archive* to, const Archive* from);

  protected:

    //! The point where the reference source signal is coupled
    /*! On some systems, the reference source (e.g. noise diode)
      signal is coupled to the astronomy signal after the OMT.  Also,
      when modeling the instrumental response using METM, variable
      ionospheric Faraday rotation causes variable apparent rotation
      about the line of sight, which in turn affects the best-fit
      Stokes parameters of the reference source.  Therefore, when
      using METM, it is best to model the reference source as though
      coupled after the frontend. */
    CouplingPoint coupling_point;
    
    //! The Stokes parameters for each frequency channel
    /*! Stokes I is used to represent data validity */
    std::vector< Stokes< Estimate<float> > > stokes;

    //! Ensure that ichan <= get_nchan
    void range_check (unsigned ichan, const char* method) const;

    friend class CalibratorStokes::Interface;

    // Internal convenience interface to the Stokes polarization vector data
    class PolnVector;

    //! Interface to StokesVector
    PolnVector* get_poln (unsigned ichan);

    Reference::To<PolnVector> current;
  };
 
  std::ostream& operator << (std::ostream& ostr,
			     CalibratorStokes::CouplingPoint);
  std::istream& operator >> (std::istream& is,
			     CalibratorStokes::CouplingPoint&);

}

#endif
