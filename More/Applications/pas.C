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

void plot_it(Reference::To<Pulsar::Archive>, Reference::To<Pulsar::Archive>, int, int, int, char *, char *);
void coef (float *, float *, int, int, double *, int *, double *, bool);
void cross_correlation(Reference::To<Pulsar::Profile>, Reference::To<Pulsar::Profile>, double *, int *, double *, bool, char *);
void smooth (Reference::To<Pulsar::Profile>, bool);
double convt(Reference::To<Pulsar::Archive>, float, bool);

void usage ()
{
  cout << "Program to align standard profiles \n"
    "Usage: pas [-r reference profile] [-s standard profile] [-v] [-V] \n"
    "Where the options are as follows \n"
    " -h        This help page \n"
    " -i        Revision information\n"
    " -r ref    Reference profile \n"
    " -s std    Standard profile to align with reference profile \n"
    " -v        Verbose output \n"
    " -V        Very verbose output \n\n"

    " This program works on keyboard interaction. To set baseline zero you type 'z' to set the start point, move the cursor and type 'z' again to accept the region.\n"
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
  float stdphase=0;
  unsigned int i;
  bool verbose = false;
  bool vverbose = false;
  int c = 0;
  int imax=0;
  bool zflag = false;
  double rmax=0, pcoef=0;
  //color of lines
  int ci_ref = 1;  //white  
  int ci_std = 2;  //red
  int ci_tex = 5;  //grey

  const char* args = "hir:s:vV";

  while ((c = getopt(argc, argv, args)) != -1) {
    switch (c) {
      
    case 'h':
      usage ();
      return 0;

    case 'i':
      cout << "$Id: pas.C,v 1.4 2003/06/05 06:08:59 pulsar Exp $" << endl;
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

  if (refname == NULL) {
    cerr << "No specified reference profile " <<endl;
    usage();
    return 0;
  }
  if (stdname == NULL) {
    cerr << "No specified standard profile " <<endl;
    usage();
    return 0;
  }

  Error::handle_signals ();

  //original archive
  Reference::To<Pulsar::Archive> refarch = Pulsar::Archive::load(refname);
  Reference::To<Pulsar::Archive> stdarch = Pulsar::Archive::load(stdname);
  if(verbose) cout << "archives loaded" << endl;

  //keep the original archive
  Reference::To<Pulsar::Archive> refarch0 (refarch->clone());
  Reference::To<Pulsar::Archive> stdarch0 (stdarch->clone());
  //profile to correlate
  Reference::To<Pulsar::Archive> refcorr (refarch->clone()); 
  Reference::To<Pulsar::Archive> stdcorr (stdarch->clone());

  //check reference profile
  if( verbose ){
    cout << "Reference profile: " << refarch->get_filename() << endl;
    cout <<"  Profile has " << refarch->get_Profile(0, 0, 0)->get_nbin() << " bins" << endl;
  }
  if( !power_of_two(refarch->get_Profile(0, 0, 0)->get_nbin()) )
    throw Error(InvalidState,"Check: ",
		"profile from archive '%s' doesn't have nbin a power of two (%d)", 
		refarch->get_filename().c_str(), refarch->get_Profile(0, 0, 0)->get_nbin());
  
  //check new profile
  if( verbose ) {
    cout << "Processing profile:" << stdarch->get_filename() << endl;
    cout <<"  Profile has " << stdarch->get_nbin() << " bins" << endl;
  }
  if( !power_of_two(stdarch->get_Profile(0, 0, 0)->get_nbin()) )
    throw Error(InvalidState,"Check: ",
		"profile from archive '%s' doesn't have nbin a power of two (%d)", 
		stdarch->get_filename().c_str(),stdarch->get_nbin());

  refarch->rotate(convt(refarch, -0.5*refarch->get_nbin(), verbose));
  stdarch->rotate(convt(stdarch, -0.5*stdarch->get_nbin(), verbose));
  *refcorr = *refarch;
  *stdcorr = *stdarch;

  cout << "Input plot device : ";
  cin >> plotdev;

  cross_correlation(refcorr->get_Profile(0, 0, 0), stdcorr->get_Profile(0, 0, 0), &rmax, &imax, &pcoef, verbose, line);

  //plot profiles
  plot_it(refarch, stdarch, ci_ref, ci_std, ci_tex, line, plotdev);

  opts = ' ';
  if (cpgcurs(&curs_x, &curs_y, &opts) == 1) {
    while (opts != 'e') {  
      try {
	switch(opts) {
	case 'a':   //Align
	    stdarch->rotate(convt(stdarch, imax, verbose));
	    if(verbose) cout << "Align: rotated " <<imax <<" bins"<<endl;
	    break;

	case 'b':  //Bscrunch
	  stdarch->bscrunch(2);
	  if( verbose ) cout << "Bscrunch: Profile is smoothed by factor 2" << endl;
	  break;
	 
	case 'c':  //Centre Profile
	  stdphase=stdarch->find_max_phase();
	  if( verbose ) cout << "Centre: Maximum phase " << stdphase << endl;
	  stdarch->rotate((stdphase-0.5)*stdarch->get_Integration(0)->get_folding_period());
	  if( verbose ) cout << "Centre: Centre the profile, done" <<endl;
	  break;

	case 'f': //Fast rotate, to the right
	  stdarch->rotate (convt(stdarch, -20.0, verbose));
	  if( verbose ) cout << "Fast Rotate profile: 20 bins to the right" <<endl;
	  break;
	  
	case 'l':  //Left rotate
	  stdarch->rotate (convt(stdarch, 1.0, verbose));
	  if( verbose ) cout << "Rotate profile: 1 bin to the left" << endl;
	  break;
	
	case 'm':   //smooth the profile
	  smooth(stdarch->get_Profile(0, 0, 0), vverbose);
	  if( verbose ) cout << "Smooth: profile smoothed" << endl;
	  break;

	case 'o':  //Back to Origin
	  *stdarch = *stdarch0;
	  stdarch->rotate(convt(stdarch, -0.5*stdarch->get_nbin(), verbose));
	  *stdcorr = *stdarch;
 	  if( verbose ) cout << "Back to origin" << endl;
	  break;
	  
	case 'q':  //Rotate quarterly
	  stdarch->rotate(convt(stdarch, -0.25*stdarch->get_nbin(), verbose)); //right rotate
	  if( verbose ) cout << "Center: Profile is centered" << endl;
	  break;
	
	case 'r':  //Right rotate
	  stdarch->rotate (convt(stdarch, -1.0, verbose));
	  if( verbose ) cout << "Rotate profile: 1 bin to the right" << endl;
	  break;
	   
	case 'z':   //Zero base line
	  cout << "Zero baseline: start point set at:" << curs_x << endl;
	  cout << "Zero baseline: move cursor and set the range" <<endl;
	  opts = ' ';
	  cpgband(2, 0, curs_x, curs_y, &x, &y, &opts);
	  if (opts == 'z') {
	    cout << "Zero baseline: end point set at:" << x << endl;
	    if(verbose) cout << "Zero baseline: zero the profile base line" << endl;
	    float * tmpdata=stdarch->get_Profile(0, 0, 0)->get_amps();
	    int istart, iend;
	    if (curs_x > x) {float tmp=x; x=curs_x; curs_x=tmp;}
	    istart = int(curs_x*stdarch->get_Profile(0, 0, 0)->get_nbin());
	    if (istart<0) istart=0;
	    fprintf(stderr, "istart = %d\n", istart);
	    iend = int(x*stdarch->get_Profile(0, 0, 0)->get_nbin());
	    if(unsigned(iend)>stdarch->get_Profile(0, 0, 0)->get_nbin()) 
	      iend=stdarch->get_Profile(0, 0, 0)->get_nbin();
	    if(verbose) cout << "Zero baseline: start & end bin number: " <<istart << ", " <<iend<<endl;
	    if(zflag==false) {
	      meantmp=0;
	      for(i=unsigned(istart); i<unsigned(iend); i++) {
		meantmp+=tmpdata[i];
	      }
	      meantmp/=(iend-istart); 
	      zflag=true;
	    }
	    
	    if (verbose) cout << "Zero baseline: baseline value = " << meantmp << endl;
	    for(i=unsigned(istart); i<unsigned(iend); i++) 
	      tmpdata[i]=meantmp;

	    if (vverbose) 
	      for(i=0; i<stdarch->get_Profile(0, 0, 0)->get_nbin(); i++) 
		cout << i << ": " << tmpdata[i]<<"  ";
	      
	    stdarch->get_Profile(0, 0, 0)->set_amps(tmpdata);
	    if (verbose) cout << "Zero baseline: done" << endl;
	  }
	  else cout << "Zero baseline: range not set" << endl;
	  break;
	  
	case 's':  //Save
	  string the_old = stdarch->get_filename().c_str();
	  if(verbose) cout << "Save: the archive file is " << the_old <<endl;
	  string the_new = " ";
	  cout << "Save: the center freq is " << stdarch->get_centre_frequency() << "MHz, "<< (30/(1e-3* stdarch->get_centre_frequency()))<< "cm" <<endl;
	  cout << "      new file name ('x' for default name): ";
	  cin >> the_new;
	  if( the_new == "x" ) {
	    int index = the_old.find_last_of(".",the_old.length());
	    string primary = the_old.substr(0, index);
	    the_new = primary + "." + "new";
	    cout << "      default file name " << the_new << endl;
	  }
	  stdarch->rotate(convt(stdarch, -0.5*stdarch->get_nbin(), verbose));
	  stdarch->unload(the_new);
	  stdarch->rotate(convt(stdarch, 0.5*stdarch->get_nbin(), verbose));
	  cout << "Save: new standard profile " << the_new << " written to disk" << endl;
	  break;	 
	}

	*refcorr=*refarch;
	*stdcorr=*stdarch;
	cross_correlation(refcorr->get_Profile(0, 0, 0), stdcorr->get_Profile(0, 0, 0), &rmax, &imax, &pcoef, verbose, line);
	plot_it(refarch, stdarch, ci_ref, ci_std, ci_tex, line, plotdev);
	cout << "Waiting for option ...." << endl;
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


void plot_it(Reference::To<Pulsar::Archive> refarch, Reference::To<Pulsar::Archive> stdarch, int ci_ref, int ci_std, int ci_tex, char line[100], char plotdev[5]) {
  float x, y;
  char str[50];
  unsigned i;
  //options
  cpgbeg(0, plotdev, 1, 1);
  
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
  sprintf(str, "Reference profile    %d bins", refarch->get_Profile(0, 0, 0)->get_nbin());
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
      cpgtext (x, y, "q:  Quarter rotate to the right");
    else if(i==3)
      cpgtext (x, y, "r:  Right rotate profile, step: 1 bin");
    else if(i==4)
      cpgtext (x, y, "s:  Save the standard profile");
    else if(i==5)
      cpgtext (x, y, "z:  set the range and Zero the base line"); 
    else  break;
  }
  cpgsci (ci_std);
  cpgmove (x, 15);
  cpgdraw (x+10, 15);
  strncpy(str, "", 50);
  sprintf(str, "New profile    %d bins", stdarch->get_Profile(0, 0, 0)->get_nbin());
  cpgtext (x+13, 15, str);

  //write the cross correlation function
  cpgsci (5);
  cpgtext (10, 0, line);

  //profiles window
  cpgsvp (0.1, 0.9, 0.05, 0.7);
  cpgswin (0, 1, 0, 1);
  
  //draw the centre line
  cpgsci (11); //blue
  cpgsls (2);
  cpgmove (0.5, 0.0);
  cpgdraw (0.5, 1.0);
  
  //draw the profiles
  cpgsls (1);
  cpgsci (ci_ref);
  refarch->get_Profile(0, 0, 0)->Pulsar::Profile::display (0, 0, 1, 0, 1, 1.0);
  cpgsci (ci_std);
  stdarch->get_Profile(0, 0, 0)->Pulsar::Profile::display (0, 0, 1, 0, 1, 1.0);
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
  
  /* Calculate the correlation coefficient on lag 'delay' */
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

void cross_correlation(Reference::To<Pulsar::Profile> refprof, Reference::To<Pulsar::Profile> stdprof, double *rmax, int *imax, double *pcoef, bool verbose, char line[100])
{
  float fact_nbin;
  int corr_nbin=0;
  
  if((fact_nbin = 1.0*stdprof->get_nbin()/refprof->get_nbin())>=1) 
    //        stdprof->halvebins(log2(fact_nbin));
    stdprof->bscrunch(int(fact_nbin));
  else
    //    refprof->halvebins(log2(1/fact_nbin));
    refprof->bscrunch(int(1/fact_nbin));

    corr_nbin=stdprof->get_nbin();
    if(verbose) cout << "Cross correlation: " <<corr_nbin << " bins applied"<< endl;
    if(verbose) cout << "bin fact stdprof/refprof: " <<fact_nbin <<endl;

  //call coef
  float *ref = refprof->get_amps();
  float *std = stdprof->get_amps();
  coef(ref, std, corr_nbin/2, corr_nbin, rmax, imax, pcoef, verbose);
  if (fact_nbin>=1) *imax = int(*imax*fact_nbin);
  if(verbose) cout << "Maximum cross correlation coeffecient: " << *rmax << " on bin lag " << *imax <<endl;
  if(verbose) cout << "Present cross correlation coeffecient: " << *pcoef <<endl;
  strncmp(line, "", 100);
  sprintf(line, "Maximum cross correlation coeffecient = %f on lag %d.   Present = %f", *rmax, *imax, *pcoef);
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


double convt(Reference::To<Pulsar::Archive> arch, float nbin, bool verbose)
{
  double phase_in_time;
  phase_in_time=nbin*arch->get_Integration(0)->get_folding_period()/arch->get_nbin();
  if (verbose)  cout << "Convert: phase in time " << phase_in_time <<" (s)" << endl;
  return phase_in_time;
}
