//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/FluxCalibratorExtensionTI.h,v $
   $Revision: 1.1 $
   $Date: 2007/06/20 03:04:53 $
   $Author: nopeer $ */



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

  TextInterface::Class *clone();
};

#endif
