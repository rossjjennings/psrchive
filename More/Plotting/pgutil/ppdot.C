
#include <iostream>
#include <vector>
#include <string>

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
       << endl;
}

int main (int argc, char** argv)
{
  string device = "/xs";
  int c = 0;

  while ((c = getopt(argc, argv, "hD:")) != -1) {
    switch (c)  {
    case 'h':
      usage();
      return -1;

    case 'D':
      device = optarg;
      break;
    }
  }

  if (optind >= argc) {
    usage();
    return -1;
  }

  FILE* fptr = fopen(argv[optind],"r");
  if (!fptr) {
    fprintf (stderr, "%s not found\n", argv[1]);
    return -1;
  }

  char name[32];
  float p1, pdot1, pb1, e;
  vector<float> p, pdot, pb;
  int psr = -1;

  while (fscanf (fptr, "%s %f %f %f %f %f %f\n", 
		 name, &p1, &e, &pdot1, &e, &pb1, &e) == 7) {
    if (p1 < 0.001 || p1 > 10) {
      // cerr << "bad p: " << p1 << " " << pdot1 << endl;
      continue;
    }

    if (pdot1 < 1e-6 || pdot1 > 1e5) {
      // cerr << "bad pdot: " << p1 << " " << pdot1 << endl;
      continue;
    }

    if (string(name) == "J0437-4715")
      psr = p.size();

    p.push_back (p1);
    pdot.push_back (pdot1);
    pb.push_back (pb1);
  }

  if (psr == -1)
    cerr << "0437 not found" << endl;
  else
    cerr << "0437 P: " << p[psr] << endl;

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

    float y = log(pdot[ipt]) / log10 - 15;
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
    if (ipt == psr)
      continue;
    if (pb[ipt] == 0)
      cpgpt1 (p[ipt], pdot[ipt], 1);
    else {
      cpgpt1 (p[ipt], pdot[ipt], 21);
      bin ++;
    }
  }
  cerr << bin << "/" << p.size() << " binaries" << endl;

  // mark 0437
  cpgsch (2.5);
  cpgpt1 (p[psr], pdot[psr], 2);
  cpgsch (1.5);
  cpgpt1 (p[psr], pdot[psr], 23);

  // draw lines of constant inferred surface magnetic field strength
  // log(Pdot)+15 = 2(log(B)-12) - log(P)
  float y1 = -6 -x1 -15; // start with 10^9 Gauss
  float y2 = -6 -x2 -15;

  cpgsch (1);
  cpgsls (2);
  
  int power = 9;
  for (int i=0; i<3; i++) {
    cpgmove (x1, y1);
    cpgdraw (x2, y2);

    if (i<2) {
      char lab[32];
      sprintf (lab, "10\\u%d\\dG", power);
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
      sprintf (lab, "10\\u%d\\dyr", power);
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
