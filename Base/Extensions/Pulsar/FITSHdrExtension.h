//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FITSHdrExtension.h,v $
   $Revision: 1.13 $
   $Date: 2007/05/20 23:23:17 $
   $Author: nopeer $ */

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
			   std::string& coord1, std::string& coord2) const;

    //! Set the coordinate mode string
    void set_coord_mode (const std::string mode);

    //! Set the creation date string
    void set_date_str (const std::string date);
    
    //! Set the observation mode
    void set_obs_mode( const std::string _obs_mode );
    
    //! Set the end coordinates
    void set_end_coordinates( const sky_coord _end_coord );
    
    //! Set the beam position angle
    void set_bpa( const double _bpa ) { bpa = _bpa; }
    
    //! Set the beam major axis length
    void set_bmaj( const double _bmaj ) { bmaj = _bmaj; }
    
    //! Set the beam minor axis length
    void set_bmin( const double _bmin ) { bmin = _bmin; }
    
    //! Get the observed frequency
    double get_obsfreq( void ) const;
    
    //! Get the header version
    std::string get_hdrver( void ) const;
    
    //! Get the file creation date
    std::string get_creation_date( void ) const;
    
    //! Get the observatio mode
    std::string get_obs_mode( void ) const;
    
    //! Get the coordinate mode
    std::string get_coordmode( void ) const;
    
    //! Get the equinox
    std::string get_equinox( void ) const;
    
    //! Get tracking mode
    std::string get_trk_mode( void ) const;
    
    //! Get the end coordinates
    sky_coord get_end_coordinates( void ) const;
    
    //! Get the beam position angle
    double get_bpa( void ) const { return bpa; }
    
    //! Get the beam major axis length
    double get_bmaj( void ) const { return bmaj; }
    
    //! Get the beam minor axis length
    double get_bmin( void ) const { return bmin; }
    
    //! Get the start date
    std::string get_stt_date( void ) const { return stt_date; }
    
    //! Get the start time
    std::string get_stt_time( void ) const { return stt_time; }
    
    //! Get the stt_imjd
    int get_stt_imjd( void ) const { return start_time.intday(); }
    
    //! Get the stt_smjd
    int get_stt_smjd( void ) const { return start_time.get_secs(); }
    
    //! Get the stt_offs
    double get_stt_offs( void ) const { return start_time.get_fracsec(); }
    
    //! Get stt_lst
    double get_stt_lst( void ) const { return stt_lst; }
    
    //////////////////////////////////////////////////////////////////////
    
    //! Header start time (as opposed to subint start time)
    MJD start_time;
    
    //! Pulsar FITS header version information
    std::string hdrver;
    int major_version;
    int minor_version;

    //! File creation date
    std::string creation_date;

    //! Coordinate mode (EQUAT, GAL, ECLIP, AZEL, HADEC)
    std::string coordmode;
    
    //! Equinox of coordinates
    std::string equinox;

    //! Track mode (TRACK, SCANGC, SCANLAT)
    std::string trk_mode;

    //! Start UT date (YYYY-MM-DD)
    std::string stt_date;

    //! Start UT (hh:mm:ss)
    std::string stt_time;

    //! Start LST
    double stt_lst;
    
    //! Observed Frequency
    double obsfreq;
    
    //! Observation Mode
    std::string obs_mode;
    
    //! The end coordinates of the scan
    sky_coord end_coord;
    
    //! Beam position angle
    double bpa;
    
    //! Beam major axis length
    double bmaj;
    
    //! Beam minor axis length
    double bmin;
    
  };
  
}

#endif

