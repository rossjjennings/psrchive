#include "Calibration/Feed.h"
#include "Pauli.h"

using namespace std;

int main (int argc, char** argv) try {

  MEAL::Function::verbose = true;

  cerr << "Instantiating Calibration::Feed (Signal::Linear) " << endl;
  Calibration::Feed linear;

  cerr << "linear=" << linear.evaluate() << endl;

  Pauli::basis.set_basis (Basis<double>::Circular);

  cerr << "Instantiating Calibration::Feed (Signal::Circular)" << endl;
  Calibration::Feed circular;

  cerr << "circular=" << circular.evaluate() << endl;

  cerr << "Calibration::Feed constructor passes test" << endl;
  return 0;

} catch (Error& error) {
  cerr << error << endl;
  return -1;
}

