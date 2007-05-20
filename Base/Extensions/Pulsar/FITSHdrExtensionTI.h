//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/FITSHdrExtensionTI.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/20 23:37:32 $
   $Author: nopeer $ */


#include <TextInterface.h>
#include "FITSHdrExtension.h"



namespace Pulsar {

	class FITSHdrExtensionTI : public TextInterface::To< FITSHdrExtension > {
	
		public:
			FITSHdrExtensionTI();	
	};
}


