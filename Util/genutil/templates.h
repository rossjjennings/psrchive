#include <vector>
#include "psr_cpp.h"

template <class Klass>
void scrunch (vector<Klass>& vals, int factor, bool integrate = true)
{
  vector<Klass>::iterator into = vals.begin();

  for (vector<Klass>::iterator val = vals.begin(); val != vals.end(); into++) {
    *into = *val; val++;
    for (int fi=1; fi<factor && val != vals.end(); (val++, fi++))
      *into += *val;
  }
  vals.resize (vals.size()/factor);
}

// returns the mean "bin" of a histogram
template <class Klass>
double histomean (vector<Klass>& vals)
{
  double valcount = 0.0;
  double totcount = 0.0;

  double total = (double) vals.size();
  double bin = 0.0;
  for (vector<Klass>::iterator val = vals.begin(); val != vals.end(); val++) {
    valcount += *val * bin;
    totcount += *val * total;

    bin += 1.0;
  }
  return valcount/totcount;
}
