/*
 * Copyright (c) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 */



#include "PavApp.h"



PavApp::PavApp()
{
  printing = false;
  ipol = 0;
  fsub = 0, lsub = 0;
  isubint = 0;
  ichan = 0;
  rot_phase = 0.0;
  svp = true;
  publn = false;
  axes = true;
  labels = true;
  n1 = 1;
  n2 = 1;
  min_freq = 0.0;
  max_freq = 0.0;
  min_phase = 0.0;
  max_phase = 1.0;
  reference_position_angle = 0.0;
  position_angle_height = 0.25;
  border = 0.1;
  dark = false;
  y_max = 1.0;
  y_min = 0.0;
  truncate_amp = 0.0;
  colour_map = pgplot::ColourMap::Heat;

  cbppo = false;
  cbpao = false;
  cblpo = false;
  cblao = false;

  ronsub = 0;
}



void PavApp::PrintUsage( void )
{
  cout << "Preprocessing options:" << endl;
  cout << " -b scr    Bscrunch scr phase bins together" << endl;
  cout << " -C        Centre the profiles on phase zero" << endl;
  cout << " -c        Don't remove baseline before plotting" << endl;
  cout << " -d        Dedisperse all channels" << endl;
  cout << " -r phase  Rotate all profiles by phase (in turns)" << endl;
  cout << " -f scr    Fscrunch scr frequency channels together" << endl;
  cout << " -F        Fscrunch all frequency channels" << endl;
  cout << " -t src    Tscrunch scr Integrations together" << endl;
  cout << " -T        Tscrunch all Integrations" << endl;
  cout << " -p        Add all polarisations together" << endl;
  cout << " -x clip   Truncate data at max*peak" << endl;
  cout << endl;
  cout << "Configuration options:" << endl;
  cout << " -K dev    Manually specify a plot device" << endl;
  cout << " -M meta   Read a meta-file containing the files to use" << endl;
  cout << endl;
  cout << "Selection options:" << endl;
  cout << " -z x1,x2  Zoom to this pulse phase range" << endl;
  cout << " -k f1,f2  Zoom to this frequency range" << endl;
  cout << " -y s1,s2  Zoom to this subint range" << endl;
  cout << " -N x,y    Divide the window into x by y panels" << endl;
  cout << " -H chan   Select which frequency channel to display" << endl;
  cout << " -P pol    Select which polarization to display" << endl;
  cout << " -I subint Select which sub-integration to display" << endl;
  cout << endl;
  cout << "Plotting options:" << endl;
  cout << " -D        Plot a single profile (chan 0, poln 0, subint 0 by default)" << endl;
  cout << " -G        Plot an image of amplitude against frequency & phase" << endl;
  cout << " -Y        Plot colour map of sub-integrations against pulse phase" << endl;
  cout << " -S        Plot polarization parameters (I,L,V,PA)" << endl;
  cout << " -X        Plot cal amplitude and phase vs frequency channel" << endl;
  cout << " -B        Display off-pulse bandpass & channel weights" << endl;
  cout << " -R        Plot stacked sub-integration profiles" << endl;
  cout << " -m        Plot Poincare vector in spherical coordinates" << endl;
  cout << " -n        Plot S/N against frequency" << endl;
  cout << " -j        Display a simple dynamic spectrum image" << endl;
  cout << endl;
  cout << "Other plotting options:" << endl;
  cout << "# --publn        Publication quality plot (B&W)  L dashed, V dots" << endl;
  cout << "# --publnc       Publication quality plot (colour)" << endl;
  cout << " --cmap index   Select a colour map for PGIMAG style plots" << endl;
  cout << "                The available indices are: (maybe 4-6 not needed)" << endl;
  cout << "                  0 -> Greyscale" << endl;
  cout << "                  1 -> Inverse Greyscale" << endl;
  cout << "                  2 -> Heat" << endl;
  cout << "                  3 -> Cold" << endl;
  cout << "                  4 -> Plasma" << endl;
  cout << "                  5 -> Forest" << endl;
  cout << "                  6 -> Alien Glow" << endl;
  cout << "                  7 -> Test" << endl;
  cout << endl;
  cout << " --plot_qu      Plot Stokes Q and Stokes U in '-S' option instead of degree of linear" << endl;
  cout << endl;
  cout << "Integration re-ordering (nsub = final # of subints) (used with pav Y for binary pulsars):" << endl;
  cout << "# --convert_binphsperi   nsub" << endl;
  cout << "# --convert_binphsasc    nsub" << endl;
  cout << "# --convert_binlngperi   nsub" << endl;
  cout << "# --convert_binlngasc    nsub" << endl;
  cout << endl;
  cout << "Utility options:" << endl;
  cout << " -a              Print available plugin information" << endl;
  cout << " -h              Display this useful help page" << endl;
  cout << " -i              Show revision information" << endl;
  cout << " -v              Verbose output" << endl;
  cout << " -V              Very verbose output" << endl;
  cout << "# -w              Use stopwatch (for benchmarking)" << endl;
  cout << endl << endl;
}


const void PavApp::d_minmaxval ( vector<double> thearray, double &min, double &max)
{
  min = thearray[0];
  max = thearray[0];

  int n = thearray.size();
  for(int i = 1; i < n; i++)
  {
    if(thearray[i] > max)
      max = thearray[i];
    if(thearray[i] < min)
      min = thearray[i];
  }
}


const void PavApp::minmaxval( vector<float> thearray, float &min, float &max )
{
  min = thearray[0];
  max = thearray[0];

  int n = thearray.size();
  for( int i = 1; i < n; i ++ )
  {
    if( thearray[i] > max )
      max = thearray[i];
    if( thearray[i] < min )
      min = thearray[i];
  }
}





//! Plot cal amplitude and phase as function of frequency channel
// void cal_plot (const Archive* data)
// {
//   Reference::To<Archive> copy = data->clone();
//
//   if (copy->get_state() != Signal::Coherence)
//   {
//     // convert back to original
//     copy->convert_state(Signal::Coherence);
// //   }
//
//   copy->tscrunch();
//
//   unsigned nchan = copy->get_nchan();
//   unsigned npol = copy->get_npol();
//
//   vector<vector<Estimate<double> > > calon;
//   vector<vector<Estimate<double> > > caloff;
//
//   copy->get_Integration(0)->cal_levels(calon, caloff);
//
//   // Form calon = calon - caloff
//   for( unsigned n=0; n<npol; n++)
//   {
//     for(unsigned i=0; i<nchan; i++)
//     {
//       calon[n][i] -= caloff[n][i];
//     }
//   }
//
//   vector<float> cal(nchan), sys(nchan), xx(nchan);
//   float mincal, maxcal, minsys, maxsys;
//
//   for(unsigned i=0; i<nchan; i++)
//   {
//     xx[i] = (float)i;
//   }
//
//   for(unsigned n=0; n<npol; n++)
//   {
//
//     for(unsigned i=0; i<nchan; i++)
//     {
//       if(n < 2)
//       {
//         cal[i] = (float)(calon[n][i].val);
//         sys[i] = (float)(caloff[n][i].val);
//       }
//       else
//       {
//         // Cal Cross
//         double x = calon[2][i].val;
//         double y = calon[3][i].val;
//         if(n == 2)
//         {
//           cal[i] = (float)(sqrt(x*x + y*y));
//         }
//         else
//         {
//           cal[i] = (float)(180.0/M_PI * atan2(y,x));
//         }
//         // System Cross
//         x = caloff[2][i].val;
//         y = caloff[3][i].val;
//         if(n == 2)
//         {
//           sys[i] = (float)(sqrt(x*x + y*y));
//         }
//         else
//         {
//           sys[i] = (float)(180.0/M_PI * atan2(y,x));
//         }
//       }
//     }
//
//     char cpol[4];
//
//     if(n < 3)
//     {
//       // Amplitudes
//       minmaxval ( cal, mincal, maxcal);
//       minmaxval ( sys, minsys, maxsys);
//
//       maxcal += 0.05 * (maxcal - mincal);
//       maxsys += 0.05 * (maxsys - minsys);
//       mincal -= 0.05 * (maxcal - mincal);
//       minsys -= 0.05 * (maxsys - minsys);
//
//       cpgsvp(0.1, 0.9, 0.1+(float)n/npol*0.8, 0.1+(float)(n+1)/npol*0.8);
//
//       // Plot cal
//       cpgswin(-1., nchan, mincal, maxcal);
//
//       if(n==0)
//       {
//         cpgbox("bcnst", 0., 0, "bcnst", 0., 0);
//         cpglab("Channel Number", "Cal Amp", " ");
//       }
//       else
//       {
//         cpgbox("bcst", 0., 0, "bcnst", 0., 0);
//         cpglab(" ", "Cal Amp", " ");
//       }
//       cpgsci(3);
//       cpgline(nchan, &xx[0], &cal[0]);
//       cpgsci(1);
//
//       // Plot sys
//       cpgswin(-1., nchan, minsys, maxsys);
//       cpgbox(" ", 0., 0, "mst",0., 0);
//       cpgmtxt("R", 3.0, 0.5, 0.5, "Sys Amp");
//       cpgsci(4);
//       cpgline(nchan, &xx[0], &sys[0]);
//
//       cpgsci(1);
//
//       if(n==0)strcpy(cpol,"AA");
//       if(n==1)strcpy(cpol,"BB");
//       if(n==2)strcpy(cpol,"AB");
//       cpgtext((float)(0.02*nchan), maxsys-0.15*(maxsys-minsys),cpol);
//
//     }
//     else
//     {
//       // Cross phase
//       cpgsvp(0.1, 0.9, 0.7, 0.9);
//
//       // Plot cal
//       cpgswin(-1., nchan, -185.0, 185.0);
//       cpgbox("bcst", 0., 0, "bcnst", 0., 0);
//       cpglab(" ", "Cal Phs", " ");
//       cpgbox(" ", 0., 0, "mst",0., 0);
//       cpgmtxt("R", 3.0, 0.5, 0.5, "Sys Phs");
//
//       cpgsci(5);
//       cpgline(nchan, &xx[0], &cal[0]);
//
//       // Plot sys
//       cpgsci(4);
//       cpgline(nchan, &xx[0], &sys[0]);
//
//       cpgsci(1);
//       cpgtext((float)(0.02*nchan), 130., cpol);
//     }
//   }
//
//   char info[512];
//   sprintf(info,"File: %s  Centre Freq: %.3f MHz, BW: %.3f MHz",
//           data->get_filename().c_str(), copy->get_centre_frequency(),
//           copy->get_bandwidth());
//   cpgmtxt("T", 1.0, 0.5, 0.5, info);
//
// }


//! Plot an off-pulse bandpass
void PavApp::bandpass (const Archive* data)
{
  Reference::To<Pulsar::Archive> copy = data->clone();

  // Define a vector to hold the bandpass
  vector<double> mean;
  mean.resize(copy->get_nchan());

  // Define a vector to hold the weights array
  vector<float> weights;
  weights.resize(copy->get_nchan());

  float phase = 0.0;
  Pulsar::Profile* profile = 0;


  for (unsigned i = 0; i < copy->get_nchan(); i++)
  {
    profile = copy->get_Integration(isubint)->get_Profile(ipol, i);
    phase = profile->find_min_phase();
    mean[i] = profile->mean(phase);
    weights[i] = profile->get_weight();
  }

  // Plot the bandpass

  double min=0.0, max=0.0;
  d_minmaxval ( mean, min, max );

  cpgsvp(0.1, 0.9, 0.4, 0.9);
  cpgswin (0,copy->get_nchan(),min-(min/20.0),max+(max/20));

  cpgslw(3);
  cpgsci(2);
  cpgsch(1.0);

  cpgmove(0, mean[0]);
  for (unsigned i = 1; i < copy->get_nchan(); i++)
  {
    cpgdraw(i, mean[i]);
  }

  cpgslw(1);
  cpgsci(1);

  float major = 5;
  if (copy->get_nchan() > 50)
    major = 10;
  if (copy->get_nchan() > 200)
    major = 50;
  if (copy->get_nchan() > 500)
    major = 100;

  cpgbox("BCINTS", major, 10, "BCINTS", 0.0, 0);

  string title = "Bandpass: " + data->get_filename();
  cpglab("Frequency Channel", "Amplitude (Arbitrary)", title.c_str());

  // Plot the weights array

  float fmin=0.0, fmax=0.0;
  minmaxval (weights, fmin, fmax);

  cpgsvp(0.1, 0.9, 0.05, 0.3);
  cpgswin (0,copy->get_nchan(),fmin-(fmin/20.0),fmax+(fmax/20));

  cpgsci(6);
  cpgsch(0.5);
  for (unsigned i = 1; i < copy->get_nchan(); i++)
  {
    cpgpt1(i, weights[i], 4);
  }

  cpgsci(1);
  cpgsch(1.0);
  cpgbox("", 0.0, 0, "NT", 0.0, 0);
  cpglab("", "Weight", "");
  cpgsch(1.0);
}



//! Plot profiles as a function of pulse phase and sub-integration.
void PavApp::line_phase_subints (const Pulsar::Archive* data)
{
  Reference::To<Pulsar::Archive> copy = data->clone();
  if (rot_phase != 0.0)
    copy->rotate(rot_phase * copy->get_Integration(copy->get_nsubint()/2)->
	get_folding_period());

  //   if (remove_baseline)
  //     copy->remove_baseline();

  unsigned first = 0;
  unsigned last  = 0;
  unsigned nsub  = 0;

  if (fsub == 0 && lsub == 0)
  {
    // No subint range specified, assume all
    nsub = copy->get_nsubint();
    last = nsub - 1;
  }
  else
  {
    nsub  = (lsub - fsub) + 1;
    first = fsub;
    last  = lsub;
  }

  unsigned nbin = copy->get_nbin();

  float* xaxis = new float[nbin];
  float* amps = new float[nbin*nsub];

  float minamp = 0.0;
  float maxamp = 0.0;
  unsigned count = 0;

  for (unsigned i = first; i <= last; i++)
  {
    vector<float> tempamps = copy->get_Integration(i)->
	get_Profile(ipol,ichan)->get_weighted_amps();

    for (unsigned j = 0; j < nbin; j++)
    {
      amps[count] = tempamps[j];

      if (amps[count] > maxamp)	maxamp = amps[count];
      if (amps[count] < minamp)	minamp = amps[count];
      count++;
    }
  }

  for (unsigned j = 0; j < nbin; j++)
    xaxis[j] = float(j)/float(nbin);

  float bias = maxamp / 3.0;

  if (svp)
    cpgsvp(0.1, 0.9, 0.1, 0.9);
  cpgswin(min_phase,max_phase,minamp-bias,bias*float(nsub)+maxamp);
  cpgslw(1);
  cpgsch(1.0);
  if(publn)
  {
    cpgsch(1.2);  /* Increase font size */
    cpgscf(2);    /* Roman font */
  }
  else
  {
    cpgsch(1.0);
    cpgscf(1);
  }

  cpgsci(7);
  count = 0;
  for (unsigned i = 0; i < nsub; i++)
  {
    for (unsigned j = 0; j < nbin; j++)
    {
      amps[count] += float(i)*bias;
      count++;
    }
    cpgline(nbin,xaxis,amps + (i*nbin));
  }

  cpgsci(1);
  cpgswin(min_phase,max_phase,first-1.+minamp/bias,last+1.+maxamp/bias);

  const Pulsar::IntegrationOrder* ptr = data->get<Pulsar::IntegrationOrder>();

  if( axes )
  {
    if (ptr && (first != last))
      cpgbox("BCINTS", 0.0, 0, "BINTS", 0.0, 0);
    else
      cpgbox("BCINTS", 0.0, 0, "BCINTS", 0.0, 0);
  }

  string str = data->get_source() + "  " + data->get_filename();

  if( labels )
  {
    if(publn)
      cpglab("Pulse Phase", "Sub Integrations", "");
    else
      cpglab("Pulse Phase", "Sub Integrations", str.c_str());
  }

  if (ptr && (first != last))
  {
    double val1 = ptr->get_Index(first);
    double val2 = ptr->get_Index(last);
    cpgswin(min_phase,max_phase,
	    val1-(1.-minamp/bias)*(val2-val1)/(last-first),
	    val2+(1.+maxamp/bias)*(val2-val1)/(last-first));
    cpgbox("", 0.0, 0, "CMIST", 0.0, 0);
    str = ptr->get_IndexState();
    if (!(ptr->get_Unit()).empty())
      str += " (" + (ptr->get_Unit()) + ")";
    cpgmtxt("R", 2.6, 0.5, 0.5, str.c_str());
  }
  else if (ptr && (first == last))
  {
    double val1 = ptr->get_Index(first);
    char* onept = new char[64];
    sprintf(onept, "%3.2lf", val1);
    //cpgmtxt("R", 1.3, 0.5, 0.5, onept);
    str = ptr->get_IndexState();
    if (!(ptr->get_Unit()).empty())
      str += " (" + (ptr->get_Unit()) + ")";
    cpgmtxt("R", 2.6, 0.5, 0.5, str.c_str());
  }

  delete[] xaxis;
  delete[] amps;
}



void PavApp::spherical ( const Profile* I,
		 const Profile* Q,
		 const Profile* U,
		 const Profile* V,
		 bool calibrated )
{
  const float* q = Q->get_amps();
  const float* u = U->get_amps();
  const float* v = V->get_amps();

  unsigned nbin = I->get_nbin();

  vector<float> phase (nbin);
  vector<float> orientation (nbin);
  vector<float> ellipticity (nbin);
  vector<float> polarization (nbin);
  vector<float> linear (nbin);

  double reference_orientation_turns = reference_position_angle / 180.0;

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {

    phase[ibin] = float(ibin)/float(nbin-1) - rot_phase;
    // ensure that phase runs from 0 to 1
    phase[ibin] -= floor (phase[ibin]);

    float Lsq = q[ibin]*q[ibin] + u[ibin]*u[ibin];
    float Psq = Lsq + v[ibin]*v[ibin];

    linear[ibin] = sqrt (Lsq);
    polarization[ibin] = sqrt (Psq);

    double orientation_turns = atan2 (u[ibin], q[ibin]) / (2.0*M_PI);
    orientation_turns -= reference_orientation_turns;
    orientation_turns -= floor (orientation_turns);

    orientation[ibin] = 180.0 * orientation_turns;

  }

  float baseline_phase = I->find_min_phase();
  Profile temp_profile;

  temp_profile.set_amps (linear);
  temp_profile -= temp_profile.mean (baseline_phase);
  temp_profile.get_amps (linear);

  temp_profile.set_amps (polarization);
  temp_profile -= temp_profile.mean (baseline_phase);

  for (unsigned ibin=0; ibin<nbin; ibin++)
    ellipticity[ibin] = 90.0/M_PI * atan2 (v[ibin], linear[ibin]);

  float x1, x2, y1, y2;
  cpgsci(1);
  cpgqvp (0, &x1, &x2, &y1, &y2);

  float height1 = position_angle_height*(y2-y1);

  // //////////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////////
  // plot the orientation
  cpgsvp (x1, x2, y2-height1, y2);

  cpgswin (min_phase, max_phase, 0.001, 179.999);
  cpgsch(1.2);
  cpgbox("bcst",0,0,"bcnvst",60,3);

  if (labels)
  {
    cpgsch(1.5);
    cpgmtxt("L",2.5,.5,.5,"\\gh (deg.)");
  }

  cpgpt (nbin, &phase[0], &orientation[0], 1);

  // //////////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////////
  // plot the ellipticity
  cpgsvp (x1, x2, y2-height1*2.0, y2-height1);

  cpgswin (min_phase, max_phase, -44.999, 44.999);
  cpgsch(1.2);
  cpgbox("bcst",0,0,"bcnvst",30,3);

  if (labels)
  {
    cpgsch(1.5);
    cpgmtxt("L",2.5,.5,.5,"\\ge (deg.)");
  }

  cpgpt (nbin, &phase[0], &ellipticity[0], 1);

  bool logarithmic = true;

  Reference::To<Profile> tempI;

  if (logarithmic)
  {

    double variance;
    I->stats (baseline_phase, 0, &variance);

    float rms = sqrt(variance);
    float log_rms = log(rms)/log(10.0);

    tempI = new Profile (*I);
    tempI->logarithm (10.0, rms);
    *tempI -= log_rms;
    I = tempI;

    temp_profile.logarithm (10.0, rms);
    temp_profile -= log_rms;

  }

  // //////////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////////
  // set the viewport to plot the rest
  cpgsvp (x1, x2, y1, y2-height1*2.0);

  float ymin = 0.0;
  float ymax = I->max() * y_max;
  float ybuf = border * (ymax-ymin);

  cpgswin (min_phase, max_phase, ymin, ymax+ybuf);


  // //////////////////////////////////////////////////////////////////////
  // plot total intensity
  if (dark)
  {
    // white, solid line
    cpgsci(1);
    cpgsls(1);
  }
  else
  {
    // black, dotted line
    cpgsci(1);
    cpgsls(1);
  }

  I->get_amps (linear);
  cpgpt (nbin, &phase[0], &linear[0], 1);


  // I->draw (rot_phase);


  // //////////////////////////////////////////////////////////////////////
  // plot total polarization
  if (dark)
  {
    // red, solid line
    cpgsci(2);
    cpgsls(1);
  }
  else
  {
    // red, dashed line
    cpgsci(2);
    cpgsls(2);
  }

  temp_profile.get_amps (linear);
  cpgpt (nbin, &phase[0], &linear[0], 1);


  // //////////////////////////////////////////////////////////////////////
  // plot some labels

  cpgsci(1);
  cpgsls(1);

  cpgsch(1.2);
  cpgbox("bcnst",0.0,0,"bcnlstv",0.0,0);

  cpgsch(1.5);
  if (labels)
  {
    cpgmtxt("L",2.5,.5,.5,"Flux (\\fi\\gs\\fr\\d0\\u)");
    cpgmtxt("B",2.5,.5,.5,"Pulse Phase");
  }

  cpgsch(1);

  // restore the viewport
  cpgsvp (x1, x2, y1, y2);

}




void PavApp::spherical_wrapper (const Archive* data)
{
  Reference::To<Archive> clone = data->clone();

  // convert to Stokes parameters
  clone->convert_state (Signal::Stokes);
  // remove the baseline
  clone->remove_baseline();

  data = clone;

  const Profile* I = data->get_Profile (isubint, 0, ichan);
  const Profile* Q = data->get_Profile (isubint, 1, ichan);
  const Profile* U = data->get_Profile (isubint, 2, ichan);
  const Profile* V = data->get_Profile (isubint, 3, ichan);

  spherical (I,Q,U,V,data->get_scale() == Signal::Jansky);
}




void PavApp::SetStokesPlotToQU( vector< Reference::To<Plot> > &plots )
{
  vector< Reference::To<Plot> >::iterator it;
  for( it = plots.begin(); it != plots.end(); it ++ )
  {
    Reference::To<StokesCylindrical> sp = dynamic_cast<StokesCylindrical*>( (*it).get() );
    if( sp )
    {
      Reference::To<TextInterface::Class> ti_base = sp->get_interface();
      Reference::To<StokesCylindrical::Interface> ti = dynamic_cast<StokesCylindrical::Interface*>( ti_base.get() );

      if( ti )
      {
	ti->process( "flux:val=IQUV" );
	ti->process( "flux:ci=1234" );
	if( printing )
	  ti->process( "flux:ls=1234" );
	else
	  ti->process( "flux:ls=1111" );
      }
    }
  }
}



void PavApp::SetPhaseZoom( double min_phase, double max_phase, vector< Reference::To<Plot> > &plots )
{
  string range_cmd = string("x:range=(") +
      tostring<double>( min_phase ) +
      string(",") +
      tostring<double>( max_phase ) +
      string( ")");

  vector< Reference::To<Plot> >::iterator it;
  for( it = plots.begin(); it != plots.end(); it ++ )
  {
    Reference::To<TextInterface::Class> ti = (*it)->get_frame_interface();

    try
    {
      ti->process( range_cmd );
    }
    catch( Error e )
    {
      // assume the user knows what they are doing and tell every plot to zoom
      // if x:range didn't exist or any other error occured, just tell the user
      // probably just using -z with wrong plot type
      cerr << "Failed to set zoom, is this a X vs phase plot?" << endl;
    }
  }
}



void PavApp::SetFreqZoom( double min_freq, double max_freq, vector< Reference::To<Plot> > &plots, Reference::To<Archive> archive )
{
  cerr << "zooming to " << min_freq << ", " << max_freq << endl;

  // get the centre frequency and bandwidth

  double ctr_freq = archive->get_centre_frequency();
  double bw = archive->get_bandwidth();
  double bw_abs = bw;

  if( bw < 0 )
  {
    bw_abs = 0 - bw;
  }

  cerr << "ctr_freq is " << ctr_freq << endl;
  cerr << "bw is " << bw << endl;

  double actual_min = ctr_freq - bw_abs / 2.0;

  cerr << "actual min is " << actual_min << endl;

  // convert the min and max freq values to (0-1)

  min_freq -= actual_min;
  max_freq -= actual_min;

  cerr << "range before / is  " << min_freq << " - " << max_freq << endl;

  min_freq /= bw_abs;
  max_freq /= bw_abs;

  if( bw < 0 )
  {
    double tmp = 1 - min_freq;
    min_freq = 1 - max_freq;
    max_freq = tmp;
  }

  cerr << "adjusted range is " << min_freq << " - " << max_freq << endl;

  // construct the command to set the range, the same for all plots (for current plots)

  string range_cmd = "y:range=(";
  range_cmd += tostring<double>( min_freq );
  range_cmd += string(",");
  range_cmd += tostring<double>( max_freq );
  range_cmd += string(")" );

  cerr << "frequency range command is " << range_cmd << endl;

  vector< Reference::To<Plot> >::iterator it;
  for( it = plots.begin(); it != plots.end(); it ++ )
  {
    Reference::To<TextInterface::Class> ti = (*it)->get_frame_interface();

    try
    {
      ti->process( range_cmd );
    }
    catch( Error e )
    {
      // assume the user knows what they are doing and tell every plot to zoom
      // if x:rage didn't exist or any other error occured, just tell the user
      // probably just using -z with wrong plot type
      cerr << "Failed to set zoom, is this a freq vs X plot?" << endl;
    }
  }
}



bool PavApp::device_is_printer( string devname )
{
  // if the device name contains ps
  //   but doesn't contain cvps
  //     then its a printer

  devname = uppercase( devname );

  vector<string> printer_names;

  printer_names.push_back( "/CANON" );
  printer_names.push_back( "/CW6320" );
  printer_names.push_back( "/HPGL" );
  printer_names.push_back( "/VHPGL" );
  printer_names.push_back( "/HP7221" );
  printer_names.push_back( "/LIPS2" );
  printer_names.push_back( "/VLIPS2" );
  printer_names.push_back( "/LATEX" );
  printer_names.push_back( "/PS" );
  printer_names.push_back( "/VPS" );


  vector<string>::iterator it;
  for( it = printer_names.begin(); it != printer_names.end(); it ++ )
  {
    if( strstr( devname.c_str(), (*it).c_str() ) != NULL )
    {
      return true;
    }
  }

  return false;
}



int PavApp::run( int argc, char *argv[] )
{
  vector< string > filenames;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // PGPLOT device name
  string plot_device = "/xs";

  // Plot classes to be used
  vector< Reference::To< Plot > > plots;

  // Options to be set
  vector<string> options;

  // Preprocessing jobs
  vector<string> jobs;

  // Overlay plots (do not clear page between different plot types)
  bool overlay = false;

  // verbosity
  bool verbose = false;

  // Available plots
  static PlotFactory factory;

  // Keep the baseline before plotting
  bool keep_baseline = false;

  bool plot_bandpass = false;

  bool plot_line_phase_subints = false;

  bool plot_spherical = false;

  bool plot_snr_spectrum = false;

  bool plot_qu = false;

  bool perform_test = false;

  int option_index;

  const int PLOT_QU          = 1001;
  const int CMAP_IND         = 1002;
  const int BINPHSP          = 1003;
  const int BINPHSA          = 1004;
  const int BINPHLP          = 1005;
  const int BINPHLA          = 1006;
  const int PUBLN            = 1007;

  static struct option long_options[] =
  {
    { "convert_binphsperi", 1, 0, BINPHSP },
    { "convert_binphsasc",  1, 0, BINPHSA },
    { "convert_binlngperi", 1, 0, BINPHLP },
    { "convert_binlngasc",  1, 0, BINPHLA },
    { "plot_qu",            0, 0, PLOT_QU },
    { "cmap",               1, 0, CMAP_IND },
    { "publn",              0, 0, PUBLN },
    { 0,0,0,0 }
  };


  char valid_args[] = "z:hb:M:K:DCcdr:f:Ft:TGYSXBRmnjpP:y:H:I:N:k:ivVax";

  int c = '\0';
  while( (c = getopt_long( argc, argv, valid_args, long_options, &option_index )) != -1 )
  {
    switch( c )
    {
      case 'a':
	Pulsar::Archive::agent_report ();
	return 0;
      case 'h':
	PrintUsage();
	break;
      case 'b':
      {
	jobs.push_back( "bscrunch" );
	break;
      }
      case 'i':
	cout << "pav VERSION $Id: PavApp.C,v 1.3 2007/09/06 01:24:12 nopeer Exp $" << endl << endl;
	return 0;
	break;
      case 'M':
	metafile = optarg;
	cout << "loading filenames from meta file " << metafile << endl;
	break;
      case 'K':
	plot_device = optarg;
	break;
      case 'D':
	plots.push_back( factory.construct( "flux" ) );
	break;
      case 'C':
	jobs.push_back( "centre" );
	break;
      case 'c':
	keep_baseline = true;
	break;
      case 'd':
	jobs.push_back( "dedisperse" );
	break;
      case 'V':
	Pulsar::Archive::set_verbosity (3);
	break;
      case 'v':
	Pulsar::Archive::set_verbosity (2);
	break;
      case 'r':
      {
	ostringstream s_job;
	s_job << "rotate " << optarg;
	jobs.push_back( s_job.str() );
	break;
      }
      case 'f':
      {
	ostringstream s_job;
	s_job << "fscrunch " << optarg;
	jobs.push_back( s_job.str() );
	break;
      }
      case 'F':
	jobs.push_back( "fscrunch" );
	break;
      case 't':
      {
	ostringstream s_job;
	s_job << "tscrunch " << optarg;
	jobs.push_back( s_job.str() );
	break;
      }
      case 'T':
	jobs.push_back( "tscrunch" );
	break;
      case 'p':
	jobs.push_back( "pscrunch" );
	break;
      case 'G':
	plots.push_back( factory.construct( "freq" ) );
	break;
      case 'Y':
      // keep_baseline = true;
	plots.push_back( factory.construct( "time" ) );
	break;
      case 'S':
	plots.push_back( factory.construct( "Scyl" ) );
	break;
      case 'X':
	keep_baseline = true;
	plots.push_back( factory.construct( "calphvf" ) );
	break;
      case 'B':
	plot_bandpass = true;
	keep_baseline = true;
	break;
      case 'R':
	keep_baseline = true;
	plots.push_back( factory.construct( "line" ) );
	break;
      case 'm':
	plot_spherical = true;
	break;
      case 'n':
	plots.push_back( factory.construct("snrspec") );
	keep_baseline = true;
	break;
      case 'j':
	plots.push_back( factory.construct( "dynsnrspec" ) );
	keep_baseline = true;
	break;
      case 'P':
	ipol = fromstring<int>( optarg );
	options.push_back( string("pol=") + string(optarg) );
	break;
      case 'y':
      {
	string s1,s2;
	string_split( optarg, s1, s2, "," );
	fsub = fromstring<unsigned int>( s1 );
	lsub = fromstring<unsigned int>( s2 );
      }
      break;
      case 'H':
	ichan = fromstring<int>( optarg );
	options.push_back( string("chan=") + string( optarg ) );
	break;
      case 'I':
	isubint = fromstring<unsigned int>( optarg );
	options.push_back( string("subint=" ) + string( optarg ) );
	break;
      case 'N':
      {
	string s1,s2;
	string_split( optarg, s1, s2, "," );
	n1 = fromstring<unsigned int>( s1 );
	n2 = fromstring<unsigned int>( s2 );
        //overlay = true;
      }
      break;
      case 'k':
      {
	string s1, s2;
	string_split( optarg, s1, s2, "," );
	min_freq = fromstring<double>( s1 );
	max_freq = fromstring<double>( s2 );
      }
      break;
      case 'z':
      {
	string s1, s2;
	string_split( optarg, s1, s2, "," );
	min_phase = fromstring<double>( s1 );
	max_phase = fromstring<double>( s2 );
      }
      break;
      case PLOT_QU:
	plot_qu = true;
	break;
      case CMAP_IND:
	switch( fromstring< unsigned int>( optarg ) )
	{
	  case 0: colour_map = pgplot::ColourMap::GreyScale; break;
	  case 1: colour_map = pgplot::ColourMap::Inverse; break;
	  case 2: colour_map = pgplot::ColourMap::Heat; break;
	  case 3: colour_map = pgplot::ColourMap::Cold; break;
	  case 4: colour_map = pgplot::ColourMap::Plasma; break;
	  case 5: colour_map = pgplot::ColourMap::Forest; break;
	  case 6: colour_map = pgplot::ColourMap::AlienGlow; break;
	  case 7: colour_map = pgplot::ColourMap::Test; break;
	  default: cerr << "Unknown colour map, use (0-7)" << endl;
	};
	break;
      case BINPHLA:
	cbpao = true;
	ronsub = fromstring<unsigned int>( optarg );
	break;
      case BINPHLP:
	cbppo = true;
	ronsub = fromstring<unsigned int>( optarg );
	break;
      case BINPHSA:
	cblao = true;
	ronsub = fromstring<unsigned int>( optarg );
	break;
      case BINPHSP:
	cblpo = true;
	ronsub = fromstring<unsigned int>( optarg );
	break;
      case PUBLN:
	publn = true;
	options.push_back( "set=pub" );
	break;
      case 'x':
	truncate_amp = fromstring<float>( optarg );
	break;
    };
  }

  if (plots.empty() && !plot_bandpass && !plot_spherical && !plot_snr_spectrum
      && !perform_test )
  {
    cout << "pav: please choose at least one plot style" << endl;
    return -1;
  }

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty())
  {
    cout << "pav: please specify filename[s]" << endl;
    return -1;
  }

  if( device_is_printer( plot_device ) )
  {
    printing = true;
  }

  if (cpgopen(plot_device.c_str()) < 0)
  {
    cout << "pav: Could not open plot device" << endl;
    return -1;
  }

  Interpreter preprocessor;

  if (n1 > 1 || n2 > 1)
    cpgsubp(n1,n2);

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try
  {

    Reference::To<Archive> archive;
    archive = Archive::load( filenames[ifile] );



    if (jobs.size())
    {
      if (verbose)
	cerr << "pav: preprocessing " << filenames[ifile] << endl;
      preprocessor.set(archive);
      preprocessor.script(jobs);
    }

      // set options for all the plots
    if( plots.size() )
    {
      vector< Reference::To<Plot> >::iterator it;
      for( it = plots.begin(); it != plots.end(); it ++ )
      {
	vector<string>::iterator oit;
	for( oit = options.begin(); oit != options.end(); oit ++ )
	{
	  (*it)->configure( (*oit) );
	}
      }
    }

    if( !keep_baseline )
      archive->remove_baseline();

    if (verbose)
      cerr << "pav: plotting " << filenames[ifile] << endl;

      //       if (!overlay)
      //         cpgpage();



      // if we are plotting qu with -S, go through each of the plots and find the stokes plot
      // and set it to plot QU
    if( plot_qu )
    {
      SetStokesPlotToQU( plots );
    }


    if( printing && !plot_qu )
    {
      vector< Reference::To<Plot> >::iterator it;
      for( it = plots.begin(); it != plots.end(); it ++ )
      {
	Plot *ptr = (*it).get();
	Reference::To<StokesCylindrical> p = dynamic_cast<StokesCylindrical*>( ptr );
	if( p )
	{
	  //p->set_printing( true );
	}
      }
    }

    SetPhaseZoom( min_phase, max_phase, plots );

    if( min_freq != max_freq )
    {
      SetFreqZoom( min_freq, max_freq, plots, archive );
    }


    pgplot::ColourMap cmap;
    cmap.set_name ( colour_map );
    cmap.apply();



    if (cbppo)
    {
      Pulsar::IntegrationOrder* myio = new Pulsar::PeriastronOrder();
      archive->add_extension(myio);
      myio->organise(archive, ronsub);
    }

    if (cbpao)
    {
      Pulsar::IntegrationOrder* myio = new Pulsar::BinaryPhaseOrder();
      archive->add_extension(myio);
      myio->organise(archive, ronsub);
    }

    if (cblpo)
    {
      Pulsar::IntegrationOrder* myio = new Pulsar::BinLngPeriOrder();
      archive->add_extension(myio);
      myio->organise(archive, ronsub);
    }

    if (cblao)
    {
      Pulsar::IntegrationOrder* myio = new Pulsar::BinLngAscOrder();
      archive->add_extension(myio);
      myio->organise(archive, ronsub);
    }


    if( plot_bandpass )
    {
      bandpass( archive );
    }
    else if ( plot_spherical )
    {
      spherical_wrapper( archive );
    }
    else
    {
      for (unsigned iplot=0; iplot < plots.size(); iplot++)
      {
	if (!overlay)
	  cpgpage ();
	plots[iplot]->plot (archive);
      }
    }
  }
  catch( Error e )
  {
    cerr << "Exception occured " << e << endl;
  }

  cpgclos();


  return 0;
}



