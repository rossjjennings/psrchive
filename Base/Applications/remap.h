/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#ifndef __remap_h
#define __remap_h



#include <vector>
#include <Reference.h>
#include <TextInterface.h>
#include <iostream>



/**
* Usage
*
* A TextInterface::To< C > typically wants to identify members by short names, eg "name" is
* ok if we are the TextInterface to a BackendExtension, and "name" is also ok if we are a 
* TextInterface to an Archive. To differentiate between the two, we add prefixes to the 
* text identifying the property ie "backend:name", but we don't want to have to write out
* strings like this on the command line where we are not after all parameters, just a set 
* of usefull ones. Also, programs such as vap have established commands that should be
* preserved for compatibility.
*
* The CmdMap class performs two functions, it allows us to list the mappings from what we
* expect on the command line to the fully qualified name that we can search in all 
* TextInterfaces for. It also allows you to group the commands according to some logic other
* than the way they are represented internally in psrchive.
*
* Typically inherit from this class and add similar to the following in your constructor.
* 
* AddGrp( "COMMON" );
* AddCmd( "freq", "freq" );
* AddCmd( "name", "backend-name" );
* AddGrp( "FITS" );
* ...
*
* To query a parameter
*
* Process( "cmdline_name" );
*
* To print out a help page describing the commands, ordered by their groups
*
**/







namespace CmdLine
{


  /** A single mapping of cmdline_name, to ti_name and TextInterface to use
  **/

  class ParamRemap : public Reference::Able
  {
  public:
    ParamRemap( std::string set_cmdline_name, std::string set_ti_name );
    ParamRemap( std::string set_cmdline_name, std::string set_ti_name, Reference::To< TextInterface::Class > set_ti);

    std::string FetchParam( std::string pname );

    static void PrintDesc( Reference::To< ParamRemap > target );
  private:
    std::string cmdline_name;
    std::string ti_name;
    Reference::To< TextInterface::Class > ti;
  };

  /** A group of mappings given a name
  **/

  class ParamGroup : public Reference::Able
  {
  private:

  public:
    ParamGroup( std::string set_group_name );

    //! Adds a new ParamRemap to this group
    void AddMap( std::string set_cmdline_name, std::string set_ti_name );
    void AddMap( std::string set_cmdline_name, std::string set_ti_name, Reference::To< TextInterface::Class > set_ti );

    std::string FetchParam( std::string pname );
    static void PrintDesc( Reference::To< ParamGroup > target );

  private:

    std::string group_name;
    std::vector< Reference::To<ParamRemap> > mappings;
  };

  /** A collection of groups of mappings 
  **/

  class ParamMap
  {
  public:
    ParamMap() {}
    ~ParamMap() {}

    void AddGrp( std::string set_name );

    void AddMap( std::string set_cmdline_name, std::string set_ti_name );
    void AddMap( std::string set_cmdline_name, std::string set_ti_name, Reference::To< TextInterface::Class > set_ti );

    std::string FetchParam( std::string pname );

    void PrintDesc( void );

  private:
    std::vector< Reference::To<ParamGroup> > groups;
    Reference::To<ParamGroup> current;
  };

}

#endif


