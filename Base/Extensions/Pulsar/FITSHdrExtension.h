//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FITSHdrExtension.h,v $
   $Revision: 1.9 $
   $Date: 2006/08/09 02:07:23 $
   $Author: redwards $ */

#ifndef __FITSHdrExtension_h
#define __FITSHdrExtension_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! Stores PSRFITS header extensions
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

    //! Clone method
    FITSHdrExtension* clone () const { return new FITSHdrExtension( *this ); }

    //! Return two strings based on coordmode attribute
    void get_coord_string (const sky_coord& coordinates,
			   string& coord1, string& coord2) const;

    //! Set the coordinate mode string
    void set_coord_mode (const string mode);

    //! Set the creation date string
    void set_date_str (const string date);

    //////////////////////////////////////////////////////////////////////
    
    //! Header start time (as opposed to subint start time)
    MJD start_time;
    
    //! Pulsar FITS header version information
    string hdrver;
    int major_version;
    int minor_version;

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

