//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/FrequencyAppend.h

#ifndef _Pulsar_FrequencyAppend_H
#define _Pulsar_FrequencyAppend_H

#include "Pulsar/Append.h"
#include "Pulsar/Config.h"
#include "Functor.h"

namespace Pulsar {

  class Integration;

  //! Algorithms that combine Integration data
  class FrequencyAppend : public Append
  {

  public:

    //! Initialize an archive for appending
    void init (Archive* into);

    //! Throw an exception if there is a reason to not append
    virtual void check (Archive* into, const Archive* from);

    typedef Functor< void (Integration*) > Weight;

    //! The policy used to weight each profile during combine
    static Option< Weight > weight_strategy;

    //! Should a new predictor be always generated?
    static Option< bool > force_new_predictor;

  protected:

    //! Return true if there is a benign reason to stop
    virtual bool stop (Archive* into, const Archive* from);

    //! Add the data in 'from' to 'into'
    virtual void combine (Archive* into, Archive* from);

    //! Add the data in 'from' to 'into'
    virtual void combine (Integration* into, Integration* from);

    /* internal */
    bool check_phase;

    //! Return the policy used to verify that data are mixable
    virtual const Archive::Match* get_mixable_policy (const Archive* into);

    void generate_new_predictor( Archive* into, Archive* from );

  };
  
}

#endif

