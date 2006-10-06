//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2002, 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/psrchive.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __Pulsar_psrchive_h
#define __Pulsar_psrchive_h

/*! \mainpage 
 
  \section Introduction
 
  The PSRCHIVE Library implements a set of base classes (in C++) that 
  may be used in the storage, manipulation, and analysis of the 
  observational data commonly used in pulsar experiments.
  The base classes implement a minimal set of general, flexible
  routines.
 
  \section profiles Pulse Profiles
 
  The basic quantity observed in most pulsar experiments is the pulse
  profile: a one-dimensional array of some measured quantity as a
  function of pulse phase.  This is represented by the Pulsar::Profile
  class.  The Pulsar::Integration class contains a two-dimensional
  array of Pulsar::Profile objects, each integrated over the same time
  interval.  The Pulsar::Profile objects are usually organized as a
  function of polarimetric measure and observing frequency, though
  other measured states are possible.  The Pulsar::Archive class
  implements a one-dimensional array of Pulsar::Integration objects,
  each with similar observational parameters and each in "pulse phase"
  with eachother.

  \section minimal Minimal Interface

  The Pulsar::Profile class implements a minimal set of operations
  required to manipulate a pulsar profile in the phase domain.  These
  include, but are not limited to:
  <UL>
  <LI> operator += - adds offset to each bin of the profile </LI>
  <LI> operator *= - multiplies each bin of the profile by scale </LI>
  <LI> rotate - rotates the profile in phase </LI>
  <LI> bscrunch - integrates neighbouring phase bins in profile </LI>
  <LI> fold - integrates neighbouring sections of the profile </LI>
  </UL>

  As well, a basic set of routines are included that may be used to
  calculate statistics, find minima and maxima, find the shift between
  the profile and a standard, etc..  Combinations of these functions
  can be used to perform basic tasks.  For instance, baseline removal
  is simply and transparently implemented as:

  <pre> profile -= mean (find_min_phase()); </pre> 

  The Pulsar::Integration class implements a minimal set of operations
  required to manipulate a set of Pulsar::Profile objects in the
  polarization and frequency domains.  In addition to simple nested
  calls of the above functions, these include:
  <UL>
  <LI> dedisperse - rotates all profiles to remove dispersion delays 
  between channels </LI>
  <LI> defaraday - transforms all profiles to remove faraday rotation
  between channels </LI>
  <LI> fscrunch - integrates profiles from neighbouring channels </LI>
  <LI> pscrunch - integrates profiles from two polarizations into 
  one total intensity </LI>
  <LI> invint - forms the polarimetric invariant interval from 
  Stokes (I,Q,U,V) </LI>
  <LI> transform - performs a polarimetric transformation </LI>
  </UL>

  The Pulsar::Archive virtual base class is the interface that will be
  used in most applications.  In addition to providing interfaces to
  all of the above functions, the Pulsar::Archive class implements:
  <UL>
  <LI> tscrunch - integrates profiles from neighbouring Integrations </LI>
  <LI> append - copies (or transfers) the Integrations from one Archive
  to another </LI>
  <LI> set_ephemeris - installs a new ephemeris and polyco </LI>
  <LI> set_polyco - installs a new polynomial and aligns all profiles
  to it </LI>
  </UL>

  For a complete list of the methods defined in each of these base classes,
  please see the <a href="annotated.html">Compound List</a>.

  \section format Archive File Formats

  The Pulsar::Archive virtual base class specifies only the minimal
  set of information required in order to implement basic data
  reduction algorithms.  Although it specifies the interface to set
  and get the values of various attributes and load and unload data
  from file, no storage or I/O capability is implemented by
  Pulsar::Archive.  These methods, especially those related to file
  I/O, must be implemented by classes that inherit the Pulsar::Archive
  base class.

  Most observatories and research groups use unique file formats and
  associate different pieces of information with their observations.
  The derived classes must therefore implement the storage and
  modification of this auxilliary information.  This is done using
  the Archive::Extension classes.

  In general, applications need not know anything about the specific
  archive file format with which they are dealing.  New Pulsar::Archive
  instances may be created and loaded from file by calling the static
  Pulsar::Archive::load factory.

  \subsection plugin File Format Plugins

  Classes that inherit Pulsar::Archive and implement the I/O routines
  for a specific archive file format are loaded as plugins.  Plugins
  are registered for use in applications by inheriting the Advocate 
  template base class and defining the pure virtual methods of the 
  Agent base class.

 */

#define PSRCHIVE_HTTP "http://psrchive.sourceforge.net"

#endif
