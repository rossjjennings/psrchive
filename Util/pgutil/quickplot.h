#include <vector>
#include <algorithm>
#include <cpgplot.h>

template<class T>
void quickplot (const vector<T>& vals)
{
  float xmin = 0;
  float xmax = vals.size();

  float ymin = *min_element(vals.begin(), vals.end());
  float ymax = *max_element(vals.begin(), vals.end());

  cpgswin (xmin, xmax, ymin, ymax);

  cpgmove (0, vals[0]);

  for (unsigned ipt=1; ipt< vals.size(); ipt++)
    cpgdraw (ipt, vals[ipt]);

}
