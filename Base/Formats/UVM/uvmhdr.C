/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "uvmio.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

void output (ostream& os, uvm_header& header, uvm_data& data);

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

  cerr << "newscan=" << newscan << endl;

  char buffer[80];

  uvm_get_source (&header, buffer);
  cout << "source=" << buffer << endl;

  uvm_get_observatory (&header, buffer);
  cout << "observatory=" << buffer << endl;

  uvm_get_windows (&header, buffer);
  cout << "windows=" << buffer << endl;

  cout << "date (dddyy)=" << header.date << endl;
  cout << "start time=" << header.startime 
       << " seconds after AST midnight" << endl << endl;

  cout << "scan record number=" << header.arecnum << endl;
  cout << "scan number (dddynnn)=" << header.nscan << endl;
  cout << "scan rec=" << header.anrecscn << endl;
  cout << "scan end=" << header.aendscan << endl << endl;

  cout << "frequency=" << header.obfreq << " MHz" << endl;
  cout << "bandwidth=" << header.abandwd << " MHz" << endl;
  cout << "clock rate=" << header.aclkrat << " MHz" << endl << endl;

  cout << "polarization addition=" << header.apoladd << endl;
  cout << "subcorrelators used=" << header.anumsbc << endl;
  cout << "correlator dump time =" << header.admptim << " mus" << endl;
  cout << "number of acf/ccf lags=" << header.alensbc << endl;
  cout << "number of samples per period=" << header.apbins << endl;
  cout << "number of pulses or averages=" << header.nseq << endl;
  cout << "number of windows=" << header.nwins << endl;

  for (int i=0; i<header.nwins; i++)
    cout << "\tsamples in window[" << i << "]=" << header.nwbins[i] << endl;

  cout << "admplen (=anumsbc X alensbc)=" << header.admplen << endl;
  cout << "admpsrd=" << header.admplen << endl;
  cout << "admptot=" << header.admptot << endl;

  cout << endl;

  cout << "Folding period=" << header.period << " s" << endl;
  cout << "Dispersion measure=" << header.adm << endl;
  cout << "Rotation measure=" << header.arm << endl;

  cout << "scale to convert muJys=" << header.scaleI << endl;
  cout << "baseline value removed=" << header.baseval << endl;
  
  cout << "R.A.=" << header.apos1st << " rad" << endl;
  cout << "Dec.=" << header.apos2st << " rad" << endl;

  int scan = 1;
  while (scan)
  {
    uvm_data data;
  
    if (uvm_getdata (program, &header, &data) < 0)
      {
	cerr << "ERROR calling uvm_getdata" << endl;
	return -1;
      }

    cout << "scale to convert muJys=" << header.scaleI << endl;
    cout << "baseline value removed=" << header.baseval << endl;

    char scanfilename [80];
    sprintf (scanfilename, "scan%04d", scan);

    ofstream os (scanfilename);

    output (os, header, data);
    
    scan++;
  }

  return 0;
}

void output (ostream& os, uvm_header& header, uvm_data& data)
{
  if (header.nwins == 0)
    {
      for (int ibin=0; ibin < header.apbins; ibin++)
	{
	  os << ibin;
	  for (int ipol=0; ipol < header.anumsbc; ipol++)
	    os << " " << data.data[ipol][ibin];
	  os << endl;
	}
    }
  else
    {
      int jbin=0;
      for (int iwin=0; iwin<header.nwins; iwin++)
	for (int ibin=0; ibin < header.nwbins[iwin]; ibin++)
	  {
	    os << " WIN=" << iwin << " " << ibin;
	    for (int ipol=0; ipol < header.anumsbc; ipol++)
	      os << " " << data.data[ipol][jbin];
	    
	    os << endl;
	    jbin ++;
	  }
    }
}

#if 0


c		samint	 f9.7	interval between samples in seconds (average)
c				   sampled fraction of period in SP program
c	line4:	scaleI	 f9.5	scale to convert IQUV to micro Janskys
c		sigmaI	 f9.5	standard deviation of offpulse baseline in I
c		scantime f13.1	scan time in microseconds ( =#periods*period)




c		aazst	 f9.5	azimuth (rad) at start of scan
c		sver	 a3	"Stokes" analysis program version
c	line2:	za	 f9.5	zenith angle (rad) at start of scan
c		azaend	 f9.5	zenith angle (rad) current or at end of scan

c		admplen	 i7	=anumsbc X alensbc
c		admpsrd	 i7	
c		ainttim	 i7	
c		aextst	 i7	ext. start flag in avrg prgm; int period in SP




c		admptot	 i7	
c		abtwscn	 e11.4,1x
c		nwbins	 5i3	number of samples in each window
c		baseval  e12.4	baseline value removed
c		wndows	 a5	function of windowns as above (i.e., "I", for
c				   interpulse; "P" for pulse; and "N" for noise)
c		anrecscn i2	
c		controlword o5	octal word giving settings in analysis program

#endif
