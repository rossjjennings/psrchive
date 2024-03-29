/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArrivalTime.h"
#include "Pulsar/ProfileStandardShift.h"
#include "Pulsar/PolnProfileShiftEstimator.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Statistics.h"
#include "Pulsar/Backend.h"

#include "Pulsar/Flux.h"
#include "Pulsar/StandardFlux.h"

#include "Pulsar/Pulsar.h"

#include <strings.h>

using namespace std;

Pulsar::Option<std::string> 
Pulsar::ArrivalTime::default_format (
    "ArrivalTime::default_format", "Parkes",
    "Default TOA line format",
    "TOAs can be generated in a number of different formats.  Valid choices\n"
    "are:  Parkes, Princeton, ITOA, Psrclock and Tempo2."
    );

Pulsar::ArrivalTime::ArrivalTime ()
{
  format = Tempo::toa::Parkes;
  set_format(default_format);
  skip_bad = true;
}

Pulsar::ArrivalTime::~ArrivalTime ()
{
}

//! Prepare the data for use
void Pulsar::ArrivalTime::preprocess (Archive* archive)
{
  shift_estimator->preprocess (archive);
}

//! Set the observation from which the arrival times will be derived
void Pulsar::ArrivalTime::set_observation (const Archive* archive)
{
  observation = archive;
}

//! Set the standard/template used by some phase shift estimators
void Pulsar::ArrivalTime::set_standard (const Archive* archive)
{
  standard = archive;
  standard_update ();
}

//! Set the algorithm used to estimate the phase shift
void Pulsar::ArrivalTime::set_shift_estimator (ShiftEstimator* shift)
{
  shift_estimator = shift;
  standard_update();
}

Pulsar::ShiftEstimator* Pulsar::ArrivalTime::get_shift_estimator () const
{
  return shift_estimator;
}

//! Set the algorithm used to estimate the flux density
void Pulsar::ArrivalTime::set_flux_estimator (Flux* flux)
{
  flux_estimator = flux;
  standard_update();
}

Pulsar::Flux* Pulsar::ArrivalTime::get_flux_estimator () const
{
  return flux_estimator;
}

void Pulsar::ArrivalTime::standard_update(unsigned ichan)
{
  if (!standard)
    return;

  if (shift_estimator)
  {

    ProfileStandardShift* shift;
    shift = dynamic_cast<ProfileStandardShift*> (shift_estimator.get());

    if (shift)
      shift->set_standard (standard->get_Profile (0,0,ichan));
  }

  if (flux_estimator)
  {
    StandardFlux* flux;
    flux = dynamic_cast<StandardFlux*> (flux_estimator.get());

    if (flux)
      flux->set_standard (standard->get_Profile (0,0,ichan));
  }
}

//! Set the format of the output time-of-arrival estimates
void Pulsar::ArrivalTime::set_format (Tempo::toa::Format fmt)
{
  format = fmt;
}

//! Set additional format flags
void Pulsar::ArrivalTime::set_format_flags (const std::string& flags)
{
  format_flags = flags;
}

//! Set additional format flags
void Pulsar::ArrivalTime::set_attributes (const std::vector<std::string>& attr)
{
  attributes = attr;

  // Check for presence of flux attribute to enable flux estimate
  if (!flux_estimator)
  {
    for (unsigned i=0; i<attributes.size(); i++) 
    {
      if (attributes[i] == "flux") 
      {
        // StandardFlux is the only implementation of Flux as of now.  If more
        // algorithms appear, should implement some factory-style way of choosing
        // the appropriate one, similar to the shift algorithms.
        set_flux_estimator(new StandardFlux); 
        break;
      }
    }
  }

}

//! Set additional TOA line text
void Pulsar::ArrivalTime::set_extra_text (const std::string& text)
{
  extra_text = text;
}

//! Add to the vector of time-of-arrival estimates
void Pulsar::ArrivalTime::get_toas (std::vector<Tempo::toa>& toas)
{
  if (observation->get<IntegrationOrder>())
    throw Error (InvalidState, "Pulsar::ArrivalTime::get_toas",
		 "Archive not in time order");

  if (standard)
  {
    string reason;
    if (!observation->standard_match (standard, reason))
      warning << "Pulsar::ArrivalTime::get_toas WARNING " << reason.c_str() << endl;

    double cf1 = observation->get_centre_frequency();
    double cf2 = standard->get_centre_frequency();
    double dfreq = fabs (cf2 - cf1);

    if (dfreq > 0.2 * cf1)
      cerr << "Pulsar::ArrivalTime::get_toas WARNING centre frequency standard= " << cf2 << " != observation=" << cf1 << endl;
  }

  const unsigned nsub = observation->get_nsubint();
  for (unsigned isub=0; isub<nsub; isub++)
  {
    vector<Tempo::toa> new_toas;

    get_toas (isub, new_toas);

    dress_toas (isub, new_toas);

    for (unsigned i=0; i<new_toas.size(); i++)
      toas.push_back (new_toas[i]);
  }
}

void Pulsar::ArrivalTime::get_toas (unsigned isub,
				    std::vector<Tempo::toa>& toas)
{
  bool multichannel_standard = standard && (standard->get_nchan() > 1);

  // Get a time adjustment from be_delay
  const Backend *be = observation->get<Backend>();

  const Integration* subint = observation->get_Integration(isub);

  if (Archive::verbose > 3)
    cerr << "Pulsar::ArrivalTime::get_toas isub=" << isub 
	 << " nchan=" << subint->get_nchan() << endl;

  for (unsigned ichan=0; ichan < subint->get_nchan(); ++ichan)
  {
    if (multichannel_standard)
      standard_update(ichan);

    const Profile* profile = subint->get_Profile (0, ichan);

    if ((skip_bad && (profile->get_weight() == 0))
	|| (multichannel_standard 
          && standard->get_Profile (0,0,ichan)->get_weight() == 0))
      continue;
    
    try
    {
      setup (subint, ichan);
      Estimate<double> shift = shift_estimator->get_shift ();

      Tempo::toa arrival_time = get_toa (shift, subint, ichan);
      arrival_time.set_reduced_chisq( shift_estimator->get_reduced_chisq () );
      arrival_time.set_StoN( shift_estimator->get_snr () );

      if (flux_estimator)
        arrival_time.set_flux(flux_estimator->get_flux (profile));

      // Adjust TOA with be_delay value, if present.
      // Positive be_delay means that the file timestamp is 
      // early as compared to the time the signal 
      // arrived at the samplers. So we add be_delay to the TOA here.
      if (be)
        arrival_time.set_arrival(arrival_time.get_arrival() + be->get_delay());

      toas.push_back( arrival_time );

      if (residual)
      {
        Integration* rsubint = residual->get_Integration (isub);
        rsubint->expert()->rotate_phase( shift.get_value() );

        const Integration* std = standard->get_Integration (0);
        foreach (rsubint, std, &Profile::diff);
      }
    }
    catch (Error& error)
    {
      if (Archive::verbose > 2)
        cerr << "Pulsar::Integration::toas error" << error << endl;

      else if (Archive::verbose)
        cerr << error.get_message() << endl;

      continue;
    }
  }
}


void Pulsar::ArrivalTime::setup (const Integration* subint, unsigned ichan)
{
  Reference::To<ProfileShiftEstimator> profile_shift;
  profile_shift = dynamic_cast<ProfileShiftEstimator*>(shift_estimator.get());

  if (profile_shift)
  {
    profile_shift->set_observation( subint->get_Profile (0, ichan) );
    return;
  }

  Reference::To<PolnProfileShiftEstimator> poln_shift;
  poln_shift = dynamic_cast<PolnProfileShiftEstimator*>(shift_estimator.get());

  if (poln_shift)
  {
    poln_shift->set_observation( subint->new_PolnProfile (ichan) );
    return;
  }

  throw Error (InvalidState, "Pulsar::ArrivalTime::setup",
	       "shift estimator is not understood");
}

void Pulsar::ArrivalTime::dress_toas (unsigned isub,
				      std::vector<Tempo::toa>& toas)
{
  std::string nsite = observation->get_telescope();
  // Try to get the special site codes for
  // various Jodrell Bank backends.
  // Perhaps there should be a subroutine for this?
  if (format == Tempo::toa::Tempo2){
      if (nsite == "h" || nsite=="JB_MKII") {
          const Backend* be = observation->get<Backend>();
          if (be) {
              if (be->get_name()=="Jod"){
                  nsite="jbmk2dfb";
              }
              if (be->get_name()=="ROACH"){
                  nsite="jbmk2roach";
              }
          }
      }
      if (nsite == "8" || nsite=="Jodrell") {
          const Backend* be = observation->get<Backend>();
          if (be) {
              if (be->get_name()=="Jod"){
                  nsite="jbdfb";
              }
              if (be->get_name()=="ROACH"){
                  nsite="jbroach";
              }
              if (be->get_name()=="COBRA2") {
                  nsite="jb42";
              }
          }
      }
  } 
  std::string aux_txt;

  if (format == Tempo::toa::Parkes || format == Tempo::toa::Psrclock)
    aux_txt = observation->get_filename() + " " + tostring(isub);

  for (unsigned i=0; i < toas.size(); i++)
  {
    toas[i].set_subint (isub);
    toas[i].set_telescope (nsite);

    toa_subint = isub;
    toa_chan = toas[i].get_channel();

    if (format == Tempo::toa::Tempo2)
      aux_txt = get_tempo2_aux_txt (toas[i]);

    string txt = toas[i].get_auxilliary_text ();
    if (txt.length())
      txt = aux_txt + " " + txt;
    else
      txt = aux_txt;

    toas[i].set_auxilliary_text (txt);
  }
}

Tempo::toa Pulsar::ArrivalTime::get_toa (Estimate<double>& shift,
					 const Pulsar::Integration* subint,
					 unsigned ichan)
{
  Tempo::toa toa (format);

  // phase shift in turns
  toa.set_phase_shift (shift.get_value());

  // topocentric folding period
  double period = subint->get_folding_period();

  // epoch of the integration (rise time of bin 0 in each profile)
  MJD epoch = subint->get_epoch ();

  // arrival time 
  toa.set_arrival (epoch + shift.get_value() * period);

  // arrival time error in microseconds
  toa.set_error (shift.get_error() * period * 1e6);

  if (subint->get_dedispersed())
    toa.set_frequency (subint->get_centre_frequency());
  else
    toa.set_frequency (subint->get_centre_frequency(ichan));

  toa.set_channel (ichan);

  toa.set_dur(subint->get_duration());
  toa.set_bw(subint->get_bandwidth());

  // would like to see this go somewhere else
  if (format == Tempo::toa::Parkes || format == Tempo::toa::Psrclock)
    toa.set_auxilliary_text( tostring(ichan) );

  return toa;
}






void Pulsar::ArrivalTime::set_format (const string& fmt)
{
  if (strcasecmp(fmt.c_str(),"parkes")==0)
    format = Tempo::toa::Parkes;

  else if (strcasecmp(fmt.c_str(),"princeton")==0)
    format = Tempo::toa::Princeton;

  else if (strcasecmp(fmt.c_str(),"itoa")==0)
    format = Tempo::toa::ITOA;

  else if (strcasecmp(fmt.c_str(),"psrclock")==0)
    format = Tempo::toa::Psrclock;

  else if (strcasecmp(fmt.c_str(),"tempo2")==0)
    format = Tempo::toa::Tempo2;
}

/* *************************************************************************

             KLUDGEY FUNCTIONS TO DUPLICATE PAV-LIKE BEHAVIOUR

   ************************************************************************* */

#include "Pulsar/ObsExtension.h"
#include "Pulsar/Backend.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/FITSHdrExtension.h"

using namespace Pulsar;

string get_parang (const Archive* archive )
{
  stringstream result;
  int nsubs = archive->get_nsubint();

  if (nsubs != 0) {
    const Integration* integration = archive->get_Integration( nsubs / 2 );

    if (integration) {
      const Pointing* ext = integration->get<Pointing>();
      
      if (ext) {
	result << ext->get_parallactic_angle().getDegrees();
      }
    }
  }
  
  return result.str();
}

string get_tsub (const Archive* archive)
{
  string result;
  int nsubs = archive->get_nsubint();
  
  if ( nsubs != 0 )
  {
    const Integration* first_int = archive->get_first_Integration();
    
    if( first_int )
      return tostring( first_int->get_duration(), 6 );
  }
  
  return result;
}

string get_observer (const Archive* archive)
{
  string result;
  const ObsExtension* ext = archive->get<ObsExtension>();
  
  if( !ext ) {
    result = "UNDEF";
    
  } else {
    result = ext->get_observer();
  }
  
  return result;
}

string get_projid (const Archive* archive)
{
  string result = "";
  const ObsExtension* ext = archive->get<ObsExtension>();
  
  if( !ext ) {
    result = "UNDEF";
    
  } else {
    result = ext->get_project_ID();
  }
  
  return result;
}

string get_rcvr (const Archive* archive)
{
  string result;
  const Receiver* ext = archive->get<Receiver>();
  
  if( ext ) {
    result = ext->get_name();
  }
  
  return result;
}

string get_backend (const Archive* archive)
{
  string result = "";
  const Backend* ext = archive->get<Backend>();
  
  if (!ext)
    ext = archive->get<WidebandCorrelator>();
  
  if( !ext ) {
    result = "UNDEF";
  } else {
    result = ext->get_name();
  }
  
  return result;
}

string get_period_as_string (const Archive* archive)
{
  // TODO check this
  return tostring( archive->get_Integration(0)->get_folding_period(), 14 );
}

int get_stt_smjd (const Archive* arch)
{
  return arch->get<FITSHdrExtension>()->get_stt_smjd();
}

int get_stt_imjd (const Archive* arch)
{
  return arch->get<FITSHdrExtension>()->get_stt_imjd();
}

string get_be_delay (const Archive* archive)
{
  const Backend* ext = archive->get<WidebandCorrelator>();
  
  if ( !ext )
    return "UNDEF";
  else
    return tostring ( ext->get_delay(), 14 );
}

std::string Pulsar::ArrivalTime::get_value (const std::string& key,
                                            const Tempo::toa& toa) try
{
  if(key == "parang") return get_parang( observation );
  else if(key == "tsub") return get_tsub( observation );
  else if(key == "observer") return get_observer( observation );
  else if(key == "projid") return get_projid( observation );
  else if(key == "rcvr") return get_rcvr( observation );
  else if(key == "backend") return get_backend( observation );
  else if(key == "period") return get_period_as_string( observation );
  else if(key == "be_delay") return get_be_delay( observation );
  else if(key == "subint") return tostring(toa_subint);
  else if(key == "chan") return tostring(toa_chan);
  else if(key == "gof") return tostring( toa.get_reduced_chisq() );
  else if(key == "snr") return tostring( toa.get_StoN(), 5 );
  else if(key == "wt") return tostring( 
      observation->get_Integration(toa_subint)->get_weight(toa_chan), 5);
  else if(key == "flux") return tostring(toa.get_flux().get_value(), 6);
  else if(key == "fluxe") return tostring(toa.get_flux().get_error(), 2);
  else
  {
    Reference::To<TextInterface::Parser> interface;
    interface = standard_interface (const_cast<Archive*>(observation.get()));

    interface->set_prefix_name (false);
    interface->set_indentation ("");

    interface->process( "subint=" + tostring(toa_subint) );
    interface->process( "chan=" + tostring(toa_chan) );

    return process( interface, key );
  }
} 
catch (Error& e)
{
  return "*error*";
}
