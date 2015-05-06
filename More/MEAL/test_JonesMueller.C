/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "MEAL/JonesMueller.h"
#include "MEAL/Coherency.h"

#include "MEAL/Composite.h"
#include "MEAL/Projection.h"
#include "MEAL/ProductRule.h"
#include "MEAL/VectorRule.h"
#include "MEAL/Depolarizer.h"
#include "MEAL/Rotation.h"
#include "MEAL/Boost.h"

using namespace std;
using namespace MEAL;

int main () try
{
  Function::verbose = true;
  Function::very_verbose = true;

  cerr << "main: new ProductRule<Real4>" << endl;
  Reference::To< ProductRule<Real4> > combination;
  combination = new ProductRule<Real4>;

  cerr << "main: new Depolarizer" << endl;
  Depolarizer* impurity = new Depolarizer;

  cerr << "main: ProductRule<Real4>::add_model Depolarizer" << endl;
  combination->add_model( impurity );

  cerr << "main: new ProductRule<Complex2>" << endl;
  ProductRule<Complex2>* xform;
  xform = new ProductRule<Complex2>;

  cerr << "main: new VectorRule<Complex2>" << endl;
  VectorRule<Complex2> boosts;

  cerr << "main: new Rotation" << endl;
  Rotation rotation;

  cerr << "main: ProductRule<Complex2>::add_model VectorRule<Complex2>" << endl;
  xform->add_model( &boosts );
  cerr << "main: ProductRule<Complex2>::add_model Rotation" << endl;
  xform->add_model( &rotation );
  
  cerr << "main: ProductRule<Real4>::add_model new JonesMueller" << endl;
  combination->add_model( new JonesMueller (xform) );

  cerr << "main: VectorRule<Complex2> new Boost" << endl;
  boosts.push_back( new Boost );

  cerr << "main: ProductRule<Real4>::evaluate" << endl;
  vector<Real4::Result> grad;
  combination->evaluate(&grad);

  cerr << "grad.size=" << grad.size() << endl;
  cerr << "comb.nparam=" << combination->get_nparam() << endl;

  return 0;
}

 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }
