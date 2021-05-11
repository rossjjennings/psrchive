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

  cout << time.printdays(10) << endl;

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
  
  unsigned nchan = 256;

  // advance by one hour
  time += 3600.0;

  double MHz = 1e6;
  double df = bw / nchan;
  double dt = 1.0 / (df * MHz);

  double block_length_seconds = 10.0; // s
  unsigned ndat = block_length_seconds / dt;
  
  double min_freq = freq - 0.5* bw;

  unsigned ntot = nchan * ndat;
  cout << "computing " << ntot << " phases" << endl;

  RealTimer clock;
  clock.start();

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    double freq = min_freq + ichan * df;

    predictor->set_observing_frequency (freq);

    for (unsigned idat=0; idat < ndat; idat++)
    {
      MJD curtime = time + idat * dt;
      predictor->phase (curtime);
    }
  }

  clock.stop();

  cout << ntot << " tf-phases computed in " << clock << endl;

  predictor->set_observing_frequency (freq);

  double max_diff = 0;
  double elapsed = 0;
  ntot = 0;
  
  cout << "simulating an orbit of " << orbital_period << " seconds" << endl;

  clock.start();
  while (elapsed < orbital_period)
  {
    double spin_freq = predictor->frequency (time);
    double phase_per_sample = dt * spin_freq;

    Pulsar::Phase phi0 = predictor->phase (time);

    for (unsigned idat=0; idat < ndat; idat++)
    {
      MJD curtime = time + idat * dt;
      Pulsar::Phase phi1 = predictor->phase (curtime);
      Pulsar::Phase phi2 = phi0 + idat * phase_per_sample;
      
      double diff = (phi2 - phi1).fracturns();
      if (fabs(diff) > max_diff)
	max_diff = fabs(diff);
    }

    elapsed += ndat * dt;
    time += ndat * dt;
    ntot += ndat;
  }
  clock.stop();

  cout << ntot << " t-phases computed in " << clock << endl;

  cout << "maximum phase difference = " << max_diff << endl;
  
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

