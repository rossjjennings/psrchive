#include "Composite.h"
#include "Rotation.h"
#include "Boost.h"

using namespace Calibration;

//! Tests the mapping function of the Composite class
class CompositeTest: public Composite {

public:
  void runtest ();
  void evaluate () { set_evaluation_changed (false); }
  bool get_changed () { return get_evaluation_changed(); }
  string get_name () const { return "CompositeTest"; }
};


void CompositeTest::runtest ()
{
  Rotation rotation (Vector<double, 3>::basis(0));
  Boost boost (Vector<double, 3>::basis(2));

  CompositeTest m1;
  CompositeTest m2;

  Project<Rotation> m1r (&rotation);
  Project<Rotation> m2r (&rotation);

  Project<Composite> m1m (&m2);

  Project<Boost> m2b (&boost);

  cerr << "********************* m1 map rotation" << endl;
  m1.map (m1r);

  cerr << "********************* m1 map m2" << endl;
  m1.map (m1m);

  if (!m1.get_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Composite not changed after mapping");

  m1.evaluate();

  if (m1.get_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Composite changed after evaluate");

  cerr << "********************* m2 map boost" << endl;
  m2.map (m2b);
  cerr << "********************* m2 map rotation" << endl;
  m2.map (m2r);
    
  if (!m1.get_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Parent Composite not changed after child Composite mapping");

  cerr << "m2 maps into m1:" << endl;

  for (unsigned iparm=0; iparm<m2.get_nparam(); iparm++)
    cerr << " " << iparm << " " << m1m.get_imap(iparm) << endl;
    
  if (m1m.get_imap(0) != 1 || m1m.get_imap(1) != 0)
    throw Error (InvalidState, "test_Composite", "mapping failure");

}

int main (int argc, char** argv) try
{
  Model::verbose = true;
  CompositeTest test;

  test.runtest ();

  cerr << "Successful completion" << endl;
  return 0;
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}

