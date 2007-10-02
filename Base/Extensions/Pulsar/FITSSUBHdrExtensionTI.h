//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/FITSSUBHdrExtensionTI.h,v $
   $Revision: 1.2 $
   $Date: 2007/10/02 04:50:09 $
   $Author: straten $ */

#ifndef __Pulsar_FITSSUBHdrExtensionTI_h
#define __Pulsar_FITSSUBHdrExtensionTI_h

#include "Pulsar/FITSSUBHdrExtension.h"
#include "TextInterface.h"

namespace Pulsar
{

//   template< class E > class ExtensionTI : public TextInterface::To<E>
//   {
//   public:
//     ExtensionTI() { setup(); }
//     ExtensionTI( E *c ) { setup(); set_instance( c ); }
// 
//     virtual void setup( void ) = 0;
// 
//     virtual std::string get_interface_name() = 0;
// 
//     TextInterface::Parser *clone()
//     {
//       if( instance )
//         return new ExtensionTI( instance );
//       else
//         return new ExtensionTI();
//     }
//   };




  //! Provides a text interface to get and set FITSSUBHdrExtension attributes
  class FITSSUBHdrExtensionTI : public TextInterface::To< FITSSUBHdrExtension >
  {

  public:

    //! Constructor
    FITSSUBHdrExtensionTI ();
    FITSSUBHdrExtensionTI ( FITSSUBHdrExtension *c );
    void setup( void );

    virtual std::string get_interface_name() { return "FITSSUBHdrExtensionTI"; }

    TextInterface::Parser *clone();
  };

}

#endif
