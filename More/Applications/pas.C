//
// $Id: pas.C, v 1.00 2003/05/05 nwang 
//
// The Pulsar Archive Standard profile
//
// A program for aligning standard profiles in Pulsar::Archive objects
//

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cpgplot.h>
#include <math.h>
#include "spectra.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Integration.h"
#include "Error.h"

#include "dirutil.h"
#include "string_utils.h"

void plot_it(Reference::To<Pulsar::Archive>, Reference::To<Pulsar::Profile>, Reference::To<Pulsar::Archive>, Reference::To<Pulsar::Profile>, int, int, int, char *, char *);
void coef (float *, float *, int, int, double *, int *, double *, bool);
void cross_correlation(Reference::To<Pulsar::Profile>, Reference::To<Pulsar::Profile>, double *, int *, double *, bool, char *);
void smooth (Reference::To<Pulsar::Profile>, bool);


void usage ()
{
  cout << "program to align standard profiles \n"
    "Usage: pas [-r reference profile] [-s standard profile] [-v] [-V] \n"
    "Where the options are as follows \n"
    " -h        This help page \n"
    " -i        Revision information\n"
    " -r        Reference profile \n"
    " -s        Standard profile to align with reference profile \n"
    " -v        Verbose output \n"
    " -V        Very verbose output \n"
       << endl;
}


int main (int argc, char** argv) 
{
  char* refname = NULL;
  char* stdname = NULL;
  char line[100];
  char plotdev[5];
  char opts;
  float curs_x, curs_y, x, y, meantmp =0;
  unsigned int i;
  bool verbose = false;
  bool vverbose = false;
  int c = 0;
  int imax=0;
  bool zflag =false;
  double rmax=0, pcoef=0;
  //color of lines
  int ci_ref = 1;  //white  
  int ci_std = 2;  //red
  int ci_tex = 5;   //grey

  const char* args = "hir:s:vV";

  while ((c = getopt(argc, argv, args)) != -1) {
    switch (c) {
      
    case 'h':
      usage ();
      return 0;

    case 'i':
      cout << "$Id: pas.C,v 1.2 2003/05/27 01:04:03 pulsar Exp $" << endl;
      return 0;

    case 'r':
      refname = optarg;
      break;

    case 's':
      stdname = optarg;
      break;

    case 'v':
      verbose = true;
      Error::verbose = true;
      break;

    case 'V':
      vverbose = true;
      verbose = true;
      Error::verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;

    default:
      cerr << "invalid param '" << c << "'" << endl;
    }
  }
  

  Error::handle_signals ();

  cout << "  Input plot device (/xs, /ps, /vps, /gif): ";
  cin >> plotdev;
  //  cpgbeg (0, "?", 0, 0);
  //  cpgask (1);

  //original archive
  Reference::To<Pulsar::Archive> refarch0 = Pulsar::Archive::load(refname);
  Reference::To<Pulsar::Archive> stdarch0 = Pulsar::Archive::load(stdname);
  if(verbose) cout << "archives loaded" << endl;
  //used in the processing
  Reference::To<Pulsar::Archive> refarch (refarch0->clone());
  Reference::To<Pulsar::Archive> stdarch (stdarch0->clone());
  //remember last change
  Reference::To<Pulsar::Archive> stdarch1 (stdarch0->clone());

  //original profile
  Reference::To<Pulsar::Profile> refprof0 (refarch0 -> get_Profile (0, 0, 0));
  Reference::To<Pulsar::Profile> stdprof0 (stdarch0 -> get_Profile (0, 0, 0));
  //used in the processing
  Reference::To<Pulsar::Profile> refprof (refprof0->clone());
  Reference::To<Pulsar::Profile> stdprof (stdprof0->clone());
  Reference::To<Pulsar::Profile> refcorr (refprof0->clone());
  Reference::To<Pulsar::Profile> stdcorr (stdprof0->clone());

  //check reference profile
  if( verbose ){
    cout << "Reference profile: " << refarch0->get_filename() << endl;
    cout <<"  Profile has " << refprof->get_nbin() << " bins" << endl;
  }
  if( !power_of_two(refprof->get_nbin()) )
    throw Error(InvalidState,"Check: ",
		"profile from archive '%s' doesn't have nbin a power of two (%d)", 
		refarch->get_filename().c_str(),refprof->get_nbin());
  
  //check new profile
  if( verbose ) {
    cout << "Processing profile:" << stdarch0->get_filename() << endl;
    cout <<"  Profile has " << stdprof->get_nbin() << " bins" << endl;
  }
  if( !power_of_two(stdprof->get_nbin()) )
    throw Error(InvalidState,"Check: ",
		"profile from archive '%s' doesn't have nbin a power of two (%d)", 
		stdarch->get_filename().c_str(),stdprof->get_nbin());

  cross_correlation(refcorr, stdcorr, &rmax, &imax, &pcoef, verbose, line);
  //plot profiles
  plot_it(refarch, refprof, stdarch, stdprof, ci_ref, ci_std, ci_tex, line, plotdev);

  opts = ' ';
  if (cpgcurs(&curs_x, &curs_y, &opts) == 1) {
    while (opts != 'e') {  
      try {
	switch(opts) {
	case 'a':   //Align
	    stdprof->rotate(imax/(1.0*stdprof->get_nbin()));
	    if(verbose) cout << "Align: rotated " <<imax <<" bins"<<endl;
	    break;

	case 'b':  //Bscrunch
	  stdarch->bscrunch(2);
	  stdprof = stdarch -> get_Profile (0, 0, 0);
	  if( verbose ) cout << "Bscrunch: Profile is smoothed by factor 2" << endl;
	  break;
	 
	case 'c':  //Centre Profile
	  stdarch->centre();
	  stdprof = stdarch -> get_Profile (0, 0, 0);
	  if( verbose ) cout << "Centre the profile" <<endl;
	  break;

	case 'e':  //Exit

	case 'f': //Fast rotate, to the right
	  stdprof->rotate (-20.0/stdprof->get_nbin());
	  if( verbose ) cout << "Fast Rotate profile: 20 bins to the right" <<endl;
	  break;
	  
	case 'l':  //Left rotate
	  stdprof->rotate (1.0/stdprof->get_nbin());
	  if( verbose ) cout << "Rotate profile: 1 bin to the left" << endl;
	  break;
	
	case 'm':   //smooth the profile
	  smooth(stdprof, vverbose);
	  if( verbose ) cout << "Smooth: profile smoothed" << endl;
	  break;

	case 'o':  //Back to Origin
	  //	  stdarch -> refresh();	
	  *stdprof = *stdprof0;
	  if( verbose ) cout << "Back to origin" << endl;
	  break;
	  
	case 'q':  //Rotate quarterly
	  stdprof->rotate(-0.25); //right rotate
	  if( verbose ) cout << "Center: Profile is centered" << endl;
	  break;
	
	case 'r':  //Right rotate
	  stdprof->rotate (-1.0/stdprof->get_nbin());
	  if( verbose ) cout << "Rotate profile: 1 bin to the right" << endl;
	  break;
	   
	case 'z':   //Zero base line
	  //stdarch -> remove_baseline();
	  cout << "Zero baseline: start point set at:" << curs_x << endl;
	  cout << "Zero baseline: move cursor and set the range" <<endl;
	  opts = ' ';
	  cpgband(2, 0, curs_x, curs_y, &x, &y, &opts);
	  if (opts == 'q' || opts == 'Q') {
	    cout << "Zero baseline: quit from setting base line zero" << endl; 
	  }
	  else if (opts == 'z') {
	    cout << "Zero baseline: end point set at:" << x << endl;
	    if(verbose) cout << "Zero baseline: zero the profile base line" << endl;
	    float * tmpdata=stdprof->get_amps();
	    int istart, iend;
	    if (curs_x > x) {float tmp=x; x=curs_x; curs_x=tmp;}
	    istart = int(curs_x*stdprof->get_nbin());
	    if (istart<0) istart=0;
	    fprintf(stderr, "istart = %d\n", istart);
	    iend = int(x*stdprof->get_nbin());
	    if(unsigned(iend)>stdprof->get_nbin()) iend=stdprof->get_nbin();
	    if(verbose) cout << "Sero baseline: start & end bin number: " <<istart << ", " <<iend<<endl;
	    if(zflag==false) {
	      meantmp=0;
	      for(i=unsigned(istart); i<unsigned(iend); i++) {
		meantmp+=tmpdata[i];
	      }
	      meantmp/=(iend-istart); 
	      zflag=true;
	    }
	    
	    for(i=unsigned(istart); i<unsigned(iend); i++) 
	      tmpdata[i]=meantmp;

	    if (vverbose) 
	      for(i=0; i<stdprof->get_nbin(); i++) 
		cout << i << ": " << tmpdata[i]<<"  ";
	      
	    stdprof->set_amps(tmpdata);
	    if (verbose) cout << "Zero baseline: done" << endl;
	  }
	  else cout << "Zero baseline: range not set" << endl;
	  break;
	  
	case 's':  //Save
	  string the_old = stdarch->get_filename().c_str();
	  if(verbose) cout << "Save: the archive file is " << the_old <<endl;
	  string the_new = " ";
	  cout << "Save: the center freq is " << stdarch->get_centre_frequency() << "MHz, "<< (30/(1e-3* stdarch -> get_centre_frequency()))<< "cm" <<endl;
	  cout << "      new file name ('x' for default name): ";
	  cin >> the_new;
	  if( the_new == "x" ) {
	    int index = the_old.find_last_of(".",the_old.length());
	    string primary = the_old.substr(0, index);
	    the_new = primary + "." + "new";
	    cout << "      default file name " << the_new << endl;
	  }
	  //	  *stdarch0=*stdarch;
	  //	  *stdprof0=*stdprof;
	  stdarch->unload(the_new);
	  cout << "Save: new standard profile " << the_new << " written to disk" << endl;
	  break;	 
	}

	*refcorr=*refprof;
	*stdcorr=*stdprof;
	cross_correlation(refcorr, stdcorr, &rmax, &imax, &pcoef, verbose, line);
	plot_it(refarch, refprof, stdarch, stdprof, ci_ref, ci_std, ci_tex, line, plotdev);
	cout << "Wait for option" << endl;
	cpgcurs(&curs_x, &curs_y, &opts);
      }
 
      catch (Error& error) {
	cerr << error << endl;
      } 
      catch (...) {
	cerr << "unknown exception thrown." << endl;
      }
    }
  }
  else fprintf(stderr, "Sorry, plot device has no cursor.");
  cpgend();
  return 0;
}


void plot_it(Reference::To<Pulsar::Archive> refarch, Reference::To<Pulsar::Profile> refprof, Reference::To<Pulsar::Archive> stdarch, Reference::To<Pulsar::Profile> stdprof, int ci_ref, int ci_std, int ci_tex, char line[100], char plotdev[5]) {
  float x, y;
  char str[50];
  unsigned i;
  //options
  cpgbeg(0, plotdev, 1, 1);
  // cpgeras();
  
  cpgsvp (0.05, .95, 0.75, .98);
  cpgswin (0, 100, 0, 100);
  cpgsch (0.8);
  cpgsci (ci_tex);
  int step=14;
  for(i=0; i<10; i++) {
    x=5;
    y=100-i*step;
    if(i==0)
      cpgtext (x, y, "a:  Align with reference profile");
    else if(i==1)
      cpgtext (x, y, "b:  Bscrunch by factor 2");
    else if(i==2)
      cpgtext (x, y, "c:  Center the profile");
    else if(i==3)
      cpgtext (x, y, "e:  Exit the program");
    else if(i==4)
      cpgtext (x, y, "f:  Fast rotate profile: 20 bins to the right");
    else if(i==5)
      cpgtext (x, y, "l:  Left rotate profile, step: 1 bin");
    else    break;
  }
  cpgsci (ci_ref);
  cpgmove (x, 15);
  cpgdraw (x+10, 15);
  strncpy(str, "", 50);
  sprintf(str, "Reference profile    %d bins", refprof->get_nbin());
  cpgtext (x+13, 15, str);

  cpgsci(ci_tex);
  for(i=0; i<10; i++) {
    x=60;
    y=100-i*step;
    if(i==0)
      cpgtext (x, y, "m: sMooth the profile");
    else if(i==1)
      cpgtext (x, y, "o:  back to Origin");
    else if(i==2)
      cpgtext (x, y, "q:  rotate Quarterly");
    else if(i==3)
      cpgtext (x, y, "r:  Right rotate profile, step: 1 bin");
    else if(i==4)
      cpgtext (x, y, "s:  Save the standard profile");
    else if(i==5)
      cpgtext (x, y, "z:  Zero the base line range, 'q' to quit"); 
    else  break;
  }
  cpgsci (ci_std);
  cpgmove (x, 15);
  cpgdraw (x+10, 15);
  strncpy(str, "", 50);
  sprintf(str, "New profile    %d bins", stdprof->get_nbin());
  cpgtext (x+13, 15, str);

  //write the cross correlation function
  cpgsci (5);
  cpgtext (10, 0, line);

  //profiles
  cpgsvp (0.1, 0.9, 0.05, 0.7);
  cpgswin (0, 1, 0, 1);
  cpgsci (ci_ref);
  //refarch->get_Profile(0,0,0)->display();
  refprof->Pulsar::Profile::display (0, 0, 1, 0, 1, 1.0);
  cpgsci (ci_std);
  //stdarch->get_Profile(0,0,0)->display();
  stdprof->Pulsar::Profile::display (0, 0, 1, 0, 1, 1.0);
}


void coef (float *x, float *y, int maxdelay, int n, double *rmax, int *imax, double *pcoef, bool verbose) 
{
  float mx, my, sx, sy, denom, r, sxy;
  int i, delay, j;
  
  *rmax=-1e8;
  mx = 0;
  my = 0;   
  for (i=0;i<n;i++) {
    mx += x[i];
    my += y[i];
  }
  mx /= n;
  my /= n;
  
  /* Calculate the denominator */
  sx = 0;
  sy = 0;
  for (i=0;i<n;i++) {
    sx += (x[i] - mx) * (x[i] - mx);
    sy += (y[i] - my) * (y[i] - my);
  }
  denom = sqrt(sx*sy);
  
  /* Calculate the correlation coefficiency on lag 'delay' */
  for (delay=-maxdelay;delay<maxdelay;delay++) {
    sxy = 0;
    for (i=0;i<n;i++) {
      j = i + delay;
      while (j < 0)
	j += n;
      j %= n;
      sxy += (x[i] - mx) * (y[j] - my);
    }
    r = sxy / denom;      /* r is the correlation coefficient at "delay" */
    if(r>*rmax) {
      *rmax=r;
      *imax=delay;
    }
    if(delay==0) {
      *pcoef=r;
    }
  }
}

void cross_correlation(Reference::To<Pulsar::Profile> refcorr, Reference::To<Pulsar::Profile> stdcorr, double *rmax, int *imax, double *pcoef, bool verbose, char line[100])
{
  float fact_nbin;
  int corr_nbin=0;
  
  if((fact_nbin = 1.0*stdcorr->get_nbin()/refcorr->get_nbin())>=1) 
    //        stdcorr->halvebins(log2(fact_nbin));
    stdcorr->bscrunch(int(fact_nbin));
  else
    //    refcorr->halvebins(log2(1/fact_nbin));
    refcorr->bscrunch(int(1/fact_nbin));

    corr_nbin=stdcorr->get_nbin();
    if(verbose) cout << "Cross correlation: " <<corr_nbin << " bins applied"<< endl;
    if(verbose) cout << "bin fact stdprof/refprof: " <<fact_nbin <<endl;

  //call coef
  float *ref = refcorr->get_amps();
  float *std = stdcorr->get_amps();
  coef(ref, std, corr_nbin/2, corr_nbin, rmax, imax, pcoef, verbose);
  if (fact_nbin>=1) *imax = int(*imax*fact_nbin);
  if(verbose) cout << "Maximum cross correlation coeffeciency: " << *rmax << " on bin lag " << *imax <<endl;
  if(verbose) cout << "Present cross correlation coeffeciency: " << *pcoef <<endl;
  strncmp(line, "", 100);
  sprintf(line, "Maximum cross correlation coeffeciency = %f on lag %d.   Present = %f", *rmax, *imax, *pcoef);
}


void smooth(Reference::To<Pulsar::Profile> prof, bool vverbose)
{
  float *temp=prof->get_amps();
  float *smth=prof->get_amps();
  int i, k1, k2;
  for(i=0; i<int(prof->get_nbin()); i++){
    k1=i-1;
    k2=i+1;
    if(k1<0)                   k1=prof->get_nbin()+k1;
    if(k2>=int(prof->get_nbin()))   k2=prof->get_nbin()-k2;
    smth[i]=0.25*(temp[k1]+temp[k2])+0.5*temp[i];
    if (vverbose)    fprintf(stderr, "%d:%f %f  ",i, temp[i], smth[i]);
  }

  prof->set_amps(smth);
}

