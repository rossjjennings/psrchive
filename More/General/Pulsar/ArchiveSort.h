//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ArchiveSort.h,v $
   $Revision: 1.1 $
   $Date: 2006/01/05 15:57:04 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveSort_h
#define __Pulsar_ArchiveSort_h

#include "MJD.h"
#include <vector>

namespace Pulsar {

  class Archive;

  //! Used in sorting archives by source name, frequency, and epoch
  class ArchiveSort {

  public:

    //! Default constructor
    ArchiveSort (const Archive* archive = 0);

    //! The filename of the archive
    std::string filename;

    //! The name of the observed source
    std::string source;

    //! The centre frequency of the observation
    double centre_frequency;

    //! The epoch of the observation
    MJD epoch;

    //! Comparison operator
    friend bool operator < (const ArchiveSort& a, const ArchiveSort& b);

    //! Sort the files according to the rule of the comparison operator
    static void sort (std::vector<std::string>& filenames);

  };

}

#endif
