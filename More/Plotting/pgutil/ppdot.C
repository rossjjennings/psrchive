
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <cpgplot.h>

using namespace std;

void usage ()
{
  cerr << 
    "usage: ppdot file\n"
    "where 'file' contains columns with P and PDOT (10^-15)\n"
    "      -s psr  to add a special symbol for the named pulsar\n"
       << endl;
}

template<typename Container, typename Type>

bool found (const Container& container, const Type& element)
{
  return std::find (container.begin(), container.end(), element) != container.end();
}

int main (int argc, char** argv)
{
  string device = "?";
  int c = 0;

  vector<string> special;

  while ((c = getopt(argc, argv, "hD:s:")) != -1)
  {
    switch (c)
    {
    case 'h':
      usage();
      return -1;

    case 'D':
      device = optarg;
      break;

    case 's':
      special.push_back(optarg);
      break;
    }
  }

  if (optind >= argc)
  {
    usage();
    return -1;
  }

  FILE* fptr = fopen(argv[optind],"r");
  if (!fptr)
  {
    fprintf (stderr, "%s not found\n", argv[1]);
    return -1;
  }

  char name[32];
  float p1, pdot1, pb1, e;
  vector<float> p, pdot, pb;
  vector<unsigned> psr;

  int scanned = 0;

  while ((scanned = fscanf (fptr, "%s %f %f %f\n", 
		 name, &p1, &pdot1, &pb1)) == 4) 
  {
    cerr << "psr=" << name << " P=" << p1 << " Pdot=" << pdot1 << endl;

    if (p1 < 0.001 || p1 > 10)
    {
      cerr << "bad p: " << p1 << " " << pdot1 << endl;
      continue;
    }

    if (pdot1 < 1e-20 || pdot1 > 1e-9)
    {
      cerr << "bad pdot: " << p1 << " " << pdot1 << endl;
      continue;
    }

    if ( found (special, string(name)) )
      psr.push_back( p.size() );

    p.push_back (p1);
    pdot.push_back (pdot1);
    pb.push_back (pb1);
  }

  cerr << "Columns scanned from last line: " << scanned << endl;

  cerr << "Loaded from file: " << p.size() << endl;

  cerr << "Special pulsars: " << psr.size() << endl;

  float xmin=0, xmax=0;
  float ymin=0, ymax=0;
  float log10 = log(10);
  unsigned ipt = 0;

  for (ipt=0; ipt<p.size(); ipt++) {
    float x = log(p[ipt]) / log10;
    p[ipt] = x;

    if (ipt==0 || x < xmin)
      xmin = x;
    if (ipt==0 || x > xmax)
      xmax = x;

    float y = log(pdot[ipt]) / log10;
    pdot[ipt] = y;
    if (ipt==0 || y < ymin)
      ymin = y;
    if (ipt==0 || y > ymax)
      ymax = y;
  }

  cpgbeg (0, device.c_str(), 0, 0);
  cpgsch (1.5);
  cpgslw (3);
  cpgsvp (.2, .9, .2, .9);

  float fm = .05;
  float xm = (xmax - xmin) * fm;
  float ym = (ymax - ymin) * fm;

  float x1 = -3;
  float x2 = 1;
  cpgswin (x1, x2, -21, ymax+ym);

  cpgbox ("bclnst", 0,0, "bcnst", 0,0);
  cpglab ("Pulse Period, \\(2066) (s)", "log \\(2066)\\b\\u\\u \\(2710)", " ");

  cpgsch (1.5);
  unsigned bin = 0;
  for (ipt=1; ipt<p.size(); ipt++) {
    if ( found (psr, ipt) )
      continue;
    if (pb[ipt] == 0)
      cpgpt1 (p[ipt], pdot[ipt], 1);
    else {
      cpgpt1 (p[ipt], pdot[ipt], 21);
      bin ++;
    }
  }
  cerr << bin << "/" << p.size() << " binaries" << endl;

  // mark special pulsars
  for (unsigned i=0; i < psr.size(); i++)
  {
    cpgslw (5);
    cpgsci (2);
    cpgsch (2.5);
    cpgpt1 (p[psr[i]], pdot[psr[i]], 2);
    cpgsch (1.5);
    cpgpt1 (p[psr[i]], pdot[psr[i]], 23);
  }

  cpgsci(1);
  cpgslw(1);

  // draw lines of constant inferred surface magnetic field strength
  // log(Pdot)+15 = 2(log(B)-12) - log(P)
  float y1 = -6 -x1 -15; // start with 10^9 Gauss
  float y2 = -6 -x2 -15;

  cpgsch (1);
  cpgsls (2);
  
  int power = 9;
  for (int i=0; i<3; i++) 
  {
    cpgmove (x1, y1);
    cpgdraw (x2, y2);

    if (i<2) {
      char lab[32];
      sprintf (lab, "10\\u%d\\d G", power);
      cpgptxt (x1+xm, y1, 0,0, lab);
    }

    // increase B by 10^2
    y1 += 4.0;
    y2 += 4.0;
    power += 2;
  }

  // draw lines of constant characteristic age
  // log(Pdot) = log(P) -log2 -log(tau)
  float log2  = log(2)/log10;
  float logyr = log(365.25 * 86400.0)/log10;
  cerr << "log(s/yr)=" << logyr << endl;

  y1 = x1 -log2 -logyr -3; // start with 10^3 years
  y2 = x2 -log2 -logyr -3;

  cpgsch (1);
  cpgsls (4);
  
  power = 3;
  for (int i=0; i<4; i++) {
    cpgmove (x1, y1);
    cpgdraw (x2, y2);

    if (i>0) {
      char lab[32];
      sprintf (lab, "10\\u%d\\d yr", power);
      cpgptxt (x2-xm, y2, 0,1, lab);
    }

    // increase tau by 10^3
    y1 -= 3.0;
    y2 -= 3.0;
    power += 3;
  }

  cpgend();
  return 0;
}
