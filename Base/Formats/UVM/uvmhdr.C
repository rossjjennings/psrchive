/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "uvmio.h"

#include <iostream>
using namespace std;

int main (int argc, char** argv)
{
  if (argc == 1)
    {
      cerr << "USAGE: uvmhdr <filename> \n" << endl;
      return -1;
    }


  char* filename = argv[1];
  int program = 0;
  int newscan = 0;
  uvm_header header;
  
  if (uvm_getheader (filename, &program, &newscan, &header) < 0)
    {
      cerr << "ERROR calling uvm_getheader" << endl;
      return -1;
    }

  cout << "version=" << header.aver << endl;

  return 0;
}

#if 0

c	line 1:	aver	 a3	version, telescope program
c		date	 i5	date (dddyy)
c		startime i5	time, seconds from AST midnight
c		source	 a9	pulsar or source name
c		arecnum	 i4,1x	record number in scan (i.e. freq. chans)
c		nscan	 i7	scan number (dddynnn)
c		aendscan i5	
c		abandwd	 e9.2	bandwidth in MHz
c		aposcrd	 i2	
c		apos1st	 f9.5	R.A. in radians
c		apos2st	 f9.5	Decl in radians
c		aazst	 f9.5	azimuth (rad) at start of scan
c		sver	 a3	"Stokes" analysis program version
c	line2:	za	 f9.5	zenith angle (rad) at start of scan
c		azaend	 f9.5	zenith angle (rad) current or at end of scan
c		aclkrat	 f9.5	clock rate in MHz
c		admptim	 i7	correlator dump time (microseconds)
c		admplen	 i7	=anumsbc X alensbc
c		admpsrd	 i7	
c		ainttim	 i7	
c		aextst	 i7	ext. start flag in avrg prgm; int period in SP
c		obfreq	 f12.5	observing frequency in MHz
c		apoladd	 i2	polarization addition flag = 0 for polarimetry
c		anumsbc	 i2	# subcorrelators used = 4 for polarimetry
c	line3:	alensbc	 i5	number of acf/ccf lags=number of freq. chans.
c		apbins	 i5	number of samples per period
c				   or sampled fraction
c		period	 f18.15	period, I6 accuracy in SP, full acc in aver prgm
c		adm	 f9.2	dispersion measure in pc/cm^3
c		arm	 f9.2	rotation measure in rad-m^2
c		samint	 f9.7	interval between samples in seconds (average)
c				   sampled fraction of period in SP program
c		admptot	 i7	
c		abtwscn	 e11.4,1x
c	line4:	scaleI	 f9.5	scale to convert IQUV to micro Janskys
c		sigmaI	 f9.5	standard deviation of offpulse baseline in I
c		nwins	 i3	0 for average program; # windows in SP program
c		nwbins	 5i3	number of samples in each window
c		baseval  e12.4	baseline value removed
c		wndows	 a5	function of windowns as above (i.e., "I", for
c				   interpulse; "P" for pulse; and "N" for noise)
c		anrecscn i2	
c		scantime f13.1	scan time in microseconds ( =#periods*period)
c		nseq	 i6,1x	integer count of pulses or averages
c		controlword o5	octal word giving settings in analysis program

#endif
