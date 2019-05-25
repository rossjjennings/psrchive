/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnCalExtFreqIntegrate.h"
#include "Pulsar/WeightedFrequency.h"

#include "Error.h"

using namespace std;
using namespace Pulsar;

//! Default constructor
Pulsar::PolnCalExtFreqIntegrate::PolnCalExtFreqIntegrate ()
{
  set_range_policy( new EvenlySpaced );
}

class PolnCalExtFrequency : public WeightedFrequency
{
  public:

  //! Default constructor
  PolnCalExtFrequency (const PolnCalibratorExtension* ext) { extension = ext; }

  //! Set the sub-integration from which to get weights and frequencies
  void set_extension (const PolnCalibratorExtension* ext) { extension = ext; }

  //! Get the number of frequencies available
  unsigned get_nindex () const { return extension->get_nchan(); }

  //! Get the frequency for the specified index
  double get_frequency (unsigned index) const { return extension->get_centre_frequency (index); }

  //! Get the weight for the specified index
  double get_weight (unsigned index) const { return extension->get_weight (index); }

  protected:

  Reference::To<const PolnCalibratorExtension> extension;
};


void Pulsar::PolnCalExtFreqIntegrate::transform (PolnCalibratorExtension* extension)
{
  PolnCalExtFrequency weighted_frequency (extension);

  range_policy->initialize (this, extension);

  unsigned output_nchan = range_policy->get_nrange();
  unsigned ext_nchan = extension->get_nchan();

  if (ext_nchan <= 1 || output_nchan >= ext_nchan)
  {
    if (Calibrator::verbose) 
      cerr << "Pulsar::PolnCalExtFreqIntegrate::transform nothing to do" << endl;
    return;
  }

  if (Calibrator::verbose)
    cerr << 
      "Pulsar::PolnCalExtFreqIntegrate::transform nchan"
      " input=" << ext_nchan << " output=" << output_nchan << endl;

  unsigned nparam = extension->get_nparam();

  for (unsigned ichan=0; ichan < output_nchan; ichan++) try
  {     
    unsigned start = 0;
    unsigned stop = 0;
    range_policy->get_range (ichan, start, stop);
    
    double reference_frequency = weighted_frequency (start,stop);

    cerr << "ichan=" << ichan << " start=" << start << " stop=" << stop << endl;

    if (Calibrator::verbose)
      cerr << "Pulsar::PolnCalExtFreqIntegrate::transform ichan=" << ichan 
	   << " freq=" << reference_frequency << endl;

    // copy the first good transformation to the target transformation
    for (unsigned jchan=start; jchan<stop; jchan++)
      if (extension->get_valid(jchan))
        *(extension->get_transformation(ichan)) = *(extension->get_transformation(jchan));

    for (unsigned iparam=0; iparam < nparam; iparam++)
    {
      MeanEstimate<double> mean;

      for (unsigned jchan=start; jchan<stop; jchan++)
        if (extension->get_valid(jchan))
          mean += extension->get_transformation(jchan)->get_Estimate(iparam);

      Estimate<double> value = mean.get_Estimate();
      extension->get_transformation(ichan)->set_Estimate(iparam, value);
    }

    extension->set_centre_frequency (ichan, reference_frequency);

  }
  catch (Error& error)
  {
   throw error += "PolnCalExtFreqIntegrate::transform";
  }

  if (Calibrator::verbose)
    cerr << "Pulsar::PolnCalExtFreqIntegrate::transform resize" << endl;

  extension->set_nchan (output_nchan);

  if (Calibrator::verbose) 
    cerr << "Pulsar::PolnCalExtFreqIntegrate::transform finish" << endl;
}

static Pulsar::PolnCalExtFreqIntegrate* operation = 0;
static Pulsar::PolnCalExtFreqIntegrate::EvenlySpaced* policy = 0;

static void static_init ()
{
  operation = new Pulsar::PolnCalExtFreqIntegrate;
  policy    = new Pulsar::PolnCalExtFreqIntegrate::EvenlySpaced;

  operation->set_range_policy( policy );
}

/*!
  \param nscrunch number of neighbouring frequency channels to
  integrate; if zero, then all channels are integrated into one
 */
void Pulsar::PolnCalibratorExtension::fscrunch (unsigned nscrunch)
{
  if (!policy)
    static_init ();

  policy->set_nintegrate (nscrunch);
  operation->transform (this);
}

void Pulsar::PolnCalibratorExtension::fscrunch_to_nchan (unsigned new_chan)
{
  if (new_chan == 0)
    throw Error (InvalidParam, "Pulsar::PolnCalibratorExtension::fscrunch_to_nchan",
                               "requested nchan=0");

  if (get_nchan() % new_chan)
    throw Error (InvalidParam, "Pulsar::PolnCalibratorExtension::fscrunch_to_nchan",
                             "requested nchan=%u modulo current nchan=%u is non-zero",
                 new_chan, get_nchan());

  fscrunch(get_nchan() / new_chan);
}

