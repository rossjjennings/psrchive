/***************************************************************************
 *
 *   Copyright (C) 2023 by Lucas Guillemot and Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DataAndModelReport.h"
#include "Pulsar/CoherencyMeasurementSet.h"

#include <fstream>
using namespace std;

void Calibration::DataAndModelReport::report ()
{
  if (filename.empty())
    throw Error( InvalidState, "Calibration::DataAndModelReport:report", "filename not set");

  set<string> sources = get_source_names();

  for (string src: sources)
  {
    current_source_name = src;
    string fullpath = make_reports_path (src);

    string fullname = fullpath + "/" + filename;
    ofstream os ( fullname.c_str() );
    if (!os)
      throw Error( FailedSys, "Calibration::DataAndModelReport::report", "ofstream (" + fullname + ")" );

    current_source_name = src;
    report (os);
  }
}

std::set<std::string> Calibration::DataAndModelReport::get_source_names ()
{
  std::set<std::string> result;

  unsigned ndat = model->get_ndata();
  for (unsigned idat=0; idat < ndat; idat++)
  {
    const CoherencyMeasurementSet& data = model->get_data (idat);
    string source_name = data.get_name();
    if (!source_name.empty())
      result.insert (source_name);
  }

  return result;
}

void Calibration::DataAndModelReport::report (ostream& outfile)
{
  unsigned idata, idata2, index;
  
  double valI, stdI, valQ, stdQ, valU, stdU, valV, stdV;
  double resI,resQ,resU,resV;
  
  Stokes< Estimate<double> > stokes;
  
  Jones<double> result;
  
  //
  
  unsigned the_size = model->get_ndata();
  for (idata=0;idata<the_size;idata++)
  {
    const CoherencyMeasurementSet& the_data = model->get_data (idata);

    if (the_data.get_name() != current_source_name)
      continue;

    unsigned the_size2 = the_data.size();
    if (the_size2 == 0) continue;
    
    // exclude CAL observations
    if (the_size2 == 1) continue;

    the_data.set_coordinates();
    
    model->set_transformation_index(the_data.get_transformation_index());
    
    for (idata2=0;idata2<the_size2;idata2++)
    {
      index  = the_data[idata2].get_input_index();
      model->set_input_index(index);
      
      result = model->evaluate();
      
      resI   = (result[0]+result[3]).real();
      resQ   = (result[0]-result[3]).real();
      resU   = (result[1]+result[2]).real();
      resV   = -(result[1]-result[2]).imag();
      
      //
      
      stokes = the_data[idata2].get_stokes();
      valI   = stokes[0].get_value();
      stdI   = sqrt(stokes[0].get_variance());
      valQ   = stokes[1].get_value();
      stdQ   = sqrt(stokes[1].get_variance());
      valU   = stokes[2].get_value();
      stdU   = sqrt(stokes[2].get_variance());
      valV   = stokes[3].get_value();
      stdV   = sqrt(stokes[3].get_variance());
      
      //
      
      outfile << idata;
      outfile << " " << idata2;
      outfile << " " << the_data.get_name();
      outfile << " " << resI << " " << valI << " " << stdI;
      outfile << " " << resQ << " " << valQ << " " << stdQ;
      outfile << " " << resU << " " << valU << " " << stdU;
      outfile << " " << resV << " " << valV << " " << stdV;

      outfile << " " << the_data.get_coordinates_string();
      outfile << endl;
    }
  }
} 
  