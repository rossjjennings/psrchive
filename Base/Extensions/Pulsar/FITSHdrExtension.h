//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FITSHdrExtension.h,v $
   $Revision: 1.3 $
   $Date: 2003/06/19 03:48:44 $
   $Author: ahotan $ */

#ifndef __FITSHdrExtension_h
#define __FITSHdrExtension_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! FITSArchive specific seader extensions
  
  class FITSHdrExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    FITSHdrExtension ();

    //! Copy constructor
    FITSHdrExtension (const FITSHdrExtension& extension);

    //! Operator =
    const FITSHdrExtension& operator= (const FITSHdrExtension& extension);
    
    //! Destructor
    ~FITSHdrExtension ();

    //////////////////////////////////////////////////////////////////////
    
    //! Header start time (as opposed to subint start time)
    MJD start_time;
    
    //! Pulsar FITS header version information
    string hdrver;
    
    //! File creation date
    string creation_date;

    //! Coordinate mode (J2000, Gal, Ecliptic, AZEL, HADEC)
    string coordmode;
    
    //! Track mode (TRACK, SCANGC, SCANLAT)
    string trk_mode;

    //! Start UT date (YYYY-MM-DD)
    string stt_date;

    //! Start UT (hh:mm:ss)
    string stt_time;

    //! Start LST
    double stt_lst;
    
  };
  
}

#endif

