#include "Pulsar/PlotFrame.h"
#include "Pulsar/ArchiveTI.h"

#include <cpgplot.h>

#include <vector>

using namespace std;

Pulsar::PlotFrame::PlotFrame ()
{
  x_zoom = new PlotZoom;
  x_axis = new PlotAxis;

  y_zoom = new PlotZoom;
  y_axis = new PlotAxis;

  above = new PlotLabel;
  below = new PlotLabel;

  get_label_above()->set_centre("=file");
  get_label_below()->set_left("=name.=freq MHz");

  label_spacing = 1.2;
  label_offset = 0.5;
}

Pulsar::PlotFrame::~PlotFrame ()
{
}

void Pulsar::PlotFrame::decorate (const Archive* data)
{
  decorate (data, get_label_above(), +label_spacing);
  decorate (data, get_label_below(), -label_spacing);
}

void Pulsar::PlotFrame::decorate (const Archive* data, 
				  PlotLabel* label, float direction)
{
  decorate (data, label->get_left(),   0.0, direction);
  decorate (data, label->get_centre(), 0.5, direction);
  decorate (data, label->get_right(),  1.0, direction);
}

void Pulsar::PlotFrame::decorate (const Archive* data, const string& label,
				  float side, float direction)
{
  if (label == PlotLabel::unset)
    return;

  vector<string> labels;
  TextInterface::separate (label, labels, '.');

  // get the length of a dash in normalized device coordinates
  float xl, yl;
  cpglen (5, "-", &xl, &yl);
  float offset = (side == 0) ? xl : -xl;
  
  float start = 0;
  if (direction > 0) {
    start = label_offset + (labels.size()-1) * direction;
    direction *= -1;
  }
  else
    start = direction - label_offset;

  for (unsigned i=0; i < labels.size(); i++) {
    resolve_variables (data, labels[i]);
    cpgmtxt ("T", start, side+offset, side, labels[i].c_str());
    start += direction;
  }
}

void Pulsar::PlotFrame::resolve_variables (const Archive* data, string& label)
{
  string::size_type start;

  while ( (start = label.find('=')) != string::npos ) {

    // string preceding the variable substitution
    string before = label.substr (0, start);
    // string following the variable name
    string after;

    // ignore the = sign
    start ++;

    // length to end of variable name
    string::size_type length = string::npos;

    // find the end of the variable name
    string::size_type end = label.find(' ');

    if (end != string::npos) {
      length = end - start;
      after = label.substr (end);
    }

    // the variable name
    string name = label.substr (start, length);
    string value = get_interface(data)->get_value(name);

    label = before + value + after;

  }
}

//! Get the text interface to the archive class
Pulsar::ArchiveTI* Pulsar::PlotFrame::get_interface (const Archive* data)
{
  if (!archive_interface)
    archive_interface = new ArchiveTI;
  archive_interface->set_instance( const_cast<Archive*>(data) );
  return archive_interface;
}

