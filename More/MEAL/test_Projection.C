#include "Projection.h"
#include "Boost.h"

using namespace Calibration;

int main (int argc, char** argv)
{ try {

  Error::verbose = true;
  Calibration::Model::verbose = true;

  cerr << "Instantiating Project<Boost>" << endl;
  Project<Boost> function;

  cerr << "Instantiating Boost" << endl;
  Boost boost;

  cerr << "Calling Project<Boost>::get_map" << endl;
  Projection* mapped = function.get_map ();
  function = &boost;

  if (mapped->get_Model() != &boost) {
    cerr << "Projecton::get_Model()=" << mapped->get_Model()
	 << " != &Boost=" << &boost << endl;
    return -1;
  }

} catch (Error& error) {
  cerr << error << endl;
  return -1;
}

  cerr << "Projection::get_Model passes test" << endl;
  return 0;
}
