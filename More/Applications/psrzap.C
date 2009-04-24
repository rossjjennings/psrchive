/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cpgplot.h>

#include <iostream>

#include "Pulsar/psrchive.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"
#include "Pulsar/DynamicBaselineSpectrum.h"
#include "TextInterface.h"

using namespace std;
using namespace Pulsar;

static PlotFactory factory;

enum cursor_type {
  freq_cursor,
  time_cursor,
  both_cursor
};

#define PROG "psrzap"

void usage() {
  cout << "psrzap - Interactive RFI zapper using off-pulse dynamic spectra" 
    << endl;
}

int main(int argc, char *argv[]) {

  /* Process any args */
  int opt=0;
  int verb=0;
  while ((opt=getopt(argc,argv,"hv"))!=-1) {
    switch (opt) {

      case 'v':
        verb++;
        Archive::set_verbosity(verb);
        break;

      case 'h':
      default:
        usage();
        exit(0);
        break;

    }
  }

  if (optind==argc) {
    usage();
    cerr << PROG ": No filename given" << endl;
    exit(-1);
  }

  /* Load file */
  string filename = argv[optind];
  Reference::To<Archive> orig_arch = Archive::load(filename);
  Reference::To<Archive> arch = orig_arch->clone();
  arch->dedisperse();
  arch->convert_state(Signal::Stokes);

  /* Create plot */
  DynamicBaselineSpectrum *dsplot = new DynamicBaselineSpectrum;
  TextInterface::Parser *plotint = dsplot->get_frame_interface();
  dsplot->configure("var=1");
  dsplot->set_reuse_baseline();
  cpgopen("/xs");

  /* Input loop */
  char ch='\0';
  enum cursor_type curs=both_cursor;
  float x0=0.0, y0=0.0, x1, y1;
  int click=0, mode=0;
  bool redraw=true, var=true, log=false;
  do {

    /* Redraw the plot if necessary */
    if (redraw) {
      cpgeras();
      dsplot->plot(arch);
      redraw = false;
      cout << PROG ": Currently displaying baseline " << (var?"variance":"mean")
        << " in " << (log?"log":"linear") << " scale" << endl;
    }

    /* On click 0 get start of a range */
    if (click==0) {
      if (curs==freq_cursor) mode=5;
      else if (curs==time_cursor) mode=6;
      else if (curs==both_cursor) mode=7;
      cpgband(mode,0,0,0,&x0,&y0,&ch);
    } 

    /* On click 1 get the end of a range */
    else if (click==1) {
      if (curs==freq_cursor) mode=3;
      else if (curs==time_cursor) mode=4;
      else if (curs==both_cursor) mode=2;
      cpgband(mode,0,x0,y0,&x1,&y1,&ch);
    }

    /* Debug */
#if 0 
    printf("x0=%.3f y0=%.3f x1=%.3f y1=%.3f ch='%c' click=%d\n",
        x0, y0, x1, y1, ch, click);
#endif

    /* Left mouse click = zoom*/
    if (ch=='A') {
      if (click==0) { click=1; continue; }
      if (click==1) {
        /* Do zoom here */
        char conf[256];
        float tmp;
        int itmp;
        if (curs==freq_cursor || curs==both_cursor) {
          if (y0>y1) { tmp=y0; y0=y1; y1=tmp; }
          sprintf(conf,"y:win=(%.3f,%.3f)",y0,y1);
          dsplot->configure(string(conf));
        }
        if (curs==time_cursor || curs==both_cursor) {
          if (x0>x1) { itmp=x0; x0=x1; x1=itmp; }
          sprintf(conf,"srange=(%d,%d)",(int)x0,(int)x1);
          dsplot->configure(string(conf));
        }
        redraw = true;
        click = 0;
        continue;
      }
    }

    /* Middle mouse click = nothing for now */
    if (ch=='D') {
      continue;
    }

    /* Right mouse click = zap */
    if (ch=='X') {

      /* Zap a single channel */
      if (click==0) {
        /* Do zap */
        click=0;
        continue;
      }

      /* Zap a range */
      if (click==1) {
        /* Channel range */
        if (curs==freq_cursor) {
        }
        /* Time range */
        else if (curs==time_cursor) {
        /* Time/freq rectangle */
        } else if (curs==both_cursor) {
        }
        click = 0;
        continue;
      }
    }

    /* Switch to freq mode */
    if (ch=='f') {
      curs = freq_cursor;
      click = 0;
      continue;
    }

    /* Switch to time mode */
    if (ch=='t') {
      curs = time_cursor;
      click = 0;
      continue;
    }

    /* Switch to time/freq mode */
    if (ch=='b') {
      curs = both_cursor;
      click = 0;
      continue;
    }

    /* Toggle variance plot */
    if (ch=='v') {
      var = !var;
      redraw = true;
      if (var) 
        dsplot->configure("var=1");
      else
        dsplot->configure("var=0");
      click = 0;
      continue;
    }

    /* toggle log scale */
    if (ch=='l') {
      log = !log;
      redraw = true;
      if (log) 
        dsplot->configure("cmap:log=1");
      else
        dsplot->configure("cmap:log=0");
      click = 0;
      continue;
    }

    /* Unset zoom */
    if (ch=='r') {
      dsplot->configure("srange=(-1,-1)");
      dsplot->configure("y:win=(0,0)");
      redraw = true;
      click = 0;
      continue;
    }

  } while (ch!='q');

}

