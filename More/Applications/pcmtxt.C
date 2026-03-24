/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/ConfigurableProjectionExtension.h"
#include "Pulsar/ConfigurableProjection.h"

#include "pairutil.h"

using namespace std;
using namespace Pulsar;

//
//! Prints data extracted/derived from pcm solutions as text
//
class pcmtxt : public Pulsar::Application
{
public:

  //! Default constructor
  pcmtxt ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Print configurable projection model evaluated on a two-dimensional grid
  void print (ConfigurableProjectionExtension* ext);

protected:

  //! Output a two-dimensional grid of the named configurable projection model parameter
  string configurable_projection_parameter_name;

  //! the abscissa index on each axis
  std::pair<unsigned,unsigned> configurable_projection_abscissae;

  //! the frequency channel for which to extract a grid
  unsigned grid_ichan;

  //! the number of grid points on each axis
  std::pair<unsigned,unsigned> grid_points; 

  //! the minimum and maximum value on each axis
  std::pair<double, double> grid_range[2]; 

  //! Add command line options
  void add_options (CommandLine::Menu&);
};


pcmtxt::pcmtxt ()
  : Application ("pcmtxt", "print data extracted/derived from pcm.fits files")
{
  configurable_projection_abscissae.first = 0;
  configurable_projection_abscissae.second = 1;

  grid_ichan = 0;

  // a 100x100 grid
  grid_points.first = 100;
  grid_points.second = 100;

  // range on x-axis (e.g. hour angle in radians)
  grid_range[0].first = -1;
  grid_range[0].second = +1;

  // range on y-axis (e.g. declination in radians)
  grid_range[1].first = -1;
  grid_range[1].second = +1;
}


/*!

  Add application-specific command-line options.

*/

void pcmtxt::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "Configurable projection grid options:");

  // set the indeces on each axis
  arg = menu.add (configurable_projection_parameter_name, "cp", "name");
  arg->set_help ("name of configurable projection model parameter to be gridded");

  // set the indeces on each axis
  arg = menu.add (configurable_projection_abscissae, 'a', "ix:iy");
  arg->set_help ("configurable projection abscissa index for each grid axis");

  // add a blank line and a header to the output of -h
  menu.add ("\n" "Generic grid options:");

  // set the frequency axis
  arg = menu.add (grid_ichan, 'c', "ichan");
  arg->set_help ("frequency channel index of model data");

  // set the indeces on each axis
  arg = menu.add (grid_points, 'N', "Nx:Ny");
  arg->set_help ("number of points on each grid axis");

  // set the range on x-axis
  arg = menu.add (grid_range[0], 'x', "xmin:xmax");
  arg->set_help ("range on grid x-axis");

  // set the range on y-axis
  arg = menu.add (grid_range[1], 'y', "ymin:ymax");
  arg->set_help ("range on grid y-axis");
}


/*!

  Scale every profile and optionally set the source name

*/

void pcmtxt::process (Pulsar::Archive* archive)
{
  if (configurable_projection_parameter_name != "")
  {
    auto ext = archive->get<ConfigurableProjectionExtension>();
    if (!ext)
      throw Error (InvalidParam, "pcmtxt::process", "archive does not contain a ConfigurableProjectionExtension");

    print (ext);
  }
}

void pcmtxt::print (ConfigurableProjectionExtension* ext)
{
  auto projection = new ConfigurableProjection (ext);

  auto xform = projection->get_transformation (grid_ichan)->get_transformation();
  auto model = xform->get_model();

  unsigned nparam = model->get_nparam();
  int model_index = -1;
  for (unsigned iparam=0; iparam < nparam; iparam++)
    if (model->get_param_name(iparam) == configurable_projection_parameter_name)
      model_index = iparam;

  if (model_index == -1)
  {
    string info = "\n\t" "constrained model parameter names include:";
    for (unsigned iparam=0; iparam < nparam; iparam++)
      info += " " + model->get_param_name(iparam);

    throw Error (InvalidParam, "pcmtxt::print",
                "parameter name='%s' not found %s",
                configurable_projection_parameter_name.c_str(), info.c_str());
  }

  if (!xform->has_constraint(model_index))
  {
    string info = "\n\t" "constrained indeces include:";
    unsigned nconstraint = xform->get_nconstraint();
    for (unsigned jconstraint=0; jconstraint < nconstraint; jconstraint++)
    {
      unsigned index = xform->get_index(jconstraint);
      info += "\n\t" + tostring(index) + " " + model->get_param_name(index);
    }

    throw Error (InvalidParam, "pcmtxt::print", 
                "model index=%u is not constrained %s", model_index, info.c_str());
  }

  auto constraint = xform->get_constraint (model_index);
  string model_parameter_name = model->get_param_name(model_index);
  vector<string> abscissa_names = projection->get_abscissa_names (model_index);
  const unsigned ndim = constraint->get_ndim();

  if (ndim < 2)
  {
    cerr << "pcmtxt::print cannot grid constraint with only " << ndim << " dimensions" << endl;
  }
  else
  {
    unsigned x_index = configurable_projection_abscissae.first;
    unsigned y_index = configurable_projection_abscissae.second;

    if (x_index >= ndim)
      throw Error (InvalidParam, "pcmtxt::print", "invalid x abscissa index=%u >= ndim=%u",
                  x_index, ndim);

    if (y_index >= ndim)
      throw Error (InvalidParam, "pcmtxt::print", "invalid y abscissa index=%u >= ndim=%u",
                  y_index, ndim);

    double x_min = grid_range[0].first;
    double x_max = grid_range[0].second;
    double x_step = (x_max - x_min) / (grid_points.first -1);

    double y_min = grid_range[1].first;
    double y_max = grid_range[1].second;
    double y_step = (y_max - y_min) / (grid_points.second - 1);

    cout << "# " << abscissa_names[x_index] << " " << abscissa_names[y_index] << " " << model_parameter_name << endl;

    for (unsigned ix=0; ix < grid_points.first; ix++)
    {
      double x_value = x_min + x_step * ix;
      constraint->set_abscissa_value (x_index, x_value);

      for (unsigned iy=0; iy < grid_points.second; iy++)
      {
        double y_value = y_min + y_step * iy;
        constraint->set_abscissa_value (y_index, y_value);

        double model_value = constraint->evaluate();

        cout << x_value << " " << y_value << " " << model_value << endl;
      }

      cout << endl;      
    }
  } 
}


/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  pcmtxt program;
  return program.main (argc, argv);
}

