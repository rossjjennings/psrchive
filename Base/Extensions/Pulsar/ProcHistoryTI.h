//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by David smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/ProcHistoryTI.h,v $
   $Revision: 1.1 $
   $Date: 2007/06/20 03:04:54 $
   $Author: nopeer $ */

#ifndef __Pulsar_ProcHistoryTI_h
#define __Pulsar_ProcHistoryTI_h

#include "Pulsar/ProcHistory.h"
#include "TextInterface.h"

namespace Pulsar
{

  //! Provides a text interface to get and set ProcHistory attributes
  class ProcHistoryTI : public TextInterface::To<ProcHistory>
  {
  public:

    //! Constructor
    ProcHistoryTI ();
    ProcHistoryTI ( ProcHistory *c );
    void setup( void );

    virtual std::string get_interface_name() { return "ProcHistoryTI"; }

    TextInterface::Class *clone();
  };

}

#endif
