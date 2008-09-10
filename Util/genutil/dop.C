
#include "Jones.h"
#include "BoxMuller.h"

#include <iostream>
#include <vector>

using namespace std;

int main (int argc, char** argv)
{
  double P = 0.0;
  double l0 = sqrt( 0.5 * (1+P) );
  double l1 = sqrt( 0.5 * (1-P) );

  unsigned long nsamples = 1 << 20;

  unsigned nresult = 8;
  vector<double> total (nresult, 0.0);
  vector<unsigned> count (nresult, 0);

  vector< Jones<double> > sum (nresult, 0.0);
  vector< bool > tally (nresult, false);

  BoxMuller normal;

  for (unsigned i=0; i<nsamples; i++)
  {
    // realization of random electric field
    Vector<2,complex<float> > e (complex<float> ( l0*normal(), l0*normal() ),
				 complex<float> ( l1*normal(), l1*normal() ));

    // corresponding intantaneous coherency matrix
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
