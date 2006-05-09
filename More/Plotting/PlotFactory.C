/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotFactory.h"

#include "Pulsar/ProfilePlot.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/PosAngPlot.h"
#include "Pulsar/EllAngPlot.h"
#include "Pulsar/StokesCylindrical.h"
#include "Pulsar/StokesSpherical.h"
#include "Pulsar/SpectrumPlot.h"
#include "Pulsar/PhaseVsFrequencyPlus.h"
#include "Pulsar/StokesFluctPlot.h"
#include "Pulsar/TwoBitPlot.h"
#include "Pulsar/Poincare.h"
#include "Pulsar/CalibratorSpectrum.h"

#include "Error.h"
#include "pad.h"

// Add plots here
Pulsar::PlotFactory::PlotFactory ()
{
  add ( new PlotFactory::Advocate<ProfilePlot>
	('p', "flux", "Single plot of flux") );

  add ( new PlotFactory::Advocate<StokesPlot>
	('S', "stokes", "Stokes parameters") );

  add ( new PlotFactory::Advocate<StokesCylindrical>
	('c', "Scyl",   "Stokes; vector in cylindrical") );

  add ( new PlotFactory::Advocate<StokesSpherical>
	('s', "Ssph",   "Stokes; vector in spherical") );

  add ( new PlotFactory::Advocate<StokesFluctPlot>
	('f', "Sfluct", "Stokes; fluctuation power spectra") );

  add ( new PlotFactory::Advocate<PhaseVsFrequency>
	('F', "freq", "Phase vs. frequency image of flux") );

  add ( new PlotFactory::Advocate<PhaseVsFrequencyPlus>
	('+', "freq+", "freq + integrated profile and spectrum") );

  add ( new PlotFactory::Advocate<PhaseVsTime>
	('T', "time", "Phase vs. time image of flux") );

  add ( new PlotFactory::Advocate<PosAngPlot>
	('o', "pa", "Orientation (Position) angle") );

  add ( new PlotFactory::Advocate<EllAngPlot>
	('e', "ell", "Ellipticity angle") );

  add ( new PlotFactory::Advocate<Poincare>
	('P', "p3d", "Stokes vector in Poincare space") );

  add ( new PlotFactory::Advocate<SpectrumPlot>
	('b', "psd", "Total power spectrum (bandpass)") );

  add ( new PlotFactory::Advocate<TwoBitPlot>
	('2', "2bit", "Two-bit distribution") );

  add ( new PlotFactory::Advocate<CalibratorSpectrum>
	('C', "cal", "Calibrator Spectrum") );
}

//! Return a new instance of the named plotter
Pulsar::Plot* Pulsar::PlotFactory::construct (std::string name)
{
  char shortcut = 0;
  if (name.length() == 1)
    shortcut = name[0];

  for (unsigned i=0; i < agents.size(); i++)
    if ( (shortcut && shortcut == agents[i]->get_shortcut()) ||
	 (name == agents[i]->get_name()) )
      return agents[i]->construct();

  throw Error (InvalidParam, "Pulsar::PlotFactory::construct",
	       "no Plot named " + name);
}
    
//! Return the name of the ith agent
std::string Pulsar::PlotFactory::get_name (unsigned i)
{
  return agents[i]->get_name();
}

//! Return the name of the ith agent
std::string Pulsar::PlotFactory::get_description (unsigned i)
{
  return agents[i]->get_description();
}

std::string Pulsar::PlotFactory::help()
{
  unsigned ia, maxlen = 0;
  for (ia=0; ia < agents.size(); ia++)
    if (get_name(ia).length() > maxlen)
      maxlen = get_name(ia).length();

  maxlen += 2;

  std::string result;

  for (ia=0; ia < agents.size(); ia++)
    result += pad(maxlen, get_name(ia)) + 
      "[" + agents[ia]->get_shortcut() + "]  " + get_description(ia) + "\n";
  
  return result;
}
