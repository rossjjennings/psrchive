/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#include "VAPTI.h"
#include <string>



using namespace Pulsar;
using namespace std;




/**
 * setup_exceptions - For any parameters that we want to access through our overloaded process function that
 *                    are not already in the ArchiveTI, add them here. The function pointed to doesn't matter
 *                    as it will never be called, but the help text will be used.
 **/

void VapArchiveTI::setup_exceptions( void )
{
  // add( &Archive::get_bandwidth, "bw", "Bandwidth (MHz)" );

  add( &Archive::get_bandwidth, "para", "Parallactic angle at archive mid point" );
  add( &Archive::get_bandwidth, "tsub", "The duration of the first subint (s)" );
}



/**
 * get_length - get the length with 6 digits of precision
 **/

string VapArchiveTI::get_length( void )
{
  stringstream result;

  result << "ArchiveTI-length=";

  // if we don't have any integrations, return length unset
  int nsub = instance->get_nsubint();
  if( nsub != 0 )
  {
    result << setiosflags( ios::fixed ) << setprecision( 6 );
    result << instance->integration_length();

    //     // get the first integration, get its length
    //     Reference::To<Integration> first_int = instance->get_first_Integration();
    //
    //     if( first_int )
    //     {
    //       double sub_length = first_int->get_duration();
    //       double total_length = sub_length * nsub;
    //       result << total_length;
    //     }
  }

  if( result.str() == "" )
    result << "*";

  return result.str();
}



/**
 * get_bw - Modified "bw", we just want to set the precision
 **/

string VapArchiveTI::get_bw( void )
{
  stringstream result;

  double bw = instance->get_bandwidth();
  result << "ArchiveTI-bw=";
  result << setiosflags( ios::fixed ) << setprecision( 3 ) << bw;

  return result.str();
}


/**
 * get_para - Treat this as an exception until I decide where to add it. 
 **/

string VapArchiveTI::get_para( void )
{
  stringstream result;

  result << "ArchiveTI-para=";
  int nsubs = instance->get_nsubint();

  if( nsubs != 0 )
  {
    Reference::To< Integration > integration = instance->get_Integration( nsubs / 2 );
    if( integration )
    {
      Reference::To< Pointing > ext = integration->get<Pointing>();

      if( ext )
      {
        result << ext->get_parallactic_angle().getDegrees();
      }
    }
  }

  if( result.str() == "" )
    result << "*";

  return result.str();
}


/**
 * get_tsub - Get the duration of the first sub int
 **/

string VapArchiveTI::get_tsub( void )
{
  stringstream result;

  result << "ArchiveTI-tsub=";

  int nsubs = instance->get_nsubint();
  if( nsubs != 0 )
  {
    Reference::To< Integration > first_int = instance->get_first_Integration();
    if( first_int )
    {
      result << setiosflags( ios::fixed ) << setprecision( 6 ) << first_int->get_duration();
    }
  }

  if( result.str() == "" )
    result << "*";

  return result.str();
}


/**
 * process - Our modified version of the process function, try and use our exeption functions to get
 *           the data first, if that fails, then call the parent process function.
 **/

string VapArchiveTI::process ( const std::string& command )
{
  string result;

  if( command == "ArchiveTI-length" )
    result = get_length();
  else if( command == "ArchiveTI-bw" )
    result = get_bw();
  else if( command == "ArchiveTI-para" )
    result = get_para();
  else if( command == "ArchiveTI-tsub" )
    result = get_tsub();
  else
    result = ArchiveTI::process( command );

  if( result == "" )
    result = command + string("=*");

  return result;
}



