#include "Pulsar/PolnProfileFitAnalysis.h"

#include "Calibration/ReceptionModel.h"
#include "Calibration/CoherencyMeasurementSet.h"

#include "Pauli.h"

using namespace std;

//! Set the PolnProfileFit algorithm to be analysed
void Pulsar::PolnProfileFitAnalysis::set_fit (PolnProfileFit* f)
{
  fit = f;

  if (!fit)
    return;

  // calculate the curvature matrix
  Matrix<8,8,double> curvature;

  unsigned ih = 0;

  for (ih=0; ih < fit->model->get_num_input(); ih++) {

    double phase_shift = -2.0 * M_PI * double(ih+1);

    fit->phase_axis.set_value (phase_shift);
    fit->model->set_input_index (ih);

    model_result = fit->model->evaluate (&model_gradient);

#ifdef _DEBUG
    for (unsigned ig=0; ig < model_gradient.size(); ig++)
      cerr << "g[" << fit->model->get_param_name(ig) << "]=\t"
	   << model_gradient[ig] << endl;
#endif

    for (unsigned ir=0; ir < 8; ir++) {

      Jones<double> delrho_deleta_r = model_gradient[ir+2];    

      for (unsigned is=0; is < 8; is ++) {

	Jones<double> delrho_deleta_s = model_gradient[is+2];

	curvature[ir][is]+=trace(delrho_deleta_r*herm(delrho_deleta_s)).real();
      }

    }

  }

  double mean_variance = 0.0;

  cerr << "standard variance=" << fit->standard_variance << endl;

  for (unsigned i=0; i<4; i++)
    mean_variance += fit->standard_variance[i] / 4.0;

  cerr << "mean variance=" << mean_variance << endl;

  // I think that this should be 2/mean_variance, but this matches the
  // covariance matrix produced by PolnProfileFit ...  :-(
  curvature *= 1.0 / mean_variance;

  cerr << "curvature=\n" << curvature << endl;

  cerr << "covariance=\n" << inv(curvature) << endl;


  for (ih=0; ih < fit->model->get_num_input(); ih++) {

    double phase_shift = -2.0 * M_PI * double(ih+1);

    fit->phase_axis.set_value (phase_shift);
    fit->model->set_input_index (ih);

    model_result = fit->model->evaluate (&model_gradient);
    xform_result = fit->transformation->evaluate (&xform_gradient);
    phase_result = fit->phase_xform->evaluate (&phase_gradient);

    for (unsigned ip=0; ip < 4; ip++) {

      Matrix<8,8,double> delalpha_delSre;
      Matrix<8,8,double> delalpha_delSim;

      for (unsigned ir=0; ir < 8; ir++) {
	
	Jones<double> del2rho_deleta_r = delgradient_delS (ir, ip);
	Jones<double> delrho_deleta_r = model_gradient[ir+2];    
	
	for (unsigned is=0; is < 8; is ++) {
	  
	  Jones<double> del2rho_deleta_s = delgradient_delS (is, ip);
	  Jones<double> delrho_deleta_s = model_gradient[is+2];
	  
	  delalpha_delSre[ir][is] = 
	    trace( del2rho_deleta_r * herm(delrho_deleta_s) +
		   delrho_deleta_r  * herm(del2rho_deleta_s) ).real();

	  complex<double> i (0,1);

	  delalpha_delSim[ir][is] = 
	    trace( i*del2rho_deleta_r * herm(delrho_deleta_s) +
		   delrho_deleta_r    * herm(i*del2rho_deleta_s) ).real();

	}

      }

      delalpha_delSre *= 1.0 / mean_variance;
      delalpha_delSim *= 1.0 / mean_variance;

    }

  }
#if 0

    xform_result = fit->transformation->evaluate (&xform_gradient);
    phase_result = fit->phase_xform->evaluate (&phase_gradient);
    
    if (!ih) {
      cerr << "xform gradient size=" << xform_gradient.size() << endl;
      cerr << "phase gradient size=" << phase_gradient.size() << endl;
      cerr << "model gradient size=" << model_gradient.size() << endl;
    }


    //! The transformation between the standard and observation
    Reference::To<MEAL::Complex2> transformation;

    //! The measurement equation used to model the fit
    Reference::To<Calibration::ReceptionModel> model;

    //! The phase transformation in the Fourier domain
    Reference::To<MEAL::Complex2> phase_xform;

    //! The variance of the standard
    Stokes<float> standard_variance;
#endif

}

Jones<double> 
Pulsar::PolnProfileFitAnalysis::delgradient_delS (unsigned i, unsigned k) const
{
  Quaternion<double,Hermitian> q;
  q[k] = 1.0;
  Jones<double> sigma_k = convert (q);

  return 0.5 *
    ( xform_gradient[i]  * sigma_k * herm(xform_result) * phase_result +
      xform_result * sigma_k * herm(xform_gradient[i])  * phase_result +
      xform_result * sigma_k * herm(xform_result) * phase_gradient[i] );
}

Estimate<double>
Pulsar::PolnProfileFitAnalysis::get_normalized_error () const
{
  return normalized_error;
}


Estimate<double>
Pulsar::PolnProfileFitAnalysis::get_multiple_correlation () const
{
  return multiple_correlation;
}
