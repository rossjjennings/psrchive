
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/ITRFExtension.h"
#include "Pulsar/AuxColdPlasma.h"

#include "interface_factory.h"
#include <iostream>

using namespace std;

Pulsar::Archive::Extension* 
Pulsar::Archive::Extension::factory (const std::string& name) try
{
  std::vector< Reference::To<Archive::Extension> > instances;

  instances.push_back( new ITRFExtension );
  instances.push_back( new AuxColdPlasma );

  if (verbose)
    cerr << "Pulsar::Integration::Extension::factory instances=" << instances.size() 
         << " name='" << name << "'" << endl;

  return TextInterface::factory<Archive::Extension> (instances, name);
}
catch (Error& error)
{
  if (verbose)
    cerr << "Pulsar::Archive::Extension::factory error=" << error << endl;
  throw error += "Pulsar::Archive::Extension::factory";
}

// //////////////////////////////////////////////////////////////////////////

#include "Pulsar/IntegrationExtension.h"
#include "Pulsar/AuxColdPlasmaMeasures.h"
#include "Pulsar/Pointing.h"

Pulsar::Integration::Extension* 
Pulsar::Integration::Extension::factory (const std::string& name) try
{
  std::vector< Reference::To<Integration::Extension> > instances;

  instances.push_back( new AuxColdPlasmaMeasures );
  instances.push_back( new Pointing );

  if (verbose)
    cerr << "Pulsar::Integration::Extension::factory instances=" << instances.size() 
         << " name='" << name << "'" << endl;

  return TextInterface::factory<Integration::Extension> (instances, name);
}
catch (Error& error)
{
  if (verbose)
    cerr << "Pulsar::Integration::Extension::factory error=" << error << endl;
  throw error += "Pulsar::Integration::Extension::factory";
}