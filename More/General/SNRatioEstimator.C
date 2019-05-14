/***************************************************************************
 *
 *   Copyright (C) 2016 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SNRatioEstimator.h"
#include "interface_factory.h"

#include "Pulsar/PhaseSNR.h"
#include "Pulsar/StandardSNR.h"
#include "Pulsar/AdaptiveSNR.h"
#include "Pulsar/ModularSNR.h"
#include "Pulsar/SquareWave.h"
#include "Pulsar/FortranSNR.h"
#include "Pulsar/FourierSNR.h"

using namespace std;

Pulsar::SNRatioEstimator*
Pulsar::SNRatioEstimator::factory (const std::string& name_parse)
{
  std::vector< Reference::To<SNRatioEstimator> > instances;

  instances.push_back( new PhaseSNR );
  instances.push_back( new StandardSNR );
  instances.push_back( new AdaptiveSNR );
  instances.push_back( new ModularSNR );
  instances.push_back( new SquareWave );
  instances.push_back( new FortranSNR );
  instances.push_back( new FourierSNR );

  return TextInterface::factory<SNRatioEstimator> (instances, name_parse);
}

#include "interface_stream.h"

namespace Pulsar
{
  std::ostream& operator<< (std::ostream& ostr, SNRatioEstimator* e)
  {
    return interface_insertion (ostr, e);
  }

  std::istream& operator>> (std::istream& istr, SNRatioEstimator* &e)
  {
    return interface_extraction (istr, e);
  }
}
