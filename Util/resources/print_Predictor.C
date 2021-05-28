#include "Pulsar/Parameters.h"
#include "Pulsar/Generator.h"
#include "Pulsar/Predictor.h"
#include "load_factory.h"
#include "RealTimer.h"

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

  // in days
  double orbital_period = fromstring<double>( params->get_value("PB") );
  // in seconds
  orbital_period *= 24 * 3600;
 
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

  cerr << time.printdays(10) << endl;

  MJD endtime = time + 86400;

  generator->set_site( "PKS" );                // hard coded to Parkes for now
  generator->set_parameters( params );
  generator->set_time_span( time, endtime );

  // low frequency and large bw/freq exacerbate the Ransom effect
  double freq = 150;  // hard coded in MHz for now
  double bw = 150;    // hard coded in MHz for now
  generator->set_frequency_span( freq-bw/2, freq+bw/2 );

  cerr << "running tempo2 ... please be patient" << endl;
  Pulsar::Predictor* predictor = generator->generate ();
  cerr << "predictor generated" << endl;
  
  // advance by 45 minute
  time += 45 * 60.0;

  double dt = 1.0;

  RealTimer clock;
  clock.start();

  double elapsed = 0;
  
  cerr << "simulating an orbit of " << orbital_period << " seconds" << endl;

  clock.start();
  while (elapsed < orbital_period)
  {
    cout << time.printdays(15);
      
    for (int offset=-1; offset <= 1; offset++)
    {
      double f = freq + offset * bw/2.0;
      predictor->set_observing_frequency (f);
      Pulsar::Phase phi = predictor->phase (time);
      
      cout << " " << phi;
    }

    cout << endl;

    time += dt;
    elapsed += dt;
  }
  clock.stop();

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

