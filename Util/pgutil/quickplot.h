#include <vector>
#include <cpgplot.h>

template<class T>
void quickplot (const vector<T>& vals)
{
  float xmin = 0;
  float xmax = vals.size();

  float ymin = *min_element(vals.begin(), vals.end());
  float ymax = *max_element(vals.begin(), vals.end());

  cpgswin (xmin, xmax, ymin, ymax);

  vector<float>::const_iterator ind;
  for (ind = vals.begin(); ind != vals.end(); ind++)
    cpgpt1 (ind, vals[ind], 5);
}
