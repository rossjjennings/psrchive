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
  equinox  = "UNSET";
  trk_mode = "UNSET";
  stt_date = "UNSET";
  stt_time = "UNSET";
  stt_lst = 0.0;
  obsfreq = 0.0;
}

//! Copy constructor
Pulsar::FITSHdrExtension::FITSHdrExtension (const FITSHdrExtension& extension)
    : Extension ("FITSHdrExtension")
{
  operator=(extension);
}

//! Operator =
const Pulsar::FITSHdrExtension&
Pulsar::FITSHdrExtension::operator= (const FITSHdrExtension& extension)
{
  start_time = extension.start_time;
  hdrver = extension.hdrver;
  creation_date = extension.creation_date;
  coordmode = extension.coordmode;
  equinox = extension.equinox;
  trk_mode = extension.trk_mode;
  stt_date = extension.stt_date;
  stt_time = extension.stt_time;
  stt_lst = extension.stt_lst;

  return *this;
}

//! Destructor
Pulsar::FITSHdrExtension::~FITSHdrExtension ()
{}



TextInterface::Parser* Pulsar::FITSHdrExtension::get_interface()
{
  return new Interface( this );
}

void Pulsar::FITSHdrExtension::set_coord_mode (const string mode)
{
  if (mode == "EQUAT")
    coordmode = "J2000";

  else if (mode == "Gal")
    coordmode = "GAL";

  else if (mode == "J2000" ||
           mode == "GAL" ||
           mode == "ECLIP" ||
           mode == "AZEL" ||
           mode == "HADEC")
    coordmode = mode;

  else
    coordmode = "UNSET";
}

void Pulsar::FITSHdrExtension::set_date_str (const string date)
{
  creation_date = date;
}



void Pulsar::FITSHdrExtension::set_obs_mode( const string _obs_mode )
{
  obs_mode = _obs_mode;
}


void Pulsar::FITSHdrExtension::get_coord_string (const sky_coord& coordinates,
    string& coord1,
    string& coord2) const
{
  AnglePair newcoord;

  if (coordmode == "J2000")
  {

    newcoord = coordinates.getRaDec();

    coord1 = newcoord.angle1.getHMS();
    coord2 = newcoord.angle2.getDMS();

    return;

  }

  if (coordmode == "GAL")
  {

    newcoord = coordinates.getGalactic();

    coord1 = tostring (newcoord.angle1.getDegrees());
    coord2 = tostring (newcoord.angle2.getDegrees());

    return;

  }

  cerr << "FITSHdrExtension::get_coord_string WARNING COORD_MD = "
  << coordmode << " not implemented" << endl;

}

void Pulsar::FITSHdrExtension::set_obsfreq( double set_obsfreq )
{
	obsfreq = set_obsfreq;
}


double Pulsar::FITSHdrExtension::get_obsfreq( void ) const
{
  return obsfreq;
}


std::string Pulsar::FITSHdrExtension::get_hdrver( void ) const
{
  return hdrver;
}


std::string Pulsar::FITSHdrExtension::get_creation_date( void ) const
{
  return creation_date;
}


std::string Pulsar::FITSHdrExtension::get_obs_mode( void ) const
{
  return obs_mode;
}



std::string Pulsar::FITSHdrExtension::get_coordmode( void ) const
{
  return coordmode;
}

std::string Pulsar::FITSHdrExtension::get_equinox( void ) const
{
  return equinox;
}


std::string Pulsar::FITSHdrExtension::get_trk_mode( void ) const
{
  return trk_mode;
}


// void Pulsar::FITSHdrExtension::set_end_coordinates( sky_coord _end_coord )
// {
//   end_coord = _end_coord;
// }


// sky_coord Pulsar::FITSHdrExtension::get_end_coordinates( void ) const
// {
//   return end_coord;
// }
// 
// 
// string Pulsar::FITSHdrExtension::get_stp_crd1( void ) const
// {
//   sky_coord coord = get_end_coordinates();
// 
//   int hours, mins;
//   double seconds;
//   coord.ra().getHMS( hours, mins, seconds );
// 
//   stringstream result;
//   result << hours << ":" << mins << ":" << seconds;
// 
//   return result.str();
// }
// 
// 
// string Pulsar::FITSHdrExtension::get_stp_crd2( void ) const
// {
//   sky_coord coord = get_end_coordinates();
// 
//   int hours, mins;
//   double seconds;
//   coord.ra().getDMS( hours, mins, seconds );
// 
//   stringstream result;
//   result << hours << ":" << mins << ":" << seconds;
// 
//   return result.str();
// }



// string Pulsar::FITSHdrExtension::get_stt_crd1( void ) const 
// {
//   return stt_crd1;
// }
// 
// 
// string Pulsar::FitsHdrExtension::get_stt_crd2( void ) const
// {
//   return stt_crd2;
// }
// 
// 
// string Pulsar::FITSHdrExtension::get_ra_HMS( void ) const
// {
//   sky_coord coords;
// 
//   if( get_coordmode() == "GAL" )
//   {
//     coords = coordinates.getGalactic();
//   }
//   else if( get_coord_mode() == "J2000" )
//   {
//     coords = coordinates;
//   }
// 
//   return coords.ra().getHMS();
// }
// 
// 
// string Pulsar::FITSHdrExtension::get_dec_DMS( void ) const
// {
//   sky_coord coords;
// 
//   if( get_coordmode() == "GAL" )
//   {
//     coords = coordinates.getGalactic();
//   }
//   else if( get_coord_mode() == "j2000" )
//   {
//     coords = coordinates;
//   }
// 
//   return coords.dec().getDMS();
// }






