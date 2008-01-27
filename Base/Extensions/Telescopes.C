/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Telescopes.h"
#include "Pulsar/Telescope.h"
#include "coord.h"

void Pulsar::Telescopes::set_telescope_info(Telescope *t, Archive *a)
{
  switch (a->get_telescope().at(0)) {
    case '1':
      Telescopes::GBT(t);
      // Hack to pick correct focus type for GBT
      if (a->get_centre_frequency()<1200.0)
        t->set_focus(Telescope::PrimeFocus);
      break;
    case '3':
      Telescopes::Arecibo(t);
      break;
    case 'a':
      Telescopes::GB140(t);
      break;
    case 'b':
      Telescopes::GB85_3(t);
      break;
    case 'f':
      Telescopes::Nancay(t);
      break;
    case 'g':
      Telescopes::Effelsberg(t);
      break;
    default: // Unknown code, throw error
      throw Error (InvalidParam, "Pulsar::Telescopes::set_telescope_info",
          "Unrecognized telescope code (%s)", a->get_telescope().c_str());
      break;
  }
}

// Info for each telescope below.  Maybe the coordinate setting
// routine could be incorporated here, to have a centralized 
// location for all this.  Also would be good to not have to use
// the Tempo codes explicitly below.

void Pulsar::Telescopes::GBT(Telescope *t) 
{
  t->set_name("GBT");
  t->set_mount(Telescope::Horizon);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::Gregorian); // XXX only true for L-band and up
  t->set_coordinates("1");
}

void Pulsar::Telescopes::Arecibo(Telescope *t)
{
  t->set_name("Arecibo");
  t->set_mount(Telescope::Fixed);
  t->set_primary(Telescope::Spherical);
  t->set_focus(Telescope::Gregorian); // What about CH receivers?
  t->set_coordinates("3");
}

void Pulsar::Telescopes::GB140(Telescope *t)
{
  t->set_name("GB 140ft");
  t->set_mount(Telescope::Equatorial);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::PrimeFocus); 
  t->set_coordinates("a");
}

void Pulsar::Telescopes::GB85_3(Telescope *t)
{
  t->set_name("GB 85-3");
  t->set_mount(Telescope::Equatorial);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::PrimeFocus); 
  t->set_coordinates("b");
}

void Pulsar::Telescopes::Nancay(Telescope *t)
{
  t->set_name("Nancay");
  t->set_mount(Telescope::Fixed);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::Gregorian); 
  t->set_coordinates("f");
}

void Pulsar::Telescopes::Effelsberg(Telescope *t)
{
  t->set_name("Effelsberg");
  t->set_mount(Telescope::Horizon);
  t->set_primary(Telescope::Parabolic);
  t->set_focus(Telescope::Gregorian); // XXX also varies by receiver
  t->set_coordinates("g");
}

