//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ArchiveSort.h

#ifndef __Pulsar_ArchiveSort_h
#define __Pulsar_ArchiveSort_h

#include "MJD.h"

#include <list>
#include <iostream>

namespace Pulsar {

  class Archive;

  //! Used in sorting archives by source name, frequency, and epoch
  class ArchiveSort {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    ArchiveSort ();

    //! Construct from from the input stream
    ArchiveSort (std::istream& input);

    //! The filename of the archive
    std::string filename;

    //! The name of the observed source
    std::string source;

    //! The centre frequency of the observation
    double centre_frequency;

    //! The epoch of the observation
    MJD epoch;
    //! The length of the observation
    double length;

    //! Load a list of ArchiveSort instances and sort them
    static void load (std::istream& input, std::list<ArchiveSort>& entries);

  };

  //! Comparison operator
  bool operator < (const ArchiveSort& a, const ArchiveSort& b);

}

#endif
