//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/ScintPowerEstimator.h,v $
   $Revision: 1.3 $
   $Date: 2005/03/22 06:20:55 $
   $Author: straten $ */

#ifndef __ScintPowerEstimator_h
#define __ScintPowerEstimator_h

#include "Pulsar/PowerEstimator.h"

namespace Pulsar {

  class ScintArchive;

  //! Returns power estimates from a ScintArchive
  class ScintPowerEstimator : public PowerEstimator {

  public:
    
    //! Default constructor
    ScintPowerEstimator (const ScintArchive* archive = 0);

    //! Copy constructor
    ScintPowerEstimator (const ScintPowerEstimator&);

    //! Operator =
    const ScintPowerEstimator& operator= (const ScintPowerEstimator&);

    //! Destructor
    ~ScintPowerEstimator ();

    //! Clone method
    ScintPowerEstimator* clone () const 
    { return new ScintPowerEstimator (*this); }

    //! Implement PowerEstimator::set_archive
    void set_archive (const ScintArchive* archive);

    //! Implement PowerEstimator::get_power
    void get_power (unsigned isub, unsigned ipol,
		    std::vector<float>& on,
		    std::vector<float>& off) const;

  protected:

    //! The ScintArchive from which to derive power estimates
    Reference::To<const ScintArchive> archive;

  };
 

}

#endif
