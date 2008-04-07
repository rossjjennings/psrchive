/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModelPlotter.h"
#include "Pulsar/CoherencyMeasurementSet.h"
#include "Pulsar/ReceptionModel.h"

#include "EstimatePlotter.h"
#include "Pauli.h"

#include <cpgplot.h>
#include <stdio.h>

using namespace std;

Calibration::ReceptionModelPlotter::ReceptionModelPlotter ()
{
  ipol = isource = ipath = 0;
  model_solved = false;
  plot_residual = false;
}

Calibration::ReceptionModelPlotter::~ReceptionModelPlotter ()
{
}

void Calibration::ReceptionModelPlotter::set_model (ReceptionModel* _model)
{
  model = _model;
}


void Calibration::ReceptionModelPlotter::set_abscissa (Abscissa* abs)
{
  abscissa = abs;
}


void Calibration::ReceptionModelPlotter::set_ipol (unsigned _ipol)
{
  ipol = _ipol;
}


void Calibration::ReceptionModelPlotter::set_isource (unsigned _isource)
{
  isource = _isource;
}


void Calibration::ReceptionModelPlotter::set_ipath (unsigned _ipath)
{
  ipath = _ipath;
}


void Calibration::ReceptionModelPlotter::set_model_solved (bool solved)
{
  model_solved = solved;
}


void Calibration::ReceptionModelPlotter::set_plot_residual (bool residual)
{
  plot_residual = residual;
}

void Calibration::ReceptionModelPlotter::set_output (const char* filename)
{
  output_filename = filename;
}

void Calibration::ReceptionModelPlotter::plot_observations ()
{
  std::vector< Estimate<float> > stokes[4];

  if (model_solved && plot_residual)
  {
    model->set_transformation_index (ipath);
    model->set_input_index (isource);
  }
  
  unsigned ndat = model->get_ndata ();
  for (unsigned idat=0; idat < ndat; idat++) try
  {
    // get the specified CoherencyMeasurementSet
    const Calibration::CoherencyMeasurementSet& data = model->get_data (idat);
    
    if (data.get_transformation_index() != ipath)
      continue;
    
    // set the independent variables for this observation
    data.set_coordinates();
    
    unsigned mstate = data.size();
    
    for (unsigned jstate=0; jstate<mstate; jstate++)
    {
      if (data[jstate].get_input_index() != isource)
	continue;
      
      Stokes< Estimate<float> > datum = data[jstate].get_stokes();
      
      if (model_solved && plot_residual) {
	Stokes<double> ms = coherency( model->evaluate() );
	datum -= ms;
      }
      
      for (unsigned ipol=0; ipol<4; ipol++)
	stokes[ipol].push_back (datum[ipol]);
	
      abscissa->push_back (); 
    }
  }
  catch (Error& error)
  {
    cerr << "Calibration::ReceptionModelPlotter::plot_observations idat="
         << idat << error << endl;
  }

  if (stokes[0].size() == 0)
  {
    cerr << "Calibration::ReceptionModelPlotter::plot_observations "
            "ipath=" << ipath << " isource=" << isource << " no data" << endl;
    return;
  }

  vector<double> values;
  abscissa->get_values (values);
  
  if ( !output_filename.empty() )
  {
    FILE* fptr = fopen (output_filename.c_str(), "w");
    if (!fptr)
      throw Error (FailedSys,
		   "Calibration::ReceptionModelPlotter::plot_observations",
		   "fopen (%s)", output_filename.c_str());

    for (unsigned ipt=0; ipt < values.size(); ipt++) {
      fprintf (fptr, "%f ", values[ipt]);

      for (unsigned ipol=0; ipol<4; ipol++)
	fprintf (fptr, "%f %f ", stokes[ipol][ipt].val, stokes[ipol][ipt].var);

      fprintf (fptr, "\n");

    }

    fclose (fptr);
  }

  // the plotting class
  EstimatePlotter plotter;

  float x1, x2, y1, y2;

  // query the current size of the viewport in normalized device coordinates
  cpgqvp (0, &x1, &x2, &y1, &y2);
  cpgslw (2);
  
  float Ispace = 0.18;
  float space = 0.02;
  
  float xborder  = 0.03;
  float Iyborder = 0.2;
  float Syborder = 0.08;

  cpgsvp (x1, x2, y1, y1+(y2-y1)*Ispace);
  
  plotter.add_plot (values, stokes[0]);
  plotter.set_border (xborder, Iyborder);

  set_ipol (0);
  plot_stokes (plotter, 0);

  if (model_solved && !plot_residual)
    plot_model ();

  plotter.clear ();
  plotter.set_border (xborder, Syborder);
  
  cpgsvp (x1, x2, y1+(y2-y1)*(Ispace+space), y2);
  
  for (unsigned ipol=1; ipol<4; ipol++)
    plotter.add_plot (values, stokes[ipol]);
  
  plotter.separate_viewports();
  
  for (unsigned iplot=0; iplot<3; iplot++) {

    set_ipol (iplot + 1);
    plot_stokes (plotter, iplot);

    if (model_solved && !plot_residual)
      plot_model ();

  }

  cpgsci (1);
  cpgsls (1);
  cpgsvp (x1, x2, y1, y2);
  cpgbox ("bcnst",0,0,"",0,0);
  cpgmtxt("B",3.0,.5,.5, abscissa->get_label().c_str());

  cpgsci (1);
  cpgsls (1);

}

void Calibration::ReceptionModelPlotter::plot_model ()
{
}

void Calibration::ReceptionModelPlotter::plot_stokes (EstimatePlotter& plot,
						      unsigned iplot)
{
  char stokes_label[64] = "\\fiS'\\b\\d\\fnk";
  int  position = strlen (stokes_label) - 1;
  char* stokes_index = "0123";

  plot.plot (iplot);

  cpgbox ("bcst",0,0,"bcvnst",0,0);

  stokes_label[position] = stokes_index[ipol];
  cpgmtxt("L",3.5,.5,.5,stokes_label);

  // possible circumflex, \\(0756,0832,2247)
  cpgsch(1.05);
  cpgmtxt("L",3.5,.5,.5,"\\u\\(2247)\\d \\(2197)");
  cpgsch(1.0);
}

