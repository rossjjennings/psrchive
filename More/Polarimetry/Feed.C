#include "Calibration/Feed.h"

#include "MEAL/ProductRule.h"
#include "MEAL/Complex2Constant.h"
#include "MEAL/Rotation.h"
#include "Pauli.h"

using namespace std;

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

    // construct the elipticity matrix
    ellipticity[ir] = new MEAL::Rotation (Pauli::basis.get_basis_vector(1));
    receptor->add_model (ellipticity[ir]);

    // construct the orientation matrix
    orientation[ir] = new MEAL::Rotation (Pauli::basis.get_basis_vector(2));
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

  for (unsigned i=0; i<2; i++)  {
    set_orientation (i, Pauli::basis.get_orientation());
    set_ellipticity (i, Pauli::basis.get_ellipticity());
  }
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

