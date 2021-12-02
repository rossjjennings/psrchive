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
#include "ndArray.h"

class BinaryStatistic;
class UnaryStatistic;

namespace Pulsar {

  class ArchiveComparisons : public Identifiable::Proxy<ArchiveStatistic>
  {
 
  public:

    ArchiveComparisons (BinaryStatistic*);
    
    void set_Archive (const Archive* arch);
  
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

  protected:
    
    Reference::To<BinaryStatistic> stat;
    ndArray<2,double> result;

    //! Flags for subset of sub-integrations to be computed
    std::vector<bool> compute_subint;

    //! Flags for subset of channels to be computed
    std::vector<bool> compute_chan;
    
    // what to compare
    std::string what;
    // dimension along which to compare
    std::string way;

    void build ();
    bool built;
    
  };

}

#endif

