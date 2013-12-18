//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/CalibratorType.h

#ifndef __CalibratorType_H
#define __CalibratorType_H

#include "Pulsar/Calibrator.h"
#include "Identifiable.h"

namespace Pulsar
{
  /*!  Children of this abstract base class store the minimal amount
    of calibrator information required for file I/O.  They also
    express the relationships between different calibrators and
    obviate the need to recompile all Calibrator-derived code whenever
    a new type is added.
  */
  class Calibrator::Type : public Identifiable
  {
  public:

    //! Construct a new instance of Calibrator::Type, based on name
    static Type* factory (const std::string& name);

    //! Construct a new instance of Calibrator::Type, based on Calibrator
    static Type* factory (const Calibrator*);

    //! Return the name of the calibrator type
    virtual std::string get_name () const { return get_identity(); }

    //! Return the number of parameters that describe the transformation
    virtual unsigned get_nparam () const = 0;

    //! Return a new instance of derived type
    virtual Type* clone () const = 0;

    //! Return true if that is a this
    virtual bool is_a (const Type* that) const;

    //! Return true if this is a T
    template<class T>
    bool is_a () const
    {
      return dynamic_cast<const T*> (this) != 0;
    }

    //! Text interface to calibrator type
    class Interface;

  };
}

#endif
