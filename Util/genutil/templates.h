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

// //////////////////////////////////////////////////////////////////////////
// renew
//   Much like realloc in C, will resize an array while retaining original
//   information (up to the length of the new array).
//
// Willem van Straten, August 2000
//
template <class T>
void renew (T* &ptr, unsigned neu_size, unsigned old_size)
{
  if (old_size == neu_size)
    return;

  if (neu_size == 0 && ptr != NULL) {
    delete [] ptr;  ptr = NULL;
  }

  T* nptr = new T [neu_size];
  assert (nptr != NULL);

  if (ptr) {
    unsigned cpt = min (neu_size, old_size);
    for (unsigned ipt = 0; ipt < cpt; ipt++)
      nptr[ipt] = ptr[ipt];
    delete [] ptr;
  }

  ptr = nptr;
}
