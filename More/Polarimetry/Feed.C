/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/Feed.h"

#include "MEAL/ProductRule.h"
#include "MEAL/Complex2Constant.h"
#include "MEAL/Rotation1.h"
#include "MEAL/CyclicParameter.h"

#include "Pauli.h"
#include "ModifyRestore.h"

using namespace std;

#define FEED_BOTH 0

void Calibration::Feed::init ()
{
  // name = "Feed";

  // The transformation of each receptor
  MEAL::ProductRule<MEAL::Complex2>* receptor;

  // The selection matrix of each receptor
  MEAL::Complex2Constant* selection;

  for (unsigned ir=0; ir<2; ir++) {

    string rname (1, char('0' + ir));

    receptor = new MEAL::ProductRule<MEAL::Complex2>;
    // receptor->name = "Feed::receptor[" + rname + "]";

    Jones<double> select_jones;
    select_jones(ir, ir) = 1.0;

    // construct the 2x2 selection matrix
    selection = new MEAL::Complex2Constant (select_jones);
    receptor->add_model (selection);

#if FEED_BOTH
    // construct the elipticity matrix
    ellipticity[ir] = new MEAL::Rotation1 (Pauli::basis.get_basis_vector(1));
    // construct the orientation matrix
    orientation[ir] = new MEAL::Rotation1 (Pauli::basis.get_basis_vector(2));
#else
    // construct the elipticity matrix
    ellipticity[ir] = new MEAL::Rotation1 (Vector<3, double>::basis(1));
    // construct the orientation matrix
    orientation[ir] = new MEAL::Rotation1 (Vector<3, double>::basis(2));
#endif

    receptor->add_model (ellipticity[ir]);
    receptor->add_model (orientation[ir]);

    // add the receptor
    add_model (receptor);

  }

  if (get_nparam() != 4)
    throw Error (InvalidState, "Calibration::Feed::Feed",
		 "number of parameters != 4");

}

Calibration::Feed::Feed ()
{
  init ();

#if FEED_BOTH
  for (unsigned i=0; i<2; i++)  {
    set_orientation (i, Pauli::basis.get_orientation());
    set_ellipticity (i, Pauli::basis.get_ellipticity());
  }
#endif

}

Calibration::Feed::Feed (const Feed& feed)
{
  init ();
  operator = (feed);
}

Calibration::Feed& Calibration::Feed::operator = (const Feed& feed)
{
  if (this != &feed)
    for (unsigned ir=0; ir<2; ir++) {
      *(ellipticity[ir]) = *(feed.ellipticity[ir]);
      *(orientation[ir]) = *(feed.orientation[ir]);
    }
  return *this;
}

Calibration::Feed::~Feed ()
{
}

void Calibration::Feed::set_cyclic (bool flag)
{
  // disable automatic installation so that copy can be made
  ModifyRestore<bool> (MEAL::ParameterPolicy::auto_install, false);

  if (flag) {


    for (unsigned ir=0; ir<2; ir++) {

      // set up the cyclic boundary for orientation
      MEAL::CyclicParameter* o_cyclic = 0;
      o_cyclic = new MEAL::CyclicParameter (orientation[ir]);

      o_cyclic->set_period (M_PI);
      o_cyclic->set_upper_bound (M_PI/2);
      o_cyclic->set_lower_bound (-M_PI/2);

      orientation[ir]->set_parameter_policy (o_cyclic);

      // set up the cyclic boundary for ellipticity
      MEAL::CyclicParameter* e_cyclic = 0;
      e_cyclic = new MEAL::CyclicParameter (ellipticity[ir]);

      e_cyclic->set_period (M_PI);
      e_cyclic->set_upper_bound (M_PI/4);
      e_cyclic->set_lower_bound (-M_PI/4);
      e_cyclic->set_azimuth (o_cyclic);

      ellipticity[ir]->set_parameter_policy (e_cyclic);


    }

  }
  else {

    MEAL::OneParameter* noncyclic = 0;

    for (unsigned ir=0; ir<2; ir++) {

      noncyclic = new MEAL::OneParameter (ellipticity[ir]);
      ellipticity[ir]->set_parameter_policy (noncyclic);

      noncyclic = new MEAL::CyclicParameter (orientation[ir]);
      orientation[ir]->set_parameter_policy (noncyclic);

    }

  }

}

//! Return the name of the class
string Calibration::Feed::get_name () const
{
  return "Feed";
}

void Calibration::Feed::set_ellipticity (unsigned ireceptor,
					 const Estimate<double>& chi)
{
  if (verbose)
    cerr << "Calibration::Feed::set_ellipticity " << chi << endl;

  ellipticity[ireceptor]->set_Estimate (0, chi);
}

Estimate<double> Calibration::Feed::get_ellipticity (unsigned ireceptor) const
{
  return ellipticity[ireceptor]->get_Estimate (0);
}

void Calibration::Feed::set_orientation (unsigned ireceptor,
					 const Estimate<double>& theta)
{
  if (verbose)
    cerr << "Calibration::Feed::set_orientation " << theta << endl;

  orientation[ireceptor]->set_Estimate (0, theta);
}

Estimate<double> Calibration::Feed::get_orientation (unsigned ireceptor) const
{
  return orientation[ireceptor]->get_Estimate (0);
}

