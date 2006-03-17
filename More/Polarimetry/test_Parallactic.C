/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/Parallactic.h"
#include "sky_coord.h"

int main (int argc, char** argv) try {
  
  Calibration::Parallactic para;
  
  cerr << "Simulating a telescope at lat=long=0 \n" << endl;
  para.set_observatory_coordinates (0.0,0.0);

  //
  //
  //

  cerr << "For a source directly north of dish: \n"
       << "expect HA = LST - RA = 0 and PA = 180" << endl;

  sky_coord coordinates ("00:00+45:00");
  para.set_source_coordinates( coordinates );
  para.set_hour_angle (0.0);
  double pa = para.get_parallactic_angle() * 180/M_PI;

  cerr << "source coordinates=" << coordinates << endl;
  cerr << "hour angle=" << para.get_hour_angle() << endl;
  cerr << "parallactic angle=" << pa << " deg" << endl;

  if (para.get_parallactic_angle() != M_PI) {
    cerr << "Unexpected parallactic angle" << endl;
    return -1;
  }

  //
  //
  //

  cerr << "\nFor a source north-west of dish: \n"
       << "expect HA = LST - RA > 90 and 0 < PA < 180" << endl;

  para.set_source_coordinates( coordinates );
  para.set_hour_angle (1.0);
  pa = para.get_parallactic_angle();

  cerr << "source coordinates=" << coordinates << endl;
  cerr << "hour angle=" << para.get_hour_angle() << endl;
  cerr << "parallactic angle=" << pa * 180/M_PI << " deg" << endl;

  if (pa < M_PI/2 || pa > M_PI) {
    cerr << "Unexpected parallactic angle" << endl;
    return -1;
  }

  //
  //
  //

  cerr << "\nFor a source directly west of dish: \n"
       << "HA = LST - RA > 0 and PA=90" << endl;

  coordinates = sky_coord ("00:00+00:00");
  para.set_source_coordinates( coordinates );
  para.set_hour_angle (2.0);
  pa = para.get_parallactic_angle();

  cerr << "source coordinates=" << coordinates << endl;
  cerr << "hour angle=" << para.get_hour_angle() << endl;
  cerr << "parallactic angle=" << pa * 180/M_PI << " deg" << endl;

  //
  //
  //

  if (pa != M_PI/2) {
    cerr << "Unexpected parallactic angle" << endl;
    return -1;
  }

  cerr << "\nFor a source south west of dish: \n"
       << "HA = LST - RA > 0 and 0 < PA < 90" << endl;

  coordinates = sky_coord ("00:00-45:00");
  para.set_source_coordinates( coordinates );
  para.set_hour_angle (1.0);
  pa = para.get_parallactic_angle();

  cerr << "source coordinates=" << coordinates << endl;
  cerr << "hour angle=" << para.get_hour_angle() << endl;
  cerr << "parallactic angle=" << pa * 180/M_PI << " deg" << endl;

  if (pa < 0 || pa > M_PI/2) {
    cerr << "Unexpected parallactic angle" << endl;
    return -1;
  }

  cerr << "\nCalibration::Parallactic passes test" << endl;
  return 0;

}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
