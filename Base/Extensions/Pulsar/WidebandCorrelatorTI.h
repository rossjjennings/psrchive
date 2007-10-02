




#include <TextInterface.h>
#include "WidebandCorrelator.h"



namespace Pulsar
{

  class WidebandCorrelatorTI : public TextInterface::To< WidebandCorrelator >
  {
  public:
    WidebandCorrelatorTI();
    WidebandCorrelatorTI( WidebandCorrelator *c );
    void SetupMethods( void );
    
    virtual std::string get_interface_name() { return "WidebandCorrelatorTI"; }

    TextInterface::Parser *clone();
  };
}

