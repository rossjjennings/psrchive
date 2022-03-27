//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ArchiveComparisons.h

#ifndef __ArchiveComparisons_h
#define __ArchiveComparisons_h

#include "Pulsar/ArchiveStatistic.h"
#include "Pulsar/ScrunchFactor.h"

#include "ndArray.h"

class BinaryStatistic;
class UnaryStatistic;

namespace Pulsar {

  class CompareWith;
  
  class ArchiveComparisons : public Identifiable::Proxy<ArchiveStatistic>
  {
 
  public:

    ArchiveComparisons (BinaryStatistic*);
    
    void set_Archive (const Archive*);
  
    double get ();

    class Interface;
  
    TextInterface::Parser* get_interface ();

    ArchiveComparisons* clone () const;

    const std::string& get_what () const { return what; }
    void set_what (const std::string& t) { what = t; }

    const std::string& get_way () const { return way; }
    void set_way (const std::string& t) { way = t; }

    //! Flags for subset of sub-integrations to be computed
    void set_compute_subint (const std::vector<bool>& flags)
    { compute_subint = flags; }

    //! Flags for subset of channels to be computed
    void set_compute_chan (const std::vector<bool>& flags)
    { compute_chan = flags; }

    //! Compute covariance matrix from bscrunched clone of data
    void set_bscrunch (const ScrunchFactor& f);
    
    //! Get the phase bin scrunch factor
    const ScrunchFactor get_bscrunch () const { return bscrunch_factor; }

    //! Archive used to set up
    void set_setup_Archive (const Archive*);

    //! Return true if call to set_setup_Archive sets anything up
    /* Not all archive comparisons require a global set up */
    bool get_setup ();

  protected:

    //! Manages comparisons
    Reference::To<CompareWith> compare;

    //! Performs comparisons
    Reference::To<BinaryStatistic> stat;

    //! Result of comparisons
    ndArray<2,double> result;

    //! Flags for subset of sub-integrations to be computed
    std::vector<bool> compute_subint;

    //! Flags for subset of channels to be computed
    std::vector<bool> compute_chan;

    //! Compute covariance matrix from bscrunched clone of data
    ScrunchFactor bscrunch_factor;

    // what to compare
    std::string what;
    // dimension along which to compare
    std::string way;

    void build ();
    bool built;

    void build_compare ();
    void init_compare (const Archive* arch);
  };

}

#endif

