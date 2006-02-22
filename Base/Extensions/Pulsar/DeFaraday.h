//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/DeFaraday.h,v $
   $Revision: 1.1 $
   $Date: 2006/02/22 16:48:27 $
   $Author: straten $ */

#ifndef __Pulsar_DeFaraday_h
#define __Pulsar_DeFaraday_h

#include "Pulsar/Integration.h"

namespace Pulsar {
  
  //! Stores parameters used to correct Faraday rotation in each Integration
  class DeFaraday : public Pulsar::Integration::Extension {
    
  public:
    
    //! Default constructor
    DeFaraday ();

    //! Copy constructor
    DeFaraday (const DeFaraday& extension);

    //! Assignment operator
    const DeFaraday& operator= (const DeFaraday& extension);
    
    //! Destructor
    ~DeFaraday ();

    //! Clone method
    DeFaraday* clone () const { return new DeFaraday( *this ); }

    //! Set the rotation measure
    void set_rotation_measure (double rotation_measure);
    //! Get the rotation measure
    double get_rotation_measure () const;

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);
    //! Get the reference wavelength
    double get_reference_wavelength () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency
    double get_reference_frequency () const;

  protected:

    //! The rotation measure
    double rotation_measure;

    //! The reference wavelength in metres
    double reference_wavelength;

  };
  
}

#endif

