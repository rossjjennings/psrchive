#include <vector>
#include "psr_cpp.h"

template <class Klass>
void scrunch (vector<Klass>& vals, int factor, bool integrate = true)
{
  vector<Klass>::iterator into = vals.begin();

  for (vector<Klass>::iterator val = vals.begin(); val != vals.end(); into++) {
    val ++;
    for (int fi=1; fi<factor; (val++, fi++))
      *into += *val;
  }
  vals.resize (vals.size()/factor);
}
