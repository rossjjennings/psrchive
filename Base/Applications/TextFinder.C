/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "TextFinder.h"
#include <iostream>
#include <Pulsar/ExtensionRegister.h>
#include <strutil.h>
#include <Pulsar/ArchiveTI.h>
#include <Pulsar/ProcHistory.h>
#include <Pulsar/FITSHdrExtension.h>
#include <Pulsar/Pointing.h>
#include <Pulsar/Integration.h>
#include "VAPTI.h"


using namespace std;
using namespace Pulsar;




void TextMappingSet::AddGrp( string new_group_name )
{
  current_group_name = new_group_name;

  mapping_type new_set;
  mappings[current_group_name] = new_set;
}


void TextMappingSet::AddMap( string short_name, string qualified_name )
{
  mappings[current_group_name][short_name] =qualified_name;
}


void TextMappingSet::ShowMappings( void )
{
  mapping_table_type::iterator it;
  for( it = mappings.begin(); it != mappings.end(); it ++ )
  {
    cout << "next group is " << (*it).first << endl;
    mapping_type next_set = (*it).second;
    mapping_type::iterator git;
    for( git = next_set.begin(); git != next_set.end(); git ++ )
    {
      cout << "next map is " << (*git).first << " -> " << (*git).second << endl;
    }
  }
}



////////////////////////////////////////////////////////////////////////

TextFinder::TextFinder()
{
  GetStaticTIs();
}



TextFinder::~TextFinder()
{}



string TextFinder::GetHelpForParameter( string qualified_name )
{
  string help_text;

  // iterate through each of the static tis
  //    get the ti name
  //    break the qualified name on the : character
  //    if the first part of the qualified name matches the ti name
  //       iterate through the parameters in the ti
  //         if the parameter matches the second part of the qualified name
  //            get the description for that parameter

  string ti_name, p_name;
  string_split( qualified_name, ti_name, p_name, "-" );

  text_interface_v_type::iterator sit;
  for( sit = static_tis.begin(); sit != static_tis.end(); sit ++ )
  {
    string next_ti_name = (*sit)->get_interface_name();

    // cerr << "comparing " << next_ti_name << " to " << ti_name << endl;

    if( ti_name == next_ti_name )
    {
      unsigned int n = (*sit)->get_nattribute();
      for( unsigned int i = 0; i < n; i ++ )
      {
        string next_p_name = (*sit)->get_name(i);
        if( next_p_name == p_name )
        {
          help_text = (*sit)->get_description(i);
          break;
        }
      }
      // cerr << "found interface " << ti_name << " which will provide help for " << qualified_name << endl;
    }
    if( help_text != "" )
      break;
  }

  return help_text;
}



void TextFinder::DisplayHelp( void )
{
  // iterate through each of the groups
  //   display a heading for the group
  //   iterate through each mapping in the group
  //      iterate through each static extension
  //          if the mapping is for that extension
  //              output the help for that parameter

  mapping_table_type::iterator t_it;
  for( t_it = mappings.begin(); t_it != mappings.end(); t_it ++ )
  {
    cout << endl;
    cout << "            " << setfill('g') << pad( 100, (*t_it).first ) << endl;
    mapping_type::iterator m_it;
    for( m_it = (*t_it).second.begin(); m_it != (*t_it).second.end(); m_it ++ )
    {
      cout << pad( 30, (*m_it).first ) << "\t";
      cout << GetHelpForParameter( (*m_it).second );
      cout << endl;
    }
  }
}


string TextFinder::FetchValue( string short_name )
{
  string result;
  string qualified_name = GetQualified( short_name );

  vector< Reference::To< TextInterface::Class > >::iterator it;
  for( it = current_tis.begin(); it != current_tis.end(); it ++ )
  {
    try
    {
      //cerr << "processing " << qualified_name << " on " << (*it)->get_interface_name() << endl;
      string qualified_result = (*it)->process( qualified_name );
      result = short_name + qualified_result.substr( qualified_name.size() );
      //cerr << "succeeded in getting parameter" << endl;
    }
    catch ( Error e ) { /*cerr << "attempt to process ti failed" << e << endl;*/ }
  }

  return result;
}

//! For each parameter, add an alias with the extensionname: infront of it, ie ( for name, add ObsExtension:name).

void TextFinder::AddAliases( Reference::To< TextInterface::Class > src )
{
  Reference::To< Alias > new_alias = new Alias();

  string prefix = src->get_interface_name();

  int num_attributes = src->get_nattribute();
  for( unsigned int i = 0; i < num_attributes; i ++ )
  {
    //cerr << " adding " << prefix << string(":") << src->get_name(i) << " to "  << src->get_name(i) << endl;
    new_alias->add( prefix + string("-") + src->get_name( i ), src->get_name( i ) );
  }

  src->set_aliases( new_alias );
}


void TextFinder::GetStaticTIs( void )
{
  Reference::To< TextInterface::Class > main_ti = new VapArchiveTI();
  AddAliases( main_ti );
  static_tis.push_back( main_ti );

  vector< Reference::To< Archive::Extension > >::iterator it;

  for( it = extension_registry.begin(); it != extension_registry.end(); it ++ )
  {
    try
    {
      Reference::To< TextInterface::Class > new_ti = (*it)->get_text_interface();
      if( new_ti )
      {
        AddAliases( new_ti );
        static_tis.push_back( new_ti );
      }
    }
    catch ( Error e )
    {
      cerr << e << endl;
    }
  }
}



string TextFinder::GetQualified( string short_name )
{
  string qualified_name;

  mapping_table_type::iterator it;
  for( it = mappings.begin(); it != mappings.end(); it ++ )
  {
    mapping_type::iterator mit = (*it).second.find(short_name);
    if( mit != (*it).second.end() )
    {
      qualified_name = (*mit).second;
    }
  }

  return qualified_name;
}

//////////////////////////////////////////////////////////////////////////////////////////////




ArchiveTextFinder::ArchiveTextFinder()
{}


ArchiveTextFinder::~ArchiveTextFinder()
{}

void ArchiveTextFinder::SetArchive( Reference::To< Pulsar::Archive > set_archive )
{
  if( !set_archive )
    return;

  current_archive = set_archive;

  // for each extension
  //   get the text interface for the extension
  //   add aliases for the text interface
  //   add the text interface to current_tis

  current_tis.clear();

  for( int i = 0; i < current_archive->get_nextension(); i ++ )
  {
    Reference::To< Archive::Extension > next_ext = current_archive->get_extension(i);
    Reference::To< TextInterface::Class > next_int = next_ext->get_text_interface();
    if( next_int )
    {
      AddAliases( next_int  );
      current_tis.push_back( next_int );
    }
  }

  Reference::To< TextInterface::Class > main_ti = new VapArchiveTI( current_archive );
  AddAliases( main_ti );
  current_tis.push_back( main_ti );
}


string ArchiveTextFinder::FetchValue( string param )
{
  string result;

  // process any parameters which are handled separatetly from normal tis
  // if we don'thave a result
  //    process the paramater through the normal tis
  // if we don't have an = in the result
  //    add =UNDEF to the result
  // otherwise if we have = but nothing else
  //    add =* to indicate no value

  result = ProcessExceptions( param );

  if( result == "" )
    result = TextFinder::FetchValue( param );

  if( result.find_last_of('=') == -1 )
  {
    result = param + string("=UNDEF");
  }
  else if( (result.substr(result.find_last_of('=')+1)) == "" )
  {
    result = param + string("=*");
  }

  return result;
}



//! There is a list of parameters we can't just grab from the text interfaces atm for various reasons

string ArchiveTextFinder::ProcessExceptions( string parameter )
{
  ostringstream result;

  Reference::To<Pulsar::FITSHdrExtension> fits_ext = current_archive->get<Pulsar::FITSHdrExtension>();

  Reference::To< ProcHistory > history = current_archive->get<ProcHistory>();
  bool have_history_rows = false;
  if( history )
    if( history->rows.size() > 0 )
      have_history_rows = true;


  if( parameter == "freq" )
  {
    double cf = current_archive->get_centre_frequency();
    result << parameter << "=";
    result << setiosflags( ios::fixed ) << setprecision(3) << cf;
  }
  else if( parameter == "stt_crd1" || parameter == "stt_crd2" )
  {
    sky_coord coord = current_archive->get_coordinates();

    int hours, mins;
    double seconds;
    if( parameter == "stt_crd1" )
      coord.ra().getHMS( hours, mins, seconds );
    else
      coord.dec().getDMS( hours, mins, seconds );

    result << parameter << "=";
    result << hours << ":" << mins << ":" << seconds;
  }


  return result.str();
}











