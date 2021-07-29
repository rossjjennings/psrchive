//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ScrunchFactor.h

#ifndef __Pulsar_ScrunchFactor_h
#define __Pulsar_ScrunchFactor_h

#include "Error.h"

namespace Pulsar {

  //! Manages a combined scrunch factor or target dimension
  class ScrunchFactor 
  {
    //! Number of elements to scrunch to
    unsigned n_result;
    //! Number of elements to be integrated
    unsigned n_scrunch;

  public:

    //! Useful for passing default = disabled
    static const ScrunchFactor none;
    
    //! Default constructor
    ScrunchFactor (unsigned nresult = 1) { set_nresult (nresult); }

    //! Get the number of elements to scrunch to
    unsigned get_nresult () const { return n_result; }
    //! Set the number of elements to scrunch to
    void set_nresult (unsigned value) { n_result = value; n_scrunch = 0; }

    //! Get the number of elements to be integrated
    unsigned get_nscrunch () const { return n_scrunch; }
    //! Set the number of elements to be integrated
    void set_nscrunch (unsigned value) { n_scrunch = value; n_result = 0; }

    //! Return true if scrunch should be performed
    bool scrunch_enabled () const { return n_scrunch || n_result; }

    //! Disable scrunching
    void disable_scrunch () { n_scrunch = n_result = 0; }

    //! Return the number of elements after scrunch
    unsigned get_nresult (unsigned size) const;

    //! Return the number of elements to be integrated
    unsigned get_nscrunch (unsigned size) const;
  };

  std::ostream& operator << (std::ostream& os, const ScrunchFactor&);
  std::istream& operator >> (std::istream& is, ScrunchFactor&);

  //! Integrate frequency channels
  template<class Container>
  void fscrunch (Container*, const ScrunchFactor&);

  //! Integrate sub-integrations
  template<class Container>
  void tscrunch (Container*, const ScrunchFactor&);

  //! Integrate phase bins
  template<class Container>
  void bscrunch (Container*, const ScrunchFactor&);

}

//! Integrate frequency channels
template<class Container, class getN, class doScrunch>
void scrunch (Container* container, getN get_size, doScrunch scrunch,
	      const Pulsar::ScrunchFactor& factor)
{
  if (factor.get_nresult())
  {
    unsigned size = (container->*get_size) ();
    unsigned new_size = factor.get_nresult();
    
    if (size % new_size)
      throw Error (InvalidParam, "scrunch (ScrunchFactor) template",
		   "requested size=%u modulo current size=%u is non-zero",
		   new_size, size);

    unsigned nscrunch = size / new_size;
    (container->*scrunch) ( nscrunch );
  }
  else if (factor.get_nscrunch())
  {
    (container->*scrunch) ( factor.get_nscrunch() );
  }
  else
    throw Error (InvalidState, "scrunch (ScrunchFactor) template",
                 "neither nresult nor nscrunch set");
}

//! Integrate frequency channels
template<class Container>
void Pulsar::fscrunch (Container* container, const ScrunchFactor& factor)
{
  scrunch (container, &Container::get_nchan, &Container::fscrunch, factor);
}

//! Integrate sub-integrations
template<class Container>
void Pulsar::tscrunch (Container* container, const ScrunchFactor& factor)
{
  scrunch (container, &Container::get_nsubint, &Container::tscrunch, factor);
}

//! Integratee phase bins
template<class Container>
void Pulsar::bscrunch (Container* container, const ScrunchFactor& factor)
{
  scrunch (container, &Container::get_nbin, &Container::bscrunch, factor);
}

#endif

