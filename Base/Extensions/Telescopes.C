/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "config.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Telescopes.h"
#include "Pulsar/Telescope.h"
#include "tempo++.h"
#include "coord.h"
#include "Warning.h"

#ifdef HAVE_TEMPO2
#include "T2Observatory.h"
#endif

#include "debug.h"

static Warning warn;

void Pulsar::Telescopes::set_telescope_info (Telescope *t, Archive *a)
{
  std::string emsg;

  /* WvS 2020-09-11
   * The following line makes things a little more robust to errors such as
   * telescope codes not found, or conflicting one-letter codes in use in
   * different places.  Without this line, the Telescope::name attribute
   * is left 'unknown' even when the Archive::telescope attribute is set.
   */

  t->set_name( a->get_telescope() );

  char oldcode = ' '; // should replace with new codes before we run out of letters!

#ifdef HAVE_TEMPO2

  DEBUG("Telescopes::set_telescope_info HAVE_TEMPO2");

    try {
        std::string newcode = Tempo2::observatory (a->get_telescope())->get_name();
        if (newcode.compare("JB_42ft")==0){
            Telescopes::Jodrell(t);
            t->set_name("JB_42ft");
            oldcode=0;
        }
        if (newcode.compare("JB_MKII")==0){
            Telescopes::Jodrell(t);
            t->set_name("JB_MKII");
            oldcode=0;
        }

        if (oldcode != 0)
        {
          DEBUG("Telescopes::set_telescope_info Tempo2::observatory->get_code");
          oldcode = Tempo2::observatory (a->get_telescope())->get_code();
        }
    }
    catch (Error& error)
    {
      DEBUG("Telescopes::set_telescope_info HAVE_TEMPO2 call Tempo::code");
      oldcode = Tempo::code( a->get_telescope() );
    }
#else

  DEBUG("Telescopes::set_telescope_info call Tempo::code");
  oldcode=Tempo::code( a->get_telescope() );
#endif

  switch ( oldcode )
    {

        case 0:
            // code was set by tempo2
            break;

        case '1':
            Telescopes::GBT(t);
            // Hack to pick correct focus type for GBT
            if (a->get_centre_frequency()<1200.0)
                t->set_focus(Telescope::PrimeFocus);
            break;

        case '3':
            Telescopes::Arecibo(t);
            break;

        case '4':
            Telescopes::MtPleasant26(t);
            break;

        case '6':
        case 'c':
            Telescopes::VLA(t);
            break;

        case '7':
            Telescopes::Parkes(t);
            break;

        case 'e':
            Telescopes::MOST(t);
            break;

        case '8':
            Telescopes::Jodrell(t);
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

        case 't':
            Telescopes::LOFAR(t);
            break;

        case 'm':
            Telescopes::MeerKAT(t);
            break;

        case 'n':
            Telescopes::NenuFAR(t);
            break;

        case 'i':
            Telescopes::WSRT(t);
            break;

        case 's':
            Telescopes::SHAO(t);
            break;

        case 'u':
#define u_is_MWA 0
#if u_is_MWA
            Telescopes::MWA(t);
#else
            Telescopes::FR606(t);
#endif
            break;

        case 'x':
            Telescopes::LWA(t);
            break;

        case 'y':
            Telescopes::CHIME(t);
            break;

        case 'z':
            Telescopes::SRT(t);
            break;

        default: 
            // Unknown code, throw error after calling Telecope::set_coordinates
            emsg = "Unrecognized telescope code (" + a->get_telescope() + ")";
            warn << emsg << std::endl;
            break;
    }

    try
    {
        t->set_coordinates();
    }
    catch (Error& error)
    {
        throw error += "Pulsar::Telescopes::set_telescope_info";
    }

    if (!emsg.empty())
      throw Error (InvalidParam, "Pulsar::Telescopes::set_telescope_info", emsg);
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
}

void Pulsar::Telescopes::Arecibo(Telescope *t)
{
    t->set_name("Arecibo");
    t->set_mount(Telescope::Horizon);
    t->set_primary(Telescope::Spherical);
    t->set_focus(Telescope::Gregorian); // What about CH receivers?
}

void Pulsar::Telescopes::GB140(Telescope *t)
{
    t->set_name("GB 140ft");
    t->set_mount(Telescope::Equatorial);
    t->set_primary(Telescope::Parabolic);
    t->set_focus(Telescope::PrimeFocus); 
}

void Pulsar::Telescopes::GB85_3(Telescope *t)
{
    t->set_name("GB 85-3");
    t->set_mount(Telescope::Equatorial);
    t->set_primary(Telescope::Parabolic);
    t->set_focus(Telescope::PrimeFocus); 
}

void Pulsar::Telescopes::Nancay(Telescope *t)
{
    t->set_name("Nancay");
    t->set_mount(Telescope::KrausType);
    t->set_primary(Telescope::Spherical);
    t->set_focus(Telescope::Gregorian); 
}

void Pulsar::Telescopes::Effelsberg(Telescope *t)
{
    t->set_name("Effelsberg");
    t->set_mount(Telescope::Horizon);
    t->set_primary(Telescope::Parabolic);
    t->set_focus(Telescope::Gregorian); // XXX also varies by receiver
}

void Pulsar::Telescopes::LOFAR(Telescope *t)
{
    t->set_name ("LOFAR");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);

}

void Pulsar::Telescopes::DE601(Telescope *t)
{
    t->set_name ("DE601");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::DE602(Telescope *t)
{
    t->set_name ("DE602");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::DE603(Telescope *t)
{
    t->set_name ("DE603");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::DE604(Telescope *t)
{
    t->set_name ("DE604");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::DE605(Telescope *t)
{
    t->set_name ("DE605");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::FR606(Telescope *t)
{
    t->set_name ("FR606");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::SE607(Telescope *t)
{
    t->set_name ("SE607");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::UK608(Telescope *t)
{
    t->set_name ("UK608");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::DE609(Telescope *t)
{
    t->set_name ("DE609");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::PL610(Telescope *t)
{
    t->set_name ("PL610");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::PL611(Telescope *t)
{
    t->set_name ("PL611");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::PL612(Telescope *t)
{
    t->set_name ("PL612");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::IE613(Telescope *t)
{
    t->set_name ("IE613");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::UTR2(Telescope *t)
{
    t->set_name ("UTR2");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::MeerKAT(Telescope *t)
{
    t->set_name ("MeerKAT");
    t->set_mount (Telescope::Horizon);
    t->set_primary (Telescope::Parabolic);
    t->set_focus(Telescope::Gregorian);
}

void Pulsar::Telescopes::MOST(Telescope *t)
{
  t->set_name("MOST");
    // XXX what about other settings? mount, focus,...
}

void Pulsar::Telescopes::MtPleasant26(Telescope *t)
{
    t->set_name ("Hobart");
    t->set_mount (Telescope::Meridian);
    t->set_primary (Telescope::Parabolic);
    t->set_focus (Telescope::PrimeFocus);
}

void Pulsar::Telescopes::Parkes(Telescope *t)
{
    t->set_name ("Parkes");
    t->set_mount (Telescope::Horizon);
    t->set_primary (Telescope::Parabolic);
    t->set_focus (Telescope::PrimeFocus);
}

void Pulsar::Telescopes::Jodrell(Telescope *t)
{
    t->set_name ("Jodrell");
    t->set_mount (Telescope::Horizon);
    t->set_primary (Telescope::Parabolic);
    t->set_focus (Telescope::PrimeFocus);
}

void Pulsar::Telescopes::WSRT(Telescope *t)
{
    t->set_name("WSRT");
    t->set_mount(Telescope::Equatorial);
    t->set_primary(Telescope::Parabolic);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::VLA(Telescope *t)
{
    t->set_name("VLA");
    t->set_mount(Telescope::Horizon);
    t->set_primary(Telescope::Parabolic);
    t->set_focus(Telescope::Gregorian);
}

void Pulsar::Telescopes::SHAO(Telescope *t)
{
    t->set_name("SHAO");
    t->set_mount(Telescope::Horizon);
    t->set_primary(Telescope::Parabolic);
    t->set_focus(Telescope::Gregorian);
}

void Pulsar::Telescopes::LWA(Telescope *t)
{
    t->set_name("LWA");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::CHIME(Telescope *t)
{
    t->set_name("CHIME");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::MWA(Telescope *t)
{
    t->set_name("MWA");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}

void Pulsar::Telescopes::SRT(Telescope *t)
{
    t->set_name ("SRT");
    t->set_mount (Telescope::Horizon);
    t->set_primary (Telescope::Parabolic);
    t->set_focus (Telescope::PrimeFocus);
}

void Pulsar::Telescopes::NenuFAR(Telescope *t)
{
    t->set_name("NenuFAR");
    // XXX Not sure if these are correct...
    t->set_mount(Telescope::Fixed);
    t->set_focus(Telescope::PrimeFocus);
}
