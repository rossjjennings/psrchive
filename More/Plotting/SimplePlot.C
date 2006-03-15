#include "Pulsar/SimplePlot.h"

void Pulsar::SimplePlot::plot (const Archive* data)
{
  prepare (data);

  get_frame()->focus (data);

  get_frame()->draw_axes (data);

  get_frame()->label_axes (get_xlabel (data), get_ylabel (data));

  get_frame()->decorate (data);

  draw (data);
}

