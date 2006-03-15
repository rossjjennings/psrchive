#include "Pulsar/SimplePlot.h"

using namespace std;

void Pulsar::SimplePlot::plot (const Archive* data)
{
  prepare (data);

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot focus" << endl;
  get_frame()->focus (data);

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot draw axes" << endl;
  get_frame()->draw_axes (data);

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot label axes" << endl;
  get_frame()->label_axes (get_xlabel (data), get_ylabel (data));

  if (verbose)
    cerr << "Pulsar::SimplePlot::plot decorate frame" << endl;
  get_frame()->decorate (data);

  draw (data);
}

