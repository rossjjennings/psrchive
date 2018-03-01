
#include "Pulsar/ArchiveExtension.h"

#include "Pulsar/ITRFExtension.h"
#include "Pulsar/AuxColdPlasma.h"

#include "interface_factory.h"

Pulsar::Archive::Extension* 
Pulsar::Archive::Extension::factory (const std::string& name_parse)
{
  std::vector< Reference::To<Archive::Extension> > instances;

  instances.push_back( new ITRFExtension );
  instances.push_back( new AuxColdPlasma );

  return TextInterface::factory<Archive::Extension> (instances, name_parse);
}

