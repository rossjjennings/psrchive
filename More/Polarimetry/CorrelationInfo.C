/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CorrelationInfo.h"
#include "Pulsar/PolnCalibratorInfo.h"

#include <cassert>

using namespace Pulsar;
using namespace std;

CorrelationInfo::CorrelationInfo (unsigned idx, const PolnCalibrator* cal) 
{
  index = idx;
  poln_calibrator = cal;

  auto cal_info = cal->get_Info();

  if (!cal_info)
    return;

  auto info = dynamic_cast<PolnCalibrator::Info*>(cal_info);

  if (!info)
    return;

  unsigned nparam = 0;

  unsigned nclass = info->get_nclass();
  for (unsigned iclass=0; iclass < nclass; iclass++)
  {
    nparam += info->get_nparam(iclass);
  }

  names.resize(nparam);
  for (unsigned iparam=0; iparam < nparam; iparam++)
    names[iparam] = info->get_param_name(iparam);
}

string CorrelationInfo::get_title () const
{
  if (index < names.size())
    return "Cross-correlation between " + names[index] + "\\fn and other parameters";
  else
    return "Cross-correlation between parameters";
}

unsigned CorrelationInfo::get_nchan () const
{
  return poln_calibrator->get_nchan();
}

constexpr unsigned backend_nparam = 3;

//! Return the number of parameter classes
unsigned CorrelationInfo::get_nclass () const
{
  if (names.size() > backend_nparam)
    return 2;
  else
    return 1;
}

std::string CorrelationInfo::get_param_name (unsigned iparam) const
{
  return names[iparam];
} 

//! Return the name of the specified class
string CorrelationInfo::get_label (unsigned iclass) const
{
  unsigned start_param = 0;
  unsigned nparam = names.size();

  if (iclass == 0)
  {
    nparam = std::min(nparam, backend_nparam);
  }
  else
  {
    start_param = backend_nparam;
  }

  if (start_param == index)
    start_param ++;

  string retval = names[start_param];
  for (unsigned i=start_param+1; i<nparam; i++)
  {
    if (i != index)
      retval += " " + names[i];
  }

  return retval;
}
  
//! Return the number of parameters in the specified class
unsigned CorrelationInfo::get_nparam (unsigned iclass) const
{
  unsigned nparam = 0;

  if (iclass == 0)
  {
    nparam = backend_nparam;
    if (index < backend_nparam)
      nparam --;
  }
  else
  {
    nparam = names.size() - backend_nparam;
    if (index >= backend_nparam)
      nparam --;
  }

#if _DEBUG
  cerr << "CorrelationInfo::get_nparam iclass=" << iclass << " nparam=" << nparam << endl;
#endif

  return nparam;
}

template<typename T>
T upper_triangular (const std::vector<T>& container, unsigned N, unsigned row, unsigned col)
{
  if (col < row)
  {
    std::swap(col,row);
  }

  unsigned idx = row * N + col - (row*(row+1))/2;

  return container[idx];
}

Estimate<float> CorrelationInfo::get_param (unsigned ichan, unsigned iclass, unsigned iparam) const
{
  if (!poln_calibrator->get_transformation_valid(ichan))
    return 0.0;

  if (!poln_calibrator->has_covariance())
    return 0.0;

  std::vector<double> covar;
  poln_calibrator->get_covariance (ichan, covar);

  unsigned nparam = names.size();
  unsigned expected_size = nparam * (nparam + 1) / 2;

  if (covar.size() != expected_size)
    return 0.0;

  unsigned base_iparam = 0;

  if (iclass == 1)
  {
    iparam += backend_nparam;
    base_iparam = backend_nparam;
  }

  if (index >= base_iparam && iparam >= index)
    iparam ++;

  assert(iparam < nparam);

  double varA = upper_triangular(covar, nparam, index, index);
  double varB = upper_triangular(covar, nparam, iparam, iparam);
  double covAB = upper_triangular(covar, nparam, index, iparam);
  
  if (varA == 0 || varB == 0)
    return 0.0;

  Estimate<float> result( covAB/sqrt(varA*varB) , 0.0001 );
  return result;
}
