


#include <TextInterface.h>
#include "ObsExtension.h"




namespace Pulsar
{

  class ObsExtensionTI : public TextInterface::To< ObsExtension >
  {

  public:
    ObsExtensionTI();
    ObsExtensionTI( ObsExtension *c );
    void SetupMethods( void );
    
    virtual std::string get_interface_name() { return "ObsExtensionTI"; }

    TextInterface::Parser *clone();
  };
}

