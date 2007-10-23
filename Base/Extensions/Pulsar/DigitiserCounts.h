//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/DigitiserCounts.h,v $
   $Revision: 1.4 $
   $Date: 2007/10/23 01:25:45 $
   $Author: nopeer $ */

#ifndef __Pulsar_DigitiserCounts_h
#define __Pulsar_DigitiserCounts_h

#include "Pulsar/Archive.h"
#include <TextInterface.h>



namespace Pulsar
{ 
  class DigitiserCounts : public Archive::Extension
  {
  public:
    //Default constructor
    DigitiserCounts ();

    // Copy constructor
    DigitiserCounts (const Pulsar::DigitiserCounts& extension);

    // Operator =
    const DigitiserCounts& operator= (const DigitiserCounts& extension);

    // Destructor
    ~DigitiserCounts ();

    //! Clone method
    DigitiserCounts* clone () const { return new DigitiserCounts( *this ); }

    //! Get the text interface
    TextInterface::Parser* get_interface();

    class Interface : public TextInterface::To<DigitiserCounts>
    {
      public:
	Interface( DigitiserCounts *s_instance );
    };
    
    //! class to represent a row 
    class row
    {
      public:
	row();
	~row();
	
	std::vector<float> data;
    };

    void set_dig_mode( std::string s_dig_mode ) { dig_mode = s_dig_mode; }
    void set_nlev( int s_nlev ) { nlev = s_nlev; }
    void set_npthist( int s_npthist ) { npthist = s_npthist; }
    void set_diglev( std::string s_diglev ) { diglev = s_diglev; }

    std::string get_dig_mode( void ) const { return dig_mode; }
    int get_nlev( void ) const { return nlev; }
    int get_npthist( void ) const { return npthist; }
    std::string get_diglev( void ) const { return diglev; }
  private:
    std::string dig_mode;
    int nlev;
    int npthist;
    std::string diglev;
  };
}

#endif
