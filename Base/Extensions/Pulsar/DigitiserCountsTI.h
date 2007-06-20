//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/DigitiserCountsTI.h,v $
   $Revision: 1.1 $
   $Date: 2007/06/20 03:04:53 $
   $Author: nopeer $ */

#ifndef __Pulsar_DigitiserCountsTI_h
#define __Pulsar_DigitiserCountsTI_h

#include "Pulsar/DigitiserCounts.h"
#include "TextInterface.h"

namespace Pulsar
{
  //! Provides a text interface to get and set DigitiserCounts attributes
  class DigitiserCountsTI : public TextInterface::To<DigitiserCounts>
  {

  public:

    //! Constructor
    DigitiserCountsTI () { setup(); }
    DigitiserCountsTI ( DigitiserCounts *c ) { setup(); set_instance( c ); }
    void setup( void );

    virtual std::string get_interface_name() { return "DigitiserCountsTI"; }

    TextInterface::Class *clone();
  };

}

#endif
