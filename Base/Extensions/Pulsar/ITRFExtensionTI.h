


#ifndef __ITRF_Extension_h
#define __ITRF_Extension_h


#include <TextInterface.h>
#include "ITRFExtension.h"



namespace Pulsar
{

  class ITRFExtensionTI : public TextInterface::To< ITRFExtension >
  {
  public:
    ITRFExtensionTI();
    ITRFExtensionTI( ITRFExtension * );
    void SetupMethods( void );
    
    virtual std::string get_interface_name() { return "ITRFExtensionTI"; }

    TextInterface::Class *clone();
  };
}


#endif

