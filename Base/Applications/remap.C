/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "remap.h"



using namespace CmdLine;
using namespace std;





/**
 * ParamRemap class.
 **/

ParamRemap::ParamRemap( std::string set_cmdline_name, std::string set_ti_name )
    : cmdline_name( set_cmdline_name ), ti_name( set_ti_name )
{}

std::string ParamRemap::FetchParam( std::string pname )
{
  std::string res;

  if( cmdline_name == pname )
    res = ti->process( pname );

  return res;
}

void ParamRemap::PrintDesc( Reference::To< ParamRemap > target )
{
  string desc = "testing";
  
  cout << target->cmdline_name << "         " << desc << endl;
}





/**
* ParamGroup class.
**/

ParamGroup::ParamGroup( std::string set_group_name )
    : group_name( set_group_name )
{}

void ParamGroup::AddMap( std::string set_cmdline_name, std::string set_ti_name )
{
  mappings.push_back( new ParamRemap( set_cmdline_name, set_ti_name ) );
}

std::string ParamGroup::FetchParam( std::string pname )
{
  std::string result = "";

  vector< Reference::To<ParamRemap> >::iterator it;
  for( it = mappings.begin(); it != mappings.end(); it ++ )
  {
    result = (*it)->FetchParam( pname );
    if( result != "" )
      break;
  }

  return result;
}

void ParamGroup::PrintDesc( Reference::To< ParamGroup > target )
{
  cerr << "--- " << target->group_name << "---------------" << endl;
  
  for_each( target->mappings.begin(), target->mappings.end(), ParamRemap::PrintDesc );
}


/**
 * ParamMap class.
 **/

void ParamMap::AddGrp( std::string name )
{
  current = new ParamGroup( name );
  groups.push_back( current );
}

void ParamMap::AddMap( std::string set_cmdline_name, std::string set_ti_name )
{
  current->AddMap( set_cmdline_name, set_ti_name );
}

std::string ParamMap::FetchParam( std::string pname )
{
  std::string result;

  vector< Reference::To<ParamGroup> >::iterator it;
  for( it = groups.begin(); it != groups.end(); it ++ )
  {
    result = (*it)->FetchParam( pname );
    if( result != "" )
      break;
  }

  return result;
}

void ParamMap::PrintDesc( void )
{
  for_each( groups.begin(), groups.end(), ParamGroup::PrintDesc );
}


