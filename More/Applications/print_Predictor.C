#include "Pulsar/Parameters.h"
#include "T2Generator.h"
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

  Tempo2::Generator* t2g = dynamic_cast<Tempo2::Generator*> (generator);

  if (t2g)
  {
    t2g -> set_time_ncoeff (16);
    t2g -> set_frequency_ncoeff (8);
  }
  
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

  cerr << "generating predictor ... please be patient" << endl;
  Pulsar::Predictor* predictor = generator->generate ();
  cerr << "predictor generated" << endl;
  predictor->unload (stderr);
  
  // advance by 45 minutes
  time += 45 * 60.0;

  double dt = 10.0;
  double elapsed = 0;
  
  cerr << "simulating an orbit of " << orbital_period << " seconds" << endl;

  bool first = true;
  Pulsar::Phase phi0;
  double spin_f0;
  
  while (elapsed < orbital_period)
  {
    cout << elapsed;
      
    for (int offset=-1; offset <= 1; offset++)
    {
      double f = freq + offset * bw/2.0;
      predictor->set_observing_frequency (f);
      Pulsar::Phase phi = predictor->phase (time);
      double spin_f = predictor->frequency (time);

      if (first)
      {
	phi0 = phi;
        spin_f0 = spin_f;
	first = false;
      }

      cout << " " << phi-phi0 
           << " " << std::setprecision(12) << spin_f-spin_f0;
    }

    cout << endl;

    time += dt;
    elapsed += dt;
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

