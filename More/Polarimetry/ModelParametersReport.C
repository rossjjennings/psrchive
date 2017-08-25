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

  os << nparam << " model parameters [index:free name=value]" << endl << endl;

  unsigned nparam_infit = 0;

  for (unsigned iparm=0; iparm < model->get_nparam(); iparm++)
  {
    os << iparm << ":" << model->get_infit(iparm) << " "
       << model->get_param_name(iparm)
       << "=" << model->get_param(iparm) << endl;
    
    if (model->get_infit(iparm))
      nparam_infit ++;
  }

  os << nparam_infit << " free parameters" << endl;
  os << model->get_num_input() << " input states" << endl;
  os << model->get_num_transformation() << " signal paths" << endl;
}
