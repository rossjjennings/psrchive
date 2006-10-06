/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSHdrExtension.h"
#include "strutil.h"

using namespace std;

//! Default constructor
Pulsar::FITSHdrExtension::FITSHdrExtension ()
  : Extension ("FITSHdrExtension")
{
  start_time.Construct("00000.00000");
  hdrver = "UNSET";
  creation_date = "UNSET";
  coordmode = "UNSET";
  trk_mode = "UNSET";
  stt_date = "UNSET";
  stt_time = "UNSET";
  stt_lst = 0.0;
}

//! Copy constructor
Pulsar::FITSHdrExtension::FITSHdrExtension (const FITSHdrExtension& extension)
  : Extension ("FITSHdrExtension")
{
  start_time = extension.start_time;
  hdrver = extension.hdrver;
  creation_date = extension.creation_date;
  coordmode = extension.coordmode;
  trk_mode = extension.trk_mode;
  stt_date = extension.stt_date;
  stt_time = extension.stt_time;
  stt_lst = extension.stt_lst;
}

//! Operator =
const Pulsar::FITSHdrExtension&
Pulsar::FITSHdrExtension::operator= (const FITSHdrExtension& extension)
{
  start_time = extension.start_time;
  hdrver = extension.hdrver;
  creation_date = extension.creation_date;
  coordmode = extension.coordmode;
  trk_mode = extension.trk_mode;
  stt_date = extension.stt_date;
  stt_time = extension.stt_time;
  stt_lst = extension.stt_lst;
  
  return *this;
}

//! Destructor
Pulsar::FITSHdrExtension::~FITSHdrExtension ()
{
}

void Pulsar::FITSHdrExtension::set_coord_mode (const string mode)
{
  if (mode == "J2000" || mode == "Gal" || mode == "Ecliptic" ||
        mode == "AZEL" || mode == "HADEC") {
    coordmode = mode;
  }
  else {
    coordmode = "UNSET";
  }
}

void Pulsar::FITSHdrExtension::set_date_str (const string date)
{
  creation_date = date;
}

void Pulsar::FITSHdrExtension::get_coord_string (const sky_coord& coordinates,
						 string& coord1,
						 string& coord2) const
{
  AnglePair newcoord;
  
  if (coordmode == "J2000") {
    
    newcoord = coordinates.getRaDec();
    
    coord1 = newcoord.angle1.getHMS();
    coord2 = newcoord.angle2.getDMS();
   
    return;

  }

  if (coordmode == "Gal") {
    
    newcoord = coordinates.getGalactic();
    
    coord1 = stringprintf ("%f", newcoord.angle1.getDegrees());
    coord2 = stringprintf ("%f", newcoord.angle2.getDegrees());
    
    return;

  }

  cerr << "WARNING: FITSHdrExtension::get_coord_string COORD_MD = "
       << coordmode << " not implimented" << endl;

}
