//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ProjectGradient.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __ProjectProductGradient_H
#define __ProjectProductGradient_H

#include "Calibration/Projection.h"

namespace Calibration {

  template <class Model, class Grad>
  void ProjectGradient (const Project<Model>& model, unsigned& igrad,
			const vector<Grad>& input, vector<Grad>& output)
  {
    unsigned nparam = model->get_nparam();
    
    for (unsigned iparam = 0; iparam < nparam; iparam++) {

      unsigned imap = model.get_map()->get_imap (iparam);

      if (Model::verbose)
	cerr << "ProjectGradient iparam=" << iparam << " imap=" << imap
	     << " igrad=" << igrad << endl;

      output[imap] += input[igrad];
      
      igrad ++;

    }
  }

  template <class Model, class Grad>
  void ProjectGradient (const Project<Model>& model,
			const vector<Grad>& input, vector<Grad>& output)
  {
    unsigned igrad = 0;
    ProjectGradient (model, igrad, input, output);
  }

  template <class Model, class Grad>
  void ProjectGradient (const vector<Project<Model> >& model,
			const vector<Grad>& input, vector<Grad>& output)
  {
    unsigned nparam = output.size();
    unsigned nmodel = model.size();
    
    // set each element of the gradient to zero
    for (unsigned iparam=0; iparam<nparam; iparam++)
      output[iparam] = 0.0;
    
    unsigned igrad = 0;
    
    for (unsigned imodel=0; imodel<nmodel; imodel++) {

      if (Model::verbose)
	cerr << "ProjectGradient imodel=" << imodel 
	     << " igrad=" << igrad << endl;

      ProjectGradient (model[imodel], igrad, input, output);

    }

    // sanity check, ensure that all elements have been set
    if (igrad != input.size())
      throw Error (InvalidState, "Calibration::ProjectProductGradient",
		   "on completion igrad=%d != ngrad=%d",
		   igrad, input.size());
  }
  


}

#endif
