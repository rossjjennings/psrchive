/***************************************************************************
 *
 *   Copyright (C) 2008-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveInterface.h"
#include "Pulsar/StatisticsInterface.h"
#include "Pulsar/PolnStatistics.h"
#include "substitute.h"
#include "evaluate.h"

#if _DEBUG
#include <iostream>
using namespace std;
#endif

using namespace Pulsar;

TextInterface::Parser* standard_interface (Archive* archive)
{
  // print/parse in degrees
  Angle::default_type = Angle::Degrees;

  Reference::To<TextInterface::Parser> interface 
    = archive->get_interface()->clone();

  interface->set_indentation (" ");

  Statistics* stats = new Statistics(archive);
  if (archive->get_npol() == 4)
  {
    PolnStatistics* pstats = new PolnStatistics;
#if _DEBUG
    cerr << "standard_interface: Statistics=" << stats 
	 << " PolnStatistics=" << pstats << endl;
#endif
    stats->add_plugin ( pstats );
  }

  interface->insert( new Statistics::Interface( stats ) );
    
  return interface.release();
}

std::string process (TextInterface::Parser* interface, const std::string& text)
try
{
  if ( text.find('$') == std::string::npos )
    return interface->process ( text );
  else
    return interface->get_indentation() + 
      evaluate( substitute( text, interface ) );
}
catch (Error& error)
{
  throw error += "process (text=\"" + text + "\",parser="
    + interface->get_interface_name() + ")";
}
