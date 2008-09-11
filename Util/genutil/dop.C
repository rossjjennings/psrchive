
#include "Jones.h"
#include "BoxMuller.h"

#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

static BoxMuller normal;

static double lambda0 = 0.0;
static double lambda1 = 0.0;

typedef Vector< 2, complex<float> > Efield;

Efield white ()
{
  return Efield
    ( complex<float> ( lambda0*normal(), lambda0*normal() ),
      complex<float> ( lambda1*normal(), lambda1*normal() ) );
}

class Correlated
{
public:
  void exponential (double tau);
  Efield operator () ();
  vector< complex<float> > impulse_response;
  vector< Efield > input;
  unsigned current;
};

int main (int argc, char** argv)
{
  double P = 0.0;
  double tau = 0.0;

  if (argc > 1)
    P = atof (argv[1]);

  if (argc > 2)
    tau = 1.0 / atof (argv[2]);

  lambda0 = sqrt( 0.5 * (1+P) );
  lambda1 = sqrt( 0.5 * (1-P) );

  unsigned long nsamples = 1 << 20;

  unsigned nresult = 8;
  vector<double> total (nresult, 0.0);
  vector<unsigned> count (nresult, 0);

  vector< Jones<double> > sum (nresult, 0.0);
  vector< bool > tally (nresult, false);

  Correlated correlated;
  if (tau)
    correlated.exponential (tau);

  for (unsigned i=0; i<nsamples; i++)
  {
    // random electric field vector (Goodman 1963)
    Efield e;

    if (tau)
      e = correlated ();
    else
      e = white ();

    // intantaneous coherency matrix
    Jones<double> J( outer( e, conj(e) ) );

    // make averages of 
    for (unsigned ires=0; ires < nresult; ires++)
    {
      if (tally[ires])
      {
	sum[ires] += J;
	J = sum[ires];
	tally[ires] = false;

	total[ires] += J.p();
	count[ires] ++;
      }
      else
      {
	sum[ires] = J;
	tally[ires] = true;
	break;
      }

    }
  }

  unsigned power = 2;
  for (unsigned ires=0; ires < nresult; ires++)
  {
    cout << power << " " << count[ires]
	 << " " << total[ires]/count[ires] << endl;
    power *= 2;
  }
}


void Correlated::exponential (double tau)
{
  double limit = 1e-6;
  unsigned npt = unsigned( log(limit) / -tau );

  cerr << "Correlated::exponential tau=" << tau << " npt=" << npt << endl;

  impulse_response.resize( npt );
  input.resize( npt );

  for (unsigned ipt=0; ipt < npt; ipt++)
  {
    input[ipt] = white ();
    impulse_response [ipt] = 1/tau * exp (-tau * ipt);
  }

  current = 0;
}

Efield Correlated::operator () ()
{
  Vector< 2, complex<double> > total;
  unsigned npt = input.size();

  for (unsigned ipt=0; ipt<npt; ipt++)
    {
      // cerr << ipt << " " << endl;
      total += impulse_response [ipt] * input[ (ipt+current) % npt ];
    }

  input[current] = white();
  current ++;
  current %= npt;

  return total;
}
