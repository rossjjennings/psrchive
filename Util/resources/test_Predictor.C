#include "Pulsar/Parameters.h"
#include "Pulsar/Generator.h"
#include "Pulsar/Predictor.h"
#include "load_factory.h"

using namespace std;

int main (int argc, char** argv) try
{
  // Pulsar::Predictor::verbose = true;

  if (argc < 2)
  {
    cerr << "Please specify the name of the file containing the pulsar parameters" << endl;
    return -1;
  }

  Pulsar::Parameters* params =  factory<Pulsar::Parameters> (argv[1]);

  Pulsar::Generator* generator = Pulsar::Generator::factory (params);

  /*
   * Tempo2 predictor code:
   *
   * Here we make a predictor valid for the next 24 hours
   * I consider this to be a bit of a hack, since theoreticaly
   * observations could be longer, and it's a bit silly to make
   * such a polyco for a 10 min obs.
   *
   */

  time_t temp = time(NULL);
  struct tm date = *gmtime(&temp);
  cerr << "Using today's date: " << asctime(&date) << endl;
  MJD time (date);

  cout << time.printdays(10) << endl;

  MJD endtime = time + 86400;

  generator->set_site( "PKS" );                // hard coded to Parkes for now
  generator->set_parameters( params );
  generator->set_time_span( time, endtime );

  double freq = 1400;  // hard coded in MHz for now
  double bw = 400;     // hard coded in MHz for now
  generator->set_frequency_span( freq-bw/2, freq+bw/2 );

  cerr << "running tempo2 ... please be patient" << endl;
  Pulsar::Predictor* predictor = generator->generate ();

  unsigned nbin = 1024;

  time += 3600.0;
  for (double dt=0; dt <= 0.007; dt += 0.00057)
  {
     MJD curtime = time + dt;
     Pulsar::Phase ph = predictor->phase (curtime);

     unsigned ibin = nbin * ph.fracturns();

     cerr << curtime.printdays(10) << " " << ph << " " << ibin << endl;
  }

  delete predictor;

  return 0;
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
catch (...) {
  cerr << "Exception caught" << endl;
  return -1;
}

