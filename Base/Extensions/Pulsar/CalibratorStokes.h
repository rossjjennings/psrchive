//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CalibratorStokes.h,v $
   $Revision: 1.1 $
   $Date: 2003/11/04 11:00:19 $
   $Author: straten $ */

#ifndef __CalibratorStokes_h
#define __CalibratorStokes_h

#include "Pulsar/Archive.h"
#include "Stokes.h"
#include "Estimate.h"

namespace Pulsar {
  
  //! CalibratorStokes
  /*! This class provides a simple container for storing the Stokes
   parameters of the artificial calibrator source. */
  
  class CalibratorStokes : public Pulsar::Archive::Extension {
    
  public:
    
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

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Set the validity flag for the specified channel
    void set_valid (unsigned ichan, bool valid);
    //! Get the validity flag for the specified channel
    bool get_valid (unsigned ichan) const;

    //! Set the Stokes parameters of the specified frequency channel
    void set_stokes (unsigned ichan, const Stokes< Estimate<float> >& stokes);
    //! Get the Stokes parameters of the specified frequency channel
    Stokes< Estimate<float> > get_stokes (unsigned ichan) const;

  protected:

    //! The Stokes parameters for each frequency channel
    /*! Stokes I is used to represent data validity */
    vector< Stokes< Estimate<float> > > stokes;

    //! Ensure that ichan <= get_nchan
    void range_check (unsigned ichan, const char* method) const;
  };
 

}

#endif
