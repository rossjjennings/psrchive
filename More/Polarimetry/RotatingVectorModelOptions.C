/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RotatingVectorModelOptions.h"
#include "MEAL/ScalarParameter.h"

using namespace std;

Pulsar::RotatingVectorModelOptions::RotatingVectorModelOptions ()
{
  _set_fit = false;
  RVM = 0;
}

double deg_to_rad (const std::string& arg)
{
  return fromstring<double> (arg) * M_PI/180.0;
}

void Pulsar::RotatingVectorModelOptions::add_options (CommandLine::Menu& menu)
{
  if (!RVM)
    throw Error (InvalidState, 
		 "Pulsar::RotatingVectorModelOptions::add_options",
		 "Rotating Vector Model instance not set");

  CommandLine::Argument* arg;

  menu.add ("\n" "Rotating Vector Model options:");

  arg = menu.add (RVM->magnetic_axis.get(),
		  &MEAL::ScalarParameter::set_value,
		  deg_to_rad, 'a', "degrees");
  arg->set_help ("alpha: colatitude of magnetic axis");

  if (_set_fit)
  {
    arg = menu.add (RVM->magnetic_axis.get(),
		    &MEAL::ScalarParameter::set_fit, 'A', false);
    arg->set_help ("hold alpha constant");
  }

  arg = menu.add (RVM->line_of_sight.get(),
		  &MEAL::ScalarParameter::set_value,
		  deg_to_rad, 'z', "degrees");
  arg->set_help ("zeta: colatitude of line of sight");

  if (_set_fit)
  {
    arg = menu.add (RVM->line_of_sight.get(),
		    &MEAL::ScalarParameter::set_fit, 'Z', false);
    arg->set_help ("hold zeta constant");
  }

  arg = menu.add (RVM->magnetic_meridian.get(),
		  &MEAL::ScalarParameter::set_value,
		  deg_to_rad, 'b', "degrees");
  arg->set_help ("phi0: longitude of magnetic meridian");

  if (_set_fit)
  {
    arg = menu.add (RVM->magnetic_meridian.get(),
		    &MEAL::ScalarParameter::set_fit, 'B', false);
    arg->set_help ("hold phi0 constant");
  }

  arg = menu.add (RVM->reference_position_angle.get(),
		  &MEAL::ScalarParameter::set_value,
		  deg_to_rad, 'p', "degrees");
  arg->set_help ("psi0: position angle at magnetic meridian");

  if (_set_fit)
  {
    arg = menu.add (RVM->reference_position_angle.get(),
		    &MEAL::ScalarParameter::set_fit, 'P', false);
    arg->set_help ("hold psi0 constant");
    
    menu.add ("");
    arg = menu.add (RVM, &MEAL::RotatingVectorModel::use_impact,
		    "use_beta", true);
    arg->set_help ("vary beta (not zeta) as a free model parameter");
  }
}
