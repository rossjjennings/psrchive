/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ZapInterpreter.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"

#include "Pulsar/ChannelZapMedian.h"
#include "Pulsar/LawnMower.h"
#include "Pulsar/RobustMower.h"
#include "Pulsar/Statistics.h"
#include "Pulsar/TimeFrequencyZap.h"
#include "Pulsar/ZapExtend.h"

#include "RobustEstimateZapper.h"
#include "TextInterface.h"
#include "pairutil.h"
#include "evaluate.h"
#include "substitute.h"
#include "Ranges.h"

using namespace std;

string index_help (const string& cmd)
{
  return
    "usage: " + cmd + " iex1 [iex2 ...]\n"
    "  string iexN   unsigned index exression: [i|i1-i2|-iend|istart-]";
}

string pair_help (const string& cmd)
{
  return
    "usage: " + cmd + " i1,j1 [i2,j2 ...]\n"
    "  string i1,j1  unsigned index pair";
}

Pulsar::ZapInterpreter::ZapInterpreter ()
{
  add_command 
    ( &ZapInterpreter::median, 
      "median", "median smooth the passband and zap spikes",
      "usage: median <TI> \n"
      "  type 'zap median help' for text interface help" );

  add_command 
    ( &ZapInterpreter::mow, 
      "mow", "median smooth the profile and clean spikes",
      "usage: mow <TI> \n"
      "  type 'zap mow help' for text interface help" );

  add_command 
    ( &ZapInterpreter::tfzap, 
      "tfzap", "zap subint/chan outliers using time-frequency plane",
      "usage: tfzap <TI> \n"
      "  type 'zap tfzap help' for text interface help" );

  add_command 
    ( &ZapInterpreter::extend, 
      "extend", "extend zapped regions in time and/or frequency",
      "usage: extend <TI> \n"
      "  type 'zap extend help' for text interface help" );

  add_command
    ( &ZapInterpreter::chan,
      "chan", "zap specified channels",
      index_help("chan") );

  add_command
    ( &ZapInterpreter::subint,
      "subint", "zap specified integrations",
      index_help("subint") + "\nsubint set [iex1 iex2 ...]   specify subints for which zap freq/chan apply" );

  add_command
    ( &ZapInterpreter::such,
      "such", "zap specified integration and channel",
      pair_help("such") );

  add_command
    ( &ZapInterpreter::edge,
      "edge", "zap fraction of band edges",
      "usage: edge <fraction> \n"
      "  float <fraction>  fraction of band zapped on each edge \n");

  add_command
    ( &ZapInterpreter::cumulative,
      "cumulative", "zap band edges up to cumulative power threshold",
      "usage: cumulative <threshold> \n"
      "  float <threshold>  fraction of total cumulative power \n");

  add_command
    ( &ZapInterpreter::freq,
      "freq", "zap frequency range(s)",
      "usage: freq < MHz0:MHz1 | >MHz | <MHz > \n"
      "  float pair <MHz0:MHz1> range of frequencies to zap \n"
      "  float [>MHz] zap everything above frequency \n"
      "  float [<MHz] zap everything below frequency \n");

  add_command
    ( &ZapInterpreter::zerodm,
      "zerodm", "",
      "usage: zerodm \n"
      "  Use the 'ZeroDM' RFI removal scheme. \n");

  add_command
    ( &ZapInterpreter::cal,
      "cal", "apply 'freq' and 'chan' commands to calibrator",
      "usage: cal \n"
      "  calling this command toggles the flag on/off \n");
      

  // by default zap freq/chan commands are applied to sub-integration data
  zap_calibrator = false;
}

Pulsar::ZapInterpreter::~ZapInterpreter ()
{
}

string Pulsar::ZapInterpreter::median (const string& args) try
{
  bool expand = false;
  vector<string> arguments = setup (args, expand);

#if _DEBUG
  cerr << "Pulsar::ZapInterpreter::median";
  for (unsigned i=0; i<arguments.size(); i++)
    cerr << " arg[" << i << "]=" << arguments[i];
  cerr << endl;
#endif

  if (!zap_median)
    zap_median = new ChannelZapMedian;

  if (!arguments.size())
  {
    (*zap_median)( get() );
    return response (Good);
  }

  //! Zap median interface
  Reference::To<TextInterface::Parser> interface = zap_median->get_interface();

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++)
  {
    if (icmd)
      retval += " ";
    retval += interface->process (arguments[icmd]);
  }

  return retval;
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ZapInterpreter::mow (const string& args) try
{ 
  if (args == "robust")
  {
    mower = new RobustMower;
    return response (Good);
  }

  vector<string> arguments = setup (args);

  if (!mower)
    mower = new LawnMower;

  if (!arguments.size())
  {
    Reference::To<Archive> data = get();
    for (unsigned isub = 0; isub < data->get_nsubint(); isub++)
    {
      cerr << "mowing subint " << isub << endl;
      mower->transform( data->get_Integration( isub ) );
    }
    return response (Good);
  }

  //! Zap median interface
  Reference::To<TextInterface::Parser> interface = mower->get_interface();

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++) {
    if (icmd)
      retval += " ";
    retval += interface->process (arguments[icmd]);
  }

  return retval;
}
catch (Error& error)
{
  return response (error);
}

string Pulsar::ZapInterpreter::tfzap (const string& args) try
{
  bool expand = false;
  vector<string> arguments = setup (args, expand);

  if (!tf_zapper)
    tf_zapper = new TimeFrequencyZap;

  if (!arguments.size())
  {
    (*tf_zapper)( get() );
    return response (Good);
  }

  //! TF zap interface
  Reference::To<TextInterface::Parser> interface = tf_zapper->get_interface();

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++)
  {
    if (icmd)
      retval += " ";

    try {
      retval += interface->process (arguments[icmd]);
    }
    catch (Error& error) 
    {
      // see if expanding any variables helps
      string arg = ::evaluate( substitute (arguments[icmd], get_interface()) );
      retval += "'" + arguments[icmd] + "' -> '" + arg + "'";
      retval += interface->process (arg);
    }
  }

  return retval;
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ZapInterpreter::extend (const string& args) try
{
  bool expand = false;
  vector<string> arguments = setup (args, expand);

  if (!zap_extend)
    zap_extend = new ZapExtend;

  if (!arguments.size())
  {
    (*zap_extend)( get() );
    return response (Good);
  }

  //! TF zap interface
  Reference::To<TextInterface::Parser> interface = zap_extend->get_interface();

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++)
  {
    if (icmd)
      retval += " ";
    retval += interface->process (arguments[icmd]);
  }

  return retval;
}
catch (Error& error) {
  return response (error);
}

string Pulsar::ZapInterpreter::cal (const string& args)
{
  /*
    Passing false as the second argument to setup disables variable
    substitution and expression evaluation using the Archive::Interpreter.
    
    This is done because any variables will be interpreted using the
    PolnCalibratorExtension::Transformation::Interface class.
  */
  vector<string> arguments = setup (args, false);

  if (arguments.size() == 0)
  {
    zap_calibrator = !zap_calibrator;
    return response (Good);
  }

  if (arguments[0] == "robust")
  {
    if (!robust_estimate_zapper)
      robust_estimate_zapper = new RobustEstimateZapper;

    if (arguments.size() == 1)
    {
      Reference::To<PolnCalibratorExtension> ext;
      ext = get()->get<PolnCalibratorExtension>();
      if (ext)
       for (unsigned iparam=0; iparam < ext->get_nparam(); iparam++)
        robust_estimate_zapper->excise (iparam, ext.get(), 
                                        &PolnCalibratorExtension::get_nchan,
                                        &PolnCalibratorExtension::get_Estimate,
                                        &PolnCalibratorExtension::set_weight);

      Reference::To<CalibratorStokes> cs;
      cs = get()->get<CalibratorStokes>();
      if (cs)
       for (unsigned iparam=0; iparam < cs->get_nparam(); iparam++)
        robust_estimate_zapper->excise (iparam, cs.get(), 
                                        &CalibratorStokes::get_nchan,
                                        &CalibratorStokes::get_Estimate,
                                        &CalibratorStokes::set_valid);

      Reference::To<FluxCalibratorExtension> fext;
      fext = get()->get<FluxCalibratorExtension>();
      if (fext)
       for (unsigned iparam=0; iparam < fext->get_nparam(); iparam++)
        robust_estimate_zapper->excise (iparam, fext.get(),
                                        &FluxCalibratorExtension::get_nchan,
                                        &FluxCalibratorExtension::get_Estimate,
                                        &FluxCalibratorExtension::set_weight);

      if (!ext && !cs && !fext)
        return response (Fail, "archive contains no calibrator extensions");
      else
        return response (Good);
    }

    // parse any configuration options
    TextInterface::Parser* parser = robust_estimate_zapper->get_interface();

    string retval;
    for (unsigned iarg=1; iarg < arguments.size(); iarg++)
      retval += parser->process (arguments[iarg]);

    return response (Good, retval);
  }

  /* If arguments are specified, each is interpreted as an expression
     to be evaluated using the
     PolnCalibratorExtension::Transformation::Interface class.  This
     could also be extended to FluxCalibratorExtension */

  Reference::To<PolnCalibratorExtension> ext;
  ext = get()->get<PolnCalibratorExtension>();
  if (!ext)
    return response (Fail, "archive does not contain PolnCalibratorExtension");

  for (unsigned ichan=0; ichan<ext->get_nchan(); ichan++)
  {
    PolnCalibratorExtension::Transformation::Interface parser;
    parser.set_instance( ext->get_transformation(ichan) );
    
    for (unsigned iarg=0; iarg < arguments.size(); iarg++)
    {
      string expression = arguments[iarg];
      string value = process( &parser, expression );

      if ( compute(value) )
	ext->set_weight( ichan, 0.0 );
    }
  }
  
  return response (Good);
}


string Pulsar::ZapInterpreter::empty ()
{
  return response (Fail, help());
}

void parse_indeces (vector<unsigned>& indeces,
		    const vector<string>& arguments,
		    unsigned limit)
{
  for (unsigned i=0; i<arguments.size(); i++)
  {
    vector<unsigned> tmp;
    TextInterface::parse_indeces (tmp, "[" + arguments[i] + "]", limit);
    for (auto el: tmp) indeces.push_back(el);
  }
}

string Pulsar::ZapInterpreter::chan (const string& args) try 
{
  vector<string> arguments = setup (args);

  if (zap_calibrator)
  {
    Reference::To<CalibratorExtension> ext = get()->get<CalibratorExtension>();
    if (!ext)
      return response (Fail, "archive does not contain CalibratorExtension");

    vector<unsigned> channels;
    parse_indeces (channels, arguments, ext->get_nchan());
    for (unsigned ichan=0; ichan<channels.size(); ichan++)
      ext->set_weight( channels[ichan], 0.0 );

    return response (Good);
  }
  
  vector<unsigned> channels;
  parse_indeces (channels, arguments, get()->get_nchan());

  // zap selected channels in all set sub-integrations
  if (set_subints.size())
  {
    for (unsigned isub=0; isub<set_subints.size(); isub++) {
      Integration* subint = get()->get_Integration(set_subints[isub]);
      for (unsigned i=0; i<channels.size(); i++)
        subint->set_weight( channels[i], 0.0 );
    }
  }

  // zap selected channels in all sub-integrations
  else
  {
    unsigned nsubint = get()->get_nsubint();
    for (unsigned isub=0; isub<nsubint; isub++) {
      Integration* subint = get()->get_Integration(isub);
      for (unsigned i=0; i<channels.size(); i++)
        subint->set_weight( channels[i], 0.0 );
    }
  }
  
  return response (Good);
}
catch (Error& error) {
  return response (error);
}


string Pulsar::ZapInterpreter::subint (const string& args) try 
{
  vector<string> arguments = setup (args);

  // interpret command as setting a specificed set of subints
  if (arguments[0] == "set")
  {
    if (arguments.size()==1)
      set_subints.clear();
    else
    {
      arguments.erase(arguments.begin());
      parse_indeces (set_subints, arguments, get()->get_nsubint());
    }
    return response (Good);
  }

  vector<unsigned> subints;
  parse_indeces (subints, arguments, get()->get_nsubint());

  // zap all channels in selected sub-integrations
  unsigned nchan = get()->get_nchan();
  for (unsigned i=0; i<subints.size(); i++) {
    Integration* subint = get()->get_Integration( subints[i] );
    for (unsigned ichan=0; ichan<nchan; ichan++)
      subint->set_weight( ichan, 0.0 );
  }
  return response (Good);
}
catch (Error& error) {
  return response (error);
}


template<typename T, typename U>
void parse_pairs (vector< pair<T,U> >& pairs,
		  const vector<string>& arguments,
		  T limit_first, const string& name_first,
		  U limit_second, const string& name_second)
{
  // cerr << "parse_pairs: " << arguments.size() << " arguments" << endl;

  pairs.resize( arguments.size() );

  for (unsigned i=0; i<pairs.size(); i++)
  {
    // cerr << "parse_pairs: arg[" << i << "]=" << arguments[i] << endl;

    pairs[i] = fromstring< pair<T,U> > ( "(" + arguments[i] + ")" );
    
    if (pairs[i].first >= limit_first)
    {
      Error error (InvalidParam, "parse_pairs");
      error << "i" << name_first << "=" << pairs[i].first 
	    << " >= n" << name_first << "=" << limit_first;
      throw error;
    }

    if (pairs[i].second >= limit_second)
    {
      Error error (InvalidParam, "parse_pairs");
      error << "i" << name_second << "=" << pairs[i].second
	    << " >= n" << name_second << "=" << limit_second;
      throw error;
    }

    // cerr << pairs[i] << endl;
  }
}

string Pulsar::ZapInterpreter::such (const string& args) try 
{
  vector<string> arguments = setup (args);

  vector< pair<unsigned,unsigned> > pairs;
  parse_pairs (pairs, arguments,
	       get()->get_nsubint(), "subint",
	       get()->get_nchan(), "chan");
  
  for (unsigned i=0; i<pairs.size(); i++)
    get()->get_Integration(pairs[i].first)->set_weight(pairs[i].second,0.0);
  
  return response (Good);
}
catch (Error& error) {
  return response (error);
}


// //////////////////////////////////////////////////////////////////////
//
string Pulsar::ZapInterpreter::edge (const string& args)
try {

  float fraction = setup<float> (args);

  if (fraction <= 0.0 || fraction >= 0.5)
    return response (Fail, "invalid fraction " + tostring(fraction));

  Archive* archive = get();

  if (zap_calibrator)
  {
    Reference::To<CalibratorExtension> ext = get()->get<CalibratorExtension>();
    if (!ext)
      return response (Fail, "archive does not contain CalibratorExtension");

    unsigned nchan = ext->get_nchan();
    unsigned nedge = unsigned( nchan * fraction );

    for (unsigned ichan=0; ichan<nedge; ichan++)
      ext->set_weight( ichan, 0.0 );
    for (unsigned ichan=nchan-nedge; ichan<nchan; ichan++)
      ext->set_weight( ichan, 0.0 );

    return response (Good);
  }

  unsigned isub,  nsub = archive->get_nsubint();
  unsigned ichan, nchan = archive->get_nchan();

  unsigned nedge = unsigned( nchan * fraction );

  for (isub=0; isub < nsub; isub++) {
    Integration* subint = archive->get_Integration (isub);
    for (ichan=0; ichan < nedge; ichan++)
      subint->set_weight (ichan, 0.0);
    for (ichan=nchan-nedge; ichan < nchan; ichan++)
      subint->set_weight (ichan, 0.0);
  }

  return response (Good);

}
catch (Error& error)
{
  return response (error);
}


// //////////////////////////////////////////////////////////////////////
//
string Pulsar::ZapInterpreter::cumulative (const string& args)
try {

  float fraction = setup<float> (args);

  if (fraction <= 0.0 || fraction >= 1.0)
    return response (Fail, "invalid fraction " + tostring(fraction));

  Archive* archive = get();

  Reference::To<Archive> total = archive->clone();
  total->tscrunch();
  total->pscrunch();

  unsigned nchan = total->get_nchan();
  vector<double> sum (nchan, 0.0);
  double cumu = 0;

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    sum[ichan] = total->get_Profile(0,0,ichan)->sum();
    cumu += sum[ichan];
  }

  double threshold = fraction * cumu;

  unsigned min_chan = 0;
  cumu = 0;

  while (cumu < threshold && min_chan < nchan)
  {
    cumu += sum[min_chan];
    min_chan ++;
  }

  unsigned max_chan = nchan - 1;
  cumu = 0;

  while (cumu < threshold && max_chan > 0)
  {
    cumu += sum[max_chan];
    max_chan --;
  }

  cerr << "ZapInterpreter::cumulative nchan=" << nchan 
       << " min_chan=" << min_chan << " max_chan=" << max_chan << endl;

  unsigned nsub = archive->get_nsubint();

  for (unsigned isub=0; isub < nsub; isub++) 
  {
    Integration* subint = archive->get_Integration (isub);
    for (unsigned ichan=0; ichan < min_chan; ichan++)
      subint->set_weight (ichan, 0.0);
    for (unsigned ichan=max_chan+1; ichan < nchan; ichan++)
      subint->set_weight (ichan, 0.0);
  }

  return response (Good);
}
catch (Error& error)
{
  return response (error);
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::ZapInterpreter::freq (const string& args) try
{
  vector<string> arguments = setup (args);

  if (zap_calibrator)
  {
    Reference::To<CalibratorExtension> ext = get()->get<CalibratorExtension>();
    if (!ext)
      return response (Fail, "archive does not contain CalibratorExtension");

    unsigned nchan = ext->get_nchan();
    
    for (unsigned iarg=0; iarg < arguments.size(); iarg++)
    {
      Range r = fromstring<Range> (arguments[iarg]);

      cerr << "iarg=" << iarg << " range=" << r << endl;

      for (unsigned ichan=0; ichan<nchan; ichan++)
	if (r.within( ext->get_centre_frequency(ichan) ))
            ext->set_weight (ichan, 0.0);
    }

    Reference::To<CalibratorStokes> cs = get()->get<CalibratorStokes>();
    if (!cs)
      return response (Good);

    if (nchan != cs->get_nchan())
      return response (Fail, 
             "CalibratorStokes nchan=" + tostring(cs->get_nchan()) +
             " != CalibratorExtension nchan=" + tostring(nchan));

    for (unsigned iarg=0; iarg < arguments.size(); iarg++)
    {
      Range r = fromstring<Range> (arguments[iarg]);

      for (unsigned ichan=0; ichan<nchan; ichan++)
        if (r.within( ext->get_centre_frequency(ichan) ))
          cs->set_valid (ichan, false);
    }

    return response (Good);
  }

  for (unsigned iarg=0; iarg < arguments.size(); iarg++)
  {
    Range r = fromstring<Range> (arguments[iarg]);

    Archive* archive = get();

    unsigned ichan, nchan = archive->get_nchan();

    if (set_subints.size()) {
      for (unsigned isub=0; isub < set_subints.size(); isub++)
      {
        Integration* subint = archive->get_Integration (set_subints[isub]);
        for (ichan=0; ichan < nchan; ichan++)
          if (r.within( subint->get_centre_frequency(ichan) ))
            subint->set_weight (ichan, 0.0);
      }
    }
    else
    {
      for (unsigned isub=0; isub < archive->get_nsubint(); isub++)
      {
        Integration* subint = archive->get_Integration (isub);
        for (ichan=0; ichan < nchan; ichan++)
          if (r.within( subint->get_centre_frequency(ichan) ))
            subint->set_weight (ichan, 0.0);
      }
    }
  }

  return response (Good);
}
catch (Error& error)
{
  return response (error);
}


string Pulsar::ZapInterpreter::zerodm (const string& args)
try {

  Archive* archive = get();

  unsigned isub,  nsub = archive->get_nsubint();
  unsigned ichan, nchan = archive->get_nchan();
  for( isub=0; isub<nsub;isub++){
	  unsigned nbins=archive->get_Profile(isub,0,0)->get_nbin();
	  float* mean=(float*)calloc(nbins,sizeof(float));
	  for( ichan=0; ichan < nchan;ichan++){
		  float* profile = archive->get_Profile(isub,0,ichan)->get_amps();
		  for ( unsigned ibin=0; ibin<nbins;ibin++)
		  {
			  mean[ibin]+=profile[ibin];
		  }
	  }
	  for ( unsigned ibin=0; ibin<nbins;ibin++)
	  {
		  mean[ibin]/=(float)nchan;
	  }

	  for( ichan=0; ichan<nchan;ichan++){
		  float* profile = archive->get_Profile(isub,0,ichan)->get_amps();
		  for ( unsigned ibin=0; ibin<nbins;ibin++)
		  {
			  profile[ibin] -= mean[ibin];
		  }
	  }
	  free(mean);
  }

  return response (Good);

}


catch (Error& error) {
  return response (error);
}
