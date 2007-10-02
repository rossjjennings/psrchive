//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/PassbandTI.h,v $
   $Revision: 1.2 $
   $Date: 2007/10/02 04:50:09 $
   $Author: straten $ */



#ifndef __PassbandTI_h
#define __PassbandTI_h


#include <TextInterface.h>
#include "Passband.h"



class PassbandTI : public TextInterface::To<Pulsar::Passband>
{
public:
  PassbandTI();
  PassbandTI( Pulsar::Passband *c );
  void setup( void );
  
  virtual std::string get_interface_name() { return "PassbandTI"; }

  TextInterface::Parser *clone();
};

#endif
