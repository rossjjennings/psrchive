/***************************************************************************
 *
 *   Copyright (C) 2003-2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/FluxCalibratorPolicy.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/FluxCalibratorInfo.h"

#ifdef HAVE_SPLINTER
#include "Pulsar/CalibrationInterpolator.h"
#endif

#include "Pulsar/CalibrationInterpolatorExtension.h"

#include "Pulsar/StandardCandles.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/CalibratorTypes.h"
#include "Pulsar/BasisCorrection.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/SquareWave.h"

#include "Pulsar/Receiver.h"

#include "Error.h"
#include "interpolate.h"
#include "templates.h"
#include "Pauli.h"

#include <assert.h>

using namespace std;

/*! 
  If a Pulsar::Archive is provided, and if it contains a
  PolnCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::FluxCalibrator::FluxCalibrator (const Archive* archive)
{
  // cerr << "FluxCalibrator ctor this=" << this << endl;

  init ();

  if (!archive)
    return;

  if (archive->get<CalibrationInterpolatorExtension> ())
  {
    set_calibrator( archive );

#ifdef HAVE_SPLINTER
    DEBUG("FluxCalibrator ctor set variation");
    set_variation( new CalibrationInterpolator (this) );
#else
    throw Error (InvalidState, "FluxCalibrator ctor",
		 "Archive has CalibrationInterpolatorExtension\n\t"
		 "but SPLINTER library not available to interpret it");
#endif
  }
    
  flux_extension = archive->get<FluxCalibratorExtension>();
  if (flux_extension)
  {
    // store the calibrator archive
    set_calibrator( archive );

    // store the extension
    extension = flux_extension;

    // store the filename
    filenames.push_back( archive->get_filename() );

    // disable checks for sufficient data
    have_on = have_off = true;
  }
  else
    add_observation (archive);
}

Pulsar::FluxCalibrator::~FluxCalibrator ()
{
  // cerr << "FluxCalibrator dtor this=" << this << endl;
}

void Pulsar::FluxCalibrator::init ()
{
  type = new CalibratorTypes::Flux;
  policy = new FluxCalibrator::VariableGain;
    
  calculated = have_on = have_off = false;
  outlier_threshold = 0.0;
}

void Pulsar::FluxCalibrator::set_policy (Policy* p)
{
  policy = p;
}

string Pulsar::FluxCalibrator::get_standard_candle_info () const
{
  return standard_candle_info;
}

double Pulsar::FluxCalibrator::meanTsys () try
{
  MeanEstimate<double> mean;

  unsigned invalid_count = 0;

  for (unsigned i = 0; i < data.size(); i++)
    if (get_valid(i))
      mean += data[i]->get_S_sys ();
    else
      invalid_count ++;

  if (invalid_count && verbose)
    cerr << "Pulsar::FluxCalibrator::meanTsys " << invalid_count << " invalid channels out of " << data.size() << endl; 

  return mean.get_Estimate().val;
}
catch (Error& error)
{
  throw error += "Pulsar::FluxCalibrator::meanTsys";
}

void Pulsar::FluxCalibrator::data_range_check (unsigned ichan, 
                                               const char* method) const
{
  if (ichan >= data.size())
    throw Error (InvalidParam, method,
                 "ichan=%d > data.size=%d", ichan, data.size());

  if (!data[ichan])
    throw Error (InvalidParam, method, "data[%u] is not valid", ichan);
}

double Pulsar::FluxCalibrator::Tsys (unsigned ichan)
{
  data_range_check (ichan, "Pulsar::FluxCalibrator::Tsys");

  return data[ichan]->get_S_sys().get_value();
}

//! Print all the fluxcal info in simple ascii columns
void Pulsar::FluxCalibrator::print (std::ostream& os) try
{
  if (!has_calibrator())
    throw Error (InvalidState, "Pulsar::FluxCalibrator::print",
		 "no FluxCal Archive");

  for (unsigned ic=0; ic<get_nchan(); ic++)
  {
    // Skip invalid channels
    if (!get_valid(ic)) continue;
    os << ic << " ";
    for (unsigned ir=0; ir<get_nreceptor(); ir++)
    {
      os << data[ic]->get_S_sys(ir).get_value() << " " 
        << data[ic]->get_S_sys(ir).get_error() << "  "
        << data[ic]->get_S_cal(ir).get_value() << " "
        << data[ic]->get_S_cal(ir).get_error() << "  ";
    }
    os << std::endl;
  }
}
catch (Error& error)
{
  throw error += "Pulsar::FluxCalibrator::print";
}

//! Return an estimate of the artificial cal Stokes parameters
/*! This method uses the flux cal measurement to determine 
 * the intrinsic Stokes parameters of the artifical cal 
 * source (reference source).  This is based on the assumptions 
 * that the standard candle is unpolarized, and that the reference
 * source signal is 100% correlated in each receptor with zero phase,
 * but may have unequal power in the two sides.
 */
const Pulsar::CalibratorStokes*
Pulsar::FluxCalibrator::get_CalibratorStokes () const try
{
  if (calibrator_stokes)
    return calibrator_stokes;
  
  // Check that we have both polns
  if (get_nreceptor() != 2) 
    throw Error (InvalidState, "Pulsar::FluxCalibrator::get_CalibratorStokes",
        "nreceptor=%d != 2", get_nreceptor());

  calibrator_stokes = new CalibratorStokes;
  calibrator_stokes->set_nchan(get_nchan());

  const Receiver* receiver = get_Archive()->get<Receiver>();
  
  // Loop over chans
  for (unsigned ichan=0; ichan<get_nchan(); ichan++)
  {
    Estimate<double> diff, cross;
    Stokes < Estimate<double> > stokes;

    // Skip invalid channels
    if (get_valid(ichan)==false || data[ichan]->get_S_cal()==0.0 || 
        data[ichan]->get_S_cal(0)==0.0 || data[ichan]->get_S_cal(1)==0.0)
    { 
      stokes[0] = Estimate<double> (0.0, 0.0);
      stokes[1] = Estimate<double> (0.0, 0.0);
      stokes[2] = Estimate<double> (0.0, 0.0);
      stokes[3] = Estimate<double> (0.0, 0.0);
      calibrator_stokes->set_stokes(ichan, stokes);
      calibrator_stokes->set_valid(ichan,false);
      continue;
    }

    // CalibratorStokes only stores fractional Stokes params
    stokes[0] = Estimate<double> (1.0,0.0);

    diff = (data[ichan]->get_S_cal(0) - data[ichan]->get_S_cal(1))
      / data[ichan]->get_S_cal();

    cross = data[ichan]->get_S_cal(0) * data[ichan]->get_S_cal(1);
    if (cross<0.0) { cross = -cross; }
    cross = 2.0 * sqrt(cross) / data[ichan]->get_S_cal(); 

    // This method may be more general.. applies the inverse basis
    // correction for the receiver, to produce the cal stokes parameters
    // "in the sky".  Also tries to account for the reference source 
    // phase setting.  This may still have trouble with circular feeds
    // as discussed below.
    if (receiver) {

      Jones<double> basis_correction (1.0);
      BasisCorrection corr;
      basis_correction = corr(receiver);

      double phi = receiver->get_reference_source_phase().getRadians();

      stokes[1] = diff;
      stokes[2] = Estimate<double> (cross.val * cos(phi), cross.var);
      stokes[3] = Estimate<double> (cross.val * sin(phi), cross.var);
      stokes = transform(stokes, inv(basis_correction));

    }
    
    // If no receiver extension, fall back on the original code..
    else {

      // Take care of hand convention here
      if (receiver && receiver->get_hand()==Signal::Left)
        diff = -diff;

      // This uses the Archive basis as a proxy for the cal signal 
      // properties.  It could fail if for example the signal was
      // converted from lin to circ using a hybrid AFTER the cal signal
      // was coupled in.  In that case, the Archive data would appear
      // circular, but the cal signal is really linear (nonzero Q not V).
      // TODO: Use the Receiver reference source angle setting?
      if (get_Archive()->get_basis() == Signal::Linear)
      {
        // Linear feeds, Q = diff, U = cross, V = 0
        stokes[1] = diff;
        stokes[2] = cross;
        stokes[3] = Estimate<double> (0.0, cross.var);
      }
      else if (get_Archive()->get_basis() == Signal::Circular)
      {
        // Circular feeds, Q = cross, U = 0, V = diff
        stokes[1] = cross;
        stokes[2] = Estimate<double> (0.0, cross.var);
        stokes[3] = diff;
      }

    }

    calibrator_stokes->set_stokes(ichan, stokes);
  }

  return calibrator_stokes;
}
catch (Error& error)
{
  throw error += "Pulsar::FluxCalibrator::get_CalibratorStokes";
}

float Pulsar::FluxCalibrator::get_weight (unsigned ichan) const
{
  return (get_valid(ichan)) ? 1.0 : 0.0;
}

//! Return true if the flux scale for the specified channel is valid
bool Pulsar::FluxCalibrator::get_valid (unsigned ch) const try
{
  if (ch >= data.size())
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::get_valid",
                 "ichan=%u >= size=%u", ch, data.size());

  if (!data[ch])
    return false;
  else
    return data[ch]->get_solution_available();
}
catch (Error& error)
{
  throw error += "Pulsar::FluxCalibrator::get_valid";
}

//! Set the flux scale invalid flag for the specified channel
void Pulsar::FluxCalibrator::set_invalid (unsigned ch)
{
  if (ch >= data.size())
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::set_invalid",
                 "ichan=%u >= size=%u", ch, data.size());

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::set_invalid ichan=" << ch << endl;

  if (data[ch])
    data[ch]->set_valid (false);
}

void Pulsar::FluxCalibrator::add_observation (const Archive* archive)
{
  if (!archive)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
                 "invalid Pulsar::Archive pointer");

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::add_observation source name=" 
         << archive->get_source() << " type=" 
         << Signal::Source2string(archive->get_type()) << endl;

  if ( archive->get_type() != Signal::FluxCalOn &&
       archive->get_type() != Signal::FluxCalOff )
  {
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
		 "Pulsar::Archive='" + archive->get_filename() + "'"
		 "is not a FluxCal");
  }

  string reason;
  if (has_calibrator() &&
      !(get_calibrator()->calibrator_match (archive, reason) &&
	get_calibrator()->processing_match (archive, reason)))
  {
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
		 "mismatch between\n\t" + get_calibrator()->get_filename() +
                 " and\n\t" + archive->get_filename() + reason);
  }

  unsigned nchan = archive->get_nchan ();
  unsigned nreceptor = (archive->get_npol() == 1) ? 1 : 2;

  string filename = archive->get_filename ();
  bool rename_calibrator = false;

  if (!has_calibrator()) {

    set_calibrator(archive);
    resize (nchan, nreceptor);

  }
  else if (get_calibrator()->get_type() != Signal::FluxCalOn &&
	   archive->get_type() == Signal::FluxCalOn)  {

    // Keep the FPTM naming convention in which the
    // Pulsar::FluxCalibrator is named for the first on-source
    // observation
    set_calibrator(archive);
    rename_calibrator = true;

  }

  assert (data.size() == nchan);

  Reference::To<Pulsar::Archive> clone;
  
  if (archive->get_state () == Signal::Stokes)
  {
    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::add_observation clone Stokes->Coherence"
           << endl;

    clone = archive->clone();
    clone->convert_state (Signal::Coherence);

    archive = clone;
  }


  vector< vector< Estimate<double> > > cal_hi;
  vector< vector< Estimate<double> > > cal_lo;

  unsigned nsub = archive->get_nsubint();

  SquareWave estimator;
  estimator.set_outlier_threshold (outlier_threshold);
  
  for (unsigned isub=0; isub < nsub; isub++)
  {
    const Pulsar::Integration* integration = archive->get_Integration (isub);

    if (verbose > 2) 
      cerr << "Pulsar::FluxCalibrator call Integration::cal_levels" << endl;

    estimator.levels( integration, cal_hi, cal_lo );

    for (unsigned ichan=0; ichan<nchan; ++ichan)
    {
      if (integration->get_weight(ichan) == 0)
	continue;

      for (unsigned ir=0; ir < nreceptor; ir++)
      {
	if (cal_lo[ir][ichan].val == 0)
	{
	  if (verbose > 2)
	    cerr << "Pulsar::FluxCalibrator::add_observation ir="
		 << ir << " ichan=" << ichan
		 << " division by zero" << endl;
	  continue;
	}

	data[ichan]->integrate (archive->get_type(), ir,
				cal_hi[ir][ichan],
				cal_lo[ir][ichan]);
	
      } // for each receptor

    } // for each frequency channel

  } // for each sub-integration

  if (rename_calibrator)
    filenames.insert (filenames.begin(), filename);
  else
    filenames.push_back (filename);

  calculated = false;
  calibrator_stokes = 0;
  
  if (archive->get_type() == Signal::FluxCalOn)
    have_on = true;
  if (archive->get_type() == Signal::FluxCalOff)
    have_off = true;

}

//! Set the database containing flux calibrator information
void Pulsar::FluxCalibrator::set_database (const StandardCandles* d)
{
  database = d;
}

//! Calibrate the flux in the given archive
void Pulsar::FluxCalibrator::calibration_setup (const Archive* arch)
{
  if (!has_calibrator())
    throw Error (InvalidState, "Pulsar::FluxCalibrator::calibrate",
		 "no FluxCal Archive");

  constant_scale = false;

  if ( arch->get_scale() == Signal::FluxDensity
       && dynamic_cast<ConstantGain*>(policy.get()) )
  {
    cerr << "Pulsar::FluxCalibrator::calibrate assuming constant scale"
	 << endl;

    constant_scale = true;
  }
  else if (arch->get_scale() != Signal::ReferenceFluxDensity)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::calibrate", 
                 "Archive scale != ReferenceFluxDensity");

  string reason;
  if (!get_calibrator()->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
		 "mismatch between calibrator\n\t" 
		 + get_calibrator()->get_filename() +
                 " and\n\t" + arch->get_filename() + reason);

  if (variation)
  {
    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::calibrate Variation::update" << endl;
    variation->update (arch->get_Integration(0));
  }

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::calibrate create" << endl;

  create (arch->get_nchan());
}

//! Calibrate the flux in the given archive
void Pulsar::FluxCalibrator::calibrate (Archive* arch)
{
  calibration_setup (arch);

  for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
    calibrate (arch->get_Integration(isub));

  arch->set_scale (Signal::Jansky);
}


void Pulsar::FluxCalibrator::create (unsigned required_nchan)
{
  if (!has_calibrator())
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
		 "no FluxCal Archive");

  if (flux_extension)
  {
    unsigned nchan = flux_extension->get_nchan();
    data.resize( nchan );

    if (verbose)
      cerr << "FluxCalibrator::create extension nchan=" << nchan << endl;

    if (flux_extension->has_scale())
    {
      if (verbose > 2)
        cerr << "Pulsar::FluxCalibrator::create setting constant_scale=true" << endl;
      policy = new ConstantGain;
      constant_scale = true;
    }
    else
      constant_scale = false;

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (flux_extension->has_scale())
      {
	ConstantGain* cg = new ConstantGain;
	cg->set_scale( flux_extension->get_solution(ichan)->scale );
	cg->set_gain_ratio( flux_extension->get_solution(ichan)->ratio );
	data[ichan] = cg;
      }
      else
	data[ichan] = new VariableGain;

      data[ichan]->set( flux_extension->get_solution(ichan)->S_sys,
			flux_extension->get_solution(ichan)->S_cal );
      
      data[ichan]->set_valid ( flux_extension->get_weight(ichan) != 0 );
    }
  }
  
  unsigned nchan = data.size ();

  if (!required_nchan)
    required_nchan = nchan;

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::create nchan=" << nchan 
	 << " required nchan=" << required_nchan << endl;

  if (calculated && gain.size() == required_nchan)
    return;

  if (!have_on)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
                 "no FluxCal-On data");
  if (!have_off)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
                 "no FluxCal-Off data");

  setup ();

  gain.resize (nchan);

  unsigned successful = 0;

  for (unsigned ichan=0; ichan<nchan; ++ichan) try
  {
    if (constant_scale)
    {
      ConstantGain* cg = dynamic_cast<ConstantGain*>( data[ichan].get() );
      gain[ichan] = cg->get_gain().get_value();
    }
    else
      gain[ichan] = data[ichan]->get_S_cal().get_value();

    successful ++;
  }
  catch (Error& error)
  {
    gain[ichan] = 0;
  }

  if (verbose)
    cerr << "FluxCalibrator::create success in " << successful << " channels" << endl;

  if (!successful)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
		 "failed in all %u channels", nchan);

  resize (required_nchan);

  calculated = true;
}

void Pulsar::FluxCalibrator::resize (unsigned nchan, unsigned nreceptor)
{
  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::resize nchan=" << nchan << " nreceptor=" << nreceptor << endl;

  data.resize( nchan );
  for (unsigned i=0; i < nchan; i++)
  {
    data[i] = policy->clone();
    data[i]->set_nreceptor (nreceptor);
  }
}

void Pulsar::FluxCalibrator::resize (unsigned required_nchan)
{
  unsigned nchan = gain.size();

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::resize nchan=" << nchan << " required nchan=" << required_nchan << endl;

  if (nchan == required_nchan)
    return;

  if (required_nchan < nchan)
  {
    unsigned nscrunch = nchan / required_nchan;
  
    if (verbose > 2)
        cerr << "Pulsar::FluxCalibrator::resize required nchan="
	     << required_nchan << " < nchan=" << nchan 
	     << " nscrunch=" << nscrunch << endl;

    scrunch (gain, nscrunch, true);
  }
  else
  {
    if (verbose > 2)
        cerr << "Pulsar::FluxCalibrator::resize required nchan="
             << required_nchan << " > nchan=" << nchan 
             << "\n\t Using FFT to interpolate" << endl;

    vector<float> temp (required_nchan);
    fft::interpolate (temp, gain);
    gain = temp;
  }
}


void Pulsar::FluxCalibrator::setup () try 
{
  // if built from an extension, ensure that data are unpacked
  if (flux_extension)
  {
    unsigned nchan = flux_extension->get_nchan();
    if (data.size() != nchan)
    {
      if (verbose > 2)
        cerr << "Pulsar::FluxCalibrator::setup create nchan=" << nchan << endl;

      create (nchan);
    }

    return;
  } 


  if (!database) {
    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::calculate using default database"<<endl;
    database = new StandardCandles;
  }

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::calculate search for source=" 
	 << get_calibrator()->get_source() << endl;

  StandardCandles::Entry entry;
  entry = database->match (get_calibrator()->get_source(),
			   get_calibrator()->get_centre_frequency());

  if (verbose > 2)
    cerr << "Pulsar::FluxCalibrator::calculate found matching source=" 
	 << entry.source_name[0]<< endl;

  unsigned nchan = data.size();
  
  const Integration* subint = get_calibrator()->get_Integration(0);

  double MHz = subint->get_centre_frequency();

  standard_candle_info = entry.source_name[0] + " -- at " 
    + tostring(MHz,4) + " MHz, S = "
    + tostring(entry.get_flux_mJy(MHz)/1e3,4) + " Jy";

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    double frequency = subint->get_centre_frequency(ichan);
    double source_mJy = entry.get_flux_mJy (frequency);

    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::calculate channel=" << ichan << 
	" freq=" << frequency << " flux=" << source_mJy << endl;

    data[ichan]->set_S_std (source_mJy);
    
  }

}
catch (Error& error) {
  throw error += "Pulsar::FluxCalibrator::setup";
}

void Pulsar::FluxCalibrator::calibrate (Integration* subint)
{
  unsigned npol = subint->get_npol();
  unsigned nchan = subint->get_nchan();

  for (unsigned ichan=0; ichan<nchan; ichan++)
  {
    if (verbose > 2)
      cerr << "Pulsar::FluxCalibrator::calibrate ichan=" << ichan 
	   << " gain=" << gain[ichan] << endl;

    if (gain[ichan] == 0)
      subint->set_weight (ichan, 0.0);
    else
      for (unsigned ipol=0; ipol<npol; ipol++)
	subint->get_Profile (ipol, ichan) -> scale (gain[ichan]);
  }
}

//! Get the number of frequency channels in the calibrator
unsigned Pulsar::FluxCalibrator::get_nchan () const
{
  return data.size();
}

//! Get the number of receptors
unsigned Pulsar::FluxCalibrator::get_nreceptor () const
{
  if (data.size())
    return data[0]->get_nreceptor();
  else
    return 0;
}


Pulsar::Calibrator::Info* Pulsar::FluxCalibrator::get_Info () const
{
  const_cast<FluxCalibrator*>(this)->setup();
  return new FluxCalibrator::Info (this);
}

Pulsar::CalibratorExtension*
Pulsar::FluxCalibrator::new_Extension () const
{
  const_cast<FluxCalibrator*>(this)->setup();
  return new FluxCalibratorExtension (this);
}

