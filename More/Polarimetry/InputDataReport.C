/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InputDataReport.h"
#include "Pulsar/CoherencyMeasurementSet.h"

using namespace std;

void Calibration::InputDataReport::report (ostream& os)
{ 
  const unsigned ndat = model->get_ndata ();

  os << ndat << " measurements [index:free name=value]" << endl << endl;

  for (unsigned idat = 0; idat < ndat; idat++)
  {
    // get the specified CoherencyMeasurementSet
    const Calibration::CoherencyMeasurementSet& data = model->get_data (idat);
    
    os << idat << " " << data.get_identifier() << " xform="
       << data.get_transformation_index() << endl;
    
    unsigned mstate = data.size();
    
    for (unsigned jstate=0; jstate<mstate; jstate++)
    {
      Stokes< Estimate<float> > datum = data[jstate].get_stokes();
      os << "\t" << "source=" << data[jstate].get_input_index()
	 << " " << datum << endl;
    }
  }
}

