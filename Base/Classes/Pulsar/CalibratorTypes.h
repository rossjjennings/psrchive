//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009-2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/CalibratorTypes.h

#ifndef __CalibratorTypes_H
#define __CalibratorTypes_H

#include "Pulsar/CalibratorType.h"

// These preprocessor macros define the contructor and clone method for klass
#define CALIBRATOR_TYPE_INIT(klass,name) \
  klass* clone () const { return new klass; } \
  klass () { set_identity(name); }

#define CALIBRATOR_TYPE(klass) \
  klass* clone () const { return new klass; } \
  klass () // body of constructor to follow

namespace Pulsar
{
  //! Contains the enumeration of all calibrator types
  /*! A separate namespace is used to keep the enumeration separate from
    the definition of the Calibrator::Type class */

  namespace CalibratorTypes
  {
    //! Flux calibrator
    class Flux : public Pulsar::Calibrator::Type
    {
    public:
      CALIBRATOR_TYPE_INIT(Flux,"flux")
      unsigned get_nparam () const { return 4; }
    };

    //! Corrections
    class Corrections : public Pulsar::Calibrator::Type
    {
    public:
      CALIBRATOR_TYPE_INIT(Corrections,"corrections")
      unsigned get_nparam () const { return 0; }
    };
    
    //! Polarization calibrator
    class Poln : public Pulsar::Calibrator::Type
    {
    };
    
    //! Gain, differential gain and differential phase
    class SingleAxis : public Poln
    {
    public:
      CALIBRATOR_TYPE(SingleAxis)
      {
	set_identity("single");
	add_alias("SingleAxis");
      }

      unsigned get_nparam () const { return 3; }
    };
    
    //! van Straten (2002; ApJ 568:436), equation A1
    class van02_EqA1 : public Poln
    {
    public:
      CALIBRATOR_TYPE(van02_EqA1)
      {
	set_identity("van02eA1");
	add_alias("Polar");
      }
      unsigned get_nparam () const { return 6; }
    };
    
    //! Full 7 degrees of freedom parameterization of Jones matrix
    class CompleteJones : public Poln
    {
    public:
      bool is_a (const Type* that) const
      { return dynamic_cast<const CompleteJones*>(that) != 0; }

      CALIBRATOR_TYPE_INIT(CompleteJones, "jones");

      unsigned get_nparam () const { return 7; }
    };

class ManualPoln : public Poln
    {
    public:
      CALIBRATOR_TYPE_INIT(ManualPoln,"manualpoln");

      bool is_a (const Type* that) const { return Type::is_a (that); }
      unsigned get_nparam () const { return 4; }
    };

    //! van Straten (2004; ApJSS 152:129), equation 13
    class van04_Eq13 : public CompleteJones
    {
    public:
      CALIBRATOR_TYPE_INIT(van04_Eq13, "van04e13");
      bool is_a (const Type* that) const { return Type::is_a (that); }
    };

    //! Unpublished polar decomposition
    class van09_Eq : public CompleteJones
    {
    public:
      CALIBRATOR_TYPE(van09_Eq)
      {
	set_identity ("van09");
	add_alias ("Hamaker");
      }
      bool is_a (const Type* that) const { return Type::is_a (that); }
    };
    
    //! Phenomenological parameterizations of Jones matrix
    /*! Separate backend and frontend transformations enable variation of 
      backend to be modeled */
    class Phenomenological : public CompleteJones
    {
      bool is_a (const Type* that) const
      { return dynamic_cast<const Phenomenological*>(that) != 0; }
    };
    
    //! Britton (2000; ApJ 532:1240), equation 19
    class bri00_Eq19 : public Phenomenological
    {
    public:
      CALIBRATOR_TYPE(bri00_Eq19)
      {
        set_identity ("bri00e19");
        add_alias ("Britton");
      }

      bool is_a (const Type* that) const { return Type::is_a (that); }
    };

    //! Britton (2000; ApJ 532:1240), equation 19, with isolated degeneracy
    class bri00_Eq19_iso : public Phenomenological
    {
    public:
      CALIBRATOR_TYPE(bri00_Eq19_iso)
      {
        set_identity ("bri00e19iso");
      }

      bool is_a (const Type* that) const { return Type::is_a (that); }
    };
    
    //! van Straten (2004; ApJSS 152:129), equation 18
    class van04_Eq18 : public Phenomenological
    {
    public:
      CALIBRATOR_TYPE_INIT(van04_Eq18,"van04e18")
      bool is_a (const Type* that) const { return Type::is_a (that); }
    };

    //! Mixes a SingleAxis and Phenomenological parameterization
    class Hybrid : public Poln
    {
    public:
      CALIBRATOR_TYPE_INIT(Hybrid,"hybrid");

      bool is_a (const Type* that) const
      { return dynamic_cast<const Hybrid*>(that) != 0; }

      // 7 Phenomenological + 3 SingleAxis
      unsigned get_nparam () const { return 10; }
    };
    
    //! Ord, van Straten, Hotan & Bailes (2004; MNRAS 352:804), section 2.1
    class ovhb04 : public Hybrid
    {
    public:
      CALIBRATOR_TYPE_INIT(ovhb04,"ovhb04")
      bool is_a (const Type* that) const { return Type::is_a (that); }
    };
    
    //! Calibrate by brute force and unjustified assumptions
    class Nefarious : public Poln
    {
    public:
      unsigned get_nparam () const { return 0; }
    };
    
    //! Degree of Polarization Calibrator (P236)
    class DoP : public Nefarious
    {
    public:
      CALIBRATOR_TYPE_INIT(DoP,"dop")
    };
    
    //! Off-pulse Calibrator (P236)
    class OffPulse: public Nefarious
    {
    public:
      CALIBRATOR_TYPE_INIT(OffPulse,"off")
    };
  }
}

#endif
