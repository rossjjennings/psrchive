/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IXRInfo.h"
#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/BackendFeed.h"

using namespace Calibration;
using namespace std;

//! Constructor
Pulsar::IXRInfo::IXRInfo (const PolnCalibrator* cs)
{
  poln_calibrator = cs;
}
    
std::string Pulsar::IXRInfo::get_title () const
{
  return "Intrinsic Cross-Polarization Ratio";
}

//! Return the number of frequency channels
unsigned Pulsar::IXRInfo::get_nchan () const
{
  return poln_calibrator->get_nchan();
}

//! Return the number of parameter classes
unsigned Pulsar::IXRInfo::get_nclass () const
{
  return 1;
}

//! Return the name of the specified class
std::string Pulsar::IXRInfo::get_name (unsigned iclass) const
{
  return "IXR (dB)";
}


//! Return the number of parameters in the specified class
unsigned Pulsar::IXRInfo::get_nparam (unsigned iclass) const
{
  return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::IXRInfo::get_param (unsigned ichan, unsigned iclass,
		            unsigned iparam) const
{
  if (!poln_calibrator->get_transformation_valid(ichan))
    return 0.0;

  const MEAL::Complex2* xform = poln_calibrator->get_transformation (ichan);

  const BackendFeed* instrument = dynamic_cast<const BackendFeed*> (xform);
  if (instrument)
    xform = instrument->get_frontend();

  vector< Jones<double> > gradient;
  Jones<double> J = xform->evaluate(&gradient);
  Jones<double> H = J*herm(J) / norm(det(J));

  double cosh_2beta = 0.5 * trace (H).real();
  double beta = 0.5 * acosh( cosh_2beta );

  double cosh_beta = cosh(beta);
  double sinh_beta = sinh(beta);
  double coth_beta = cosh_beta / sinh_beta;
  double ixr = coth_beta * coth_beta;

  double var = 0;

  cerr << "nparam=" << xform->get_nparam() << endl;

  for (unsigned i=0; i<xform->get_nparam(); i++)
  {
    /* where H = hermitian, H^2 = J J^dagger / | det J |
       and cosh 2beta = Tr (H^2) / 2 */

    Jones<double> K = gradient[i]*herm(J);
    Jones<double> L = H * trace(gradient[i]*inv(J)).real();

    cerr << "************* big=" << trace( K+herm(K) ) << " small=" << trace(L) << endl;

    double dbeta_di = trace( (K+herm(K))/norm(det(J)) /* - L */).real() / ( 4 * sinh(2*beta) );
cerr << "dbeta_d" << i << "=" << dbeta_di << endl;
    double dIXR_di = -2 * dbeta_di / ( sinh_beta * cosh_beta );
cerr << "dIXR_d" << i << "=" << dIXR_di << endl;
cerr << "var_" << i << "=" << xform->get_variance(i) << endl;


    var += dIXR_di*dIXR_di * xform->get_variance(i);
  }

  cerr << "--------------- IXR=" << ixr << " +/- " << sqrt(var) << endl;

  return 10 * log( Estimate<float> (ixr, var) ) / log(10);
}

