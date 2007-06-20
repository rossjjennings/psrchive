/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/





#ifndef __Text_Finder_h
#define __Text_Finder_h




#include <map>
#include <vector>
#include <string>
#include <TextInterface.h>
#include <Pulsar/Archive.h>



using namespace std;





/**
 * A simple map of maps, [ groupname -> [ shortname -> qualifiedname ] ]
 **/

class TextMappingSet
{
public:
  TextMappingSet(){}
  virtual ~TextMappingSet(){}

  void AddGrp( string new_grp_name );
  void AddMap( string short_name, string qualified_name );
  void ShowMappings( void );

  typedef map< string, string > mapping_type;
  typedef map< string, mapping_type > mapping_table_type;
protected:

  mapping_table_type mappings;
  string current_group_name;
};

/**
 * Keep a list of text interfaces, allow the user to fetch a value
 **/

class TextFinder : public TextMappingSet
{
public:
  TextFinder();
  ~TextFinder();

  virtual void DisplayHelp( void );
  virtual string FetchValue( string short_name );
  
  typedef Reference::To< TextInterface::Class > text_interface_ptr_type;
  typedef vector< text_interface_ptr_type > text_interface_v_type;

protected:
  string GetQualified( string short_name );
  string GetHelpForParameter( string qualified_name );

  text_interface_v_type static_tis;
  text_interface_v_type current_tis;

  void AddAliases( text_interface_ptr_type src );
  void GetStaticTIs( void );
};


/**
 * A finder to find data in an archive and its derived extensions.
 **/

class ArchiveTextFinder : public TextFinder
{
public:
  ArchiveTextFinder();
  ~ArchiveTextFinder();

  void SetArchive( Reference::To< Pulsar::Archive > set_archive );
  
  string ProcessExceptions( string parameter );

  string FetchValue( string short_name );
private:
  Reference::To< Pulsar::Archive > current_archive;
};


#endif





