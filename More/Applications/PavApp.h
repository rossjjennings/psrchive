/*
 * Copyright (c) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 */




#ifndef PAV_APP_H_
#define PAV_APP_H_



#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Interpreter.h"

#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <Pulsar/Integration.h>
#include <Pulsar/IntegrationOrder.h>
#include <Pulsar/PeriastronOrder.h>
#include <Pulsar/BinaryPhaseOrder.h>
#include <Pulsar/BinLngPeriOrder.h>
#include <Pulsar/BinLngAscOrder.h>

#include <Pulsar/StokesPlot.h>
#include <Pulsar/StokesCylindrical.h>

#include "TextInterface.h"
#include "strutil.h"
#include "dirutil.h"
#include "getopt.h"
#include "cpgplot.h"

#include "ColourMap.h"

#include <tostring.h>

#include <cpgplot.h>

#include <sstream>

#include <unistd.h>

using namespace Pulsar;
using namespace std;



class PavApp
{
public:
  PavApp();

  void PrintUsage( void );

  const void d_minmaxval ( vector<double> thearray, double &min, double &max);
  const void minmaxval( vector<float> thearray, float &min, float &max );

  void bandpass (const Archive* data);
  void line_phase_subints (const Pulsar::Archive* data);
  void spherical ( const Profile* I,
                   const Profile* Q,
                   const Profile* U,
                   const Profile* V,
                   bool calibrated );
  void spherical_wrapper (const Archive* data);
  
  void SetStokesPlotToQU( vector< Reference::To<Plot> > &plots );
  void SetPhaseZoom( double min_phase, double max_phase, vector< Reference::To<Plot> > &plots );
  void SetFreqZoom( double min_freq, double max_freq, vector< Reference::To<Plot> > &plots, Reference::To<Archive> archive );
  bool device_is_printer( string devname );

  int run( int argc, char *argv[] );
private:
  bool printing;
  int ipol;
  int fsub, lsub;
  int isubint;
  int ichan;
  double rot_phase;
  bool svp;
  bool publn;
  bool axes;
  bool labels;
  unsigned int n1, n2;
  double min_freq;
  double max_freq;
  double min_phase;
  double max_phase;
  double reference_position_angle;
  double position_angle_height;
  double border;
  bool dark;
  double y_max;
  double y_min;
  double truncate_amp;
  pgplot::ColourMap::Name colour_map;

  bool cbppo;
  bool cbpao;
  bool cblpo;
  bool cblao;

  unsigned int ronsub;
};

#endif






