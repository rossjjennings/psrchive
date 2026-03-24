/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ModelParametersReport.h"

using namespace std;

void Calibration::ModelParametersReport::report (ostream& os)
{
  const unsigned nparam = model->get_nparam();
  unsigned nparam_infit = 0;
  for (unsigned iparm=0; iparm < model->get_nparam(); iparm++)
    if (model->get_infit(iparm))
      nparam_infit ++;

  os << nparam << " model parameters [index:free name=value]" << endl;
  os << nparam_infit << " unique free parameters" << endl;
  os << model->get_num_input() << " input states" << endl;
  os << model->get_num_transformation() << " signal paths" << endl;

  const unsigned nxform = model->get_num_transformation();
  for (unsigned ixform=0; ixform < nxform; ixform ++)
  {
    model->set_transformation_index (ixform);
    auto xform = model->get_transformation()->get_transformation();

    const unsigned nparam = xform->get_nparam();

    os << "\n---- signal path " << ixform << " = " << (void*) xform
       << " with " << nparam << " parameters ----" << endl;

    for (unsigned iparm=0; iparm < nparam; iparm++)
    {
      os << iparm << ":" << xform->get_infit(iparm) << " "
         << xform->get_param_name(iparm)
         << " = " << xform->get_param(iparm) << endl;
    }
  }

  const unsigned ninput = model->get_num_input();
  for (unsigned iinput=0; iinput < ninput; iinput ++)
  {
    model->set_input_index (iinput);
    auto input = model->get_input ();

    const unsigned nparam = input->get_nparam();

    os << "\n---- input/source " << iinput << " = " << (void*) input
       << " with " << nparam << " parameters ----" << endl;

    for (unsigned iparm=0; iparm < nparam; iparm++)
    {
      os << iparm << ":" << input->get_infit(iparm) << " "
         << input->get_param_name(iparm)
         << " = " << input->get_param(iparm) << endl;
    }
  }
}

