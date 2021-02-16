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
    
  protected:
    
    Reference::To<BinaryStatistic> stat;
    ndArray<2,double> result;

    // what to compare
    std::string what;
    // dimension along which to compare
    std::string way;

    void build ();
    bool built;
    
  };

}

#endif

