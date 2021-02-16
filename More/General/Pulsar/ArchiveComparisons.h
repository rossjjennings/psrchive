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

  protected:
    
    Reference::To<BinaryStatistic> stat;

    ndArray<2,double> result;

    void build ();
    bool built;
    
  };

}

#endif

