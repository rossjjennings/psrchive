//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/FluxCalibratorExtensionTI.h,v $
   $Revision: 1.2 $
   $Date: 2007/10/02 04:50:09 $
   $Author: straten $ */



#ifndef __FluxCalibratorExtensionTI_h
#define __FluxCalibratorExtensionTI_h


#include <TextInterface.h>
#include "FluxCalibratorExtension.h"



class FluxCalibratorExtensionTI : public TextInterface::To<Pulsar::FluxCalibratorExtension>
{
public:
  FluxCalibratorExtensionTI();
  FluxCalibratorExtensionTI( Pulsar::FluxCalibratorExtension *c );
  void setup( void );
  
  virtual std::string get_interface_name() { return "FluxCalibratorExtensionTI"; }

  TextInterface::Parser *clone();
};

#endif
