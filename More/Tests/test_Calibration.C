#include "Calibration.h"

int main () 
{
  Pulsar::Calibrator::verbose = true;
  Pulsar::Archive::verbose = true;

  try {

    Pulsar::Calibration::Database dbase;
 
    Pulsar::Archive* arch = Pulsar::Archive::load ("fake");

    Pulsar::PolnCalibrator* pcal_engine = dbase.generatePolnCalibrator (arch);

  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }

}
