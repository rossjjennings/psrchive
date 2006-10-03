/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotLabel.h"
#include "Pulsar/ArchiveTI.h"
#include "substitute.h"

#include <cpgplot.h>

using namespace std;

string Pulsar::PlotLabel::unset = "unset";

Pulsar::PlotLabel::PlotLabel ()
{
  left = centre = right = unset;
  spacing = 1.2;
  offset = 0.5;
}

Pulsar::PlotLabel::~PlotLabel ()
{
}

void Pulsar::PlotLabel::plot (const Archive* data)
{
  plot (data, get_left(),   0.0);
  plot (data, get_centre(), 0.5);
  plot (data, get_right(),  1.0);
}

void 
Pulsar::PlotLabel::plot (const Archive* data, const string& label, float side)
{
  if (label == PlotLabel::unset)
    return;

  vector<string> labels;
  separate (label, labels, ".");

  for (unsigned i=0; i < labels.size(); i++) {
    labels[i] = substitute (labels[i], get_interface(data));
    row (labels[i], i, labels.size(), side);
  }

}

void Pulsar::PlotLabel::row (const string& label,
			     unsigned irow, unsigned nrow, float side)
{
  // get the length of a dash in normalized device coordinates
  float xl, yl;
  cpglen (5, "-", &xl, &yl);

  if (side < 0.5)
    side += xl;
  else if (side > 0.5)
    side -= xl;
  
  float start = 0;

  if (spacing > 0)
    start = offset + (nrow-irow-1) * spacing;
  else
    start = spacing - offset;

  cpgmtxt ("T", start, side, side, label.c_str());
}


//! Get the text interface to the archive class
Pulsar::ArchiveTI* Pulsar::PlotLabel::get_interface (const Archive* data)
{
  if (!archive_interface)
    archive_interface = new ArchiveTI;
  archive_interface->set_instance( const_cast<Archive*>(data) );
  return archive_interface;
}
