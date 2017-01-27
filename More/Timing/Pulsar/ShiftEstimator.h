//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/ShiftEstimator.h

#ifndef __Pulsar_ShiftEstimator_h
#define __Pulsar_ShiftEstimator_h

#include "Pulsar/Algorithm.h"

#include "TextInterface.h"
#include "Estimate.h"

namespace Pulsar {

  class Archive;

  //! Algorithms that estimate pulse phase shifts
  class ShiftEstimator : public Algorithm
  {

  public:

    static bool verbose;

    //! Return the shift estimate
    virtual Estimate<double> get_shift () const = 0;

    //! Return the statistical goodness of fit
    virtual double get_reduced_chisq () const { return 0; }

    //! Return the profile S/N ratio
    virtual double get_snr () const { return 0; }

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface () = 0;

    //! Return a copy constructed instance of self
    virtual ShiftEstimator* clone () const = 0;

    //! Return a name that can be subsituted for the template/standard
    virtual std::string get_name () const { return "N/A"; }
    
    //! Prepare the data for use
    virtual void preprocess (Archive*);

    //! Construct a new ShiftEstimator from a string
    static ShiftEstimator* factory (const std::string& name_and_parse);

  };

  //! Convenience interface used to add ShiftEstimator children to factory
  template<class T>
  class EmptyInterface : public TextInterface::To<T>
  {
    std::string name;
  public:
    EmptyInterface (const std::string& _name) { name = _name; }
    std::string get_interface_name () const { return name; }
  };
}


#endif // !defined __Pulsar_ShiftEstimator_h
