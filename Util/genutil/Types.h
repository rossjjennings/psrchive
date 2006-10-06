//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Types.h,v $
   $Revision: 1.23 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Types_h
#define __Types_h

#include "Conventions.h"
#include <string>

namespace Signal {

  //! Dimensions of the data (so far)
  enum Dimension {
    //! Time given in MJD
    Time,
    //! Radio Frequency in MHz
    Frequency,
    //! Light
    Polarization,
    //! Pulsar Phase, for instance
    Phase,
    //! Mathematical space (real, complex, jones, etc.)
    Space
  };


  //! The scale in which units are measured
  enum Scale {
    //! Uncalibrated voltage
    EMF,
    //! Calibrated voltage
    Volts,
    //! Uncalibrated energy
    Energy,
    //! Calibrated energy
    Joules,
    //! Uncalibrated flux density
    FluxDensity,
    //! Reference flux density
    ReferenceFluxDensity,
    //! Calibrated flux density
    Jansky
  };
    
  //! Sources of observed signals
  enum Source {
    //! An unknown source
    Unknown,
    //! A pulsar
    Pulsar,
    //! A polarization calibrator, such as the linear noise diode
    PolnCal,
    //! A flux calibrator, such as Hydra A + the linear noise diode
    FluxCalOn,
    //! Near a flux calibrator, such as two degrees off Hydra A
    FluxCalOff,
    //! A calibrator solution
    Calibrator
  };

  //! Possible states of the data
  enum State {
    //! Nyquist sampled voltages (real)
    Nyquist,
    //! In-phase and Quadrature sampled voltages (complex)
    Analytic,
    //! Square-law detected total power
    Intensity,
    //! Square-law detected nth power
    NthPower,
    //! Square-law detected, two polarizations
    PPQQ,
    //! PP, QQ, Re[PQ], Im[PQ]
    Coherence,
    //! Stokes I,Q,U,V
    Stokes,
    //! Stokes invariant interval
    Invariant,
    //! Other
    Other,
    //! Just PP
    PP_State,
    //! Just QQ
    QQ_State
  };
  
  //! States of different polarization dimensions
  enum Component {
    //! Unknown
    None,
    //! Stokes I,Q,U,V
    Si, Sq, Su, Sv,
    //! Coherency Products, Linear Basis
    XX, YY, ReXY, ImXY,
    //! Coherency Products, Circular Basis
    LL, RR, ReLR, ImLR,
    //! Coherency Products, Elliptical Basis
    PP, QQ, RePQ, ImPQ,
    //! Stokes Invariant Interval
    Inv,
    //! Coherency Matrix Determinant
    DetRho
  };
  
  
  Component get_Component (Basis basis, State state, int ipol);
  
  int get_ipol (State state, Component poln);
  
  const char* source_string (Source source);
  const char* state_string (State state);
  const char* basis_string (Basis basis);

  const std::string Source2string (Source source);
  const std::string State2string (State state);
  const std::string Basis2string (Basis basis);
  const std::string Scale2string (Scale scale);

  //! Returns how many polarisations the State most likely is
  //! This function is for when a user specifies a desired output state and you need to convert to an npol
  //! Try and not use this function when you actually have data
  unsigned State2npol(State state);

  Basis string2Basis(std::string ss);
  Source string2Source(std::string ss);
  State string2State(std::string ss);
  Scale string2Scale(std::string ss);

  //! Tells you if your state is consistent with your npol and ndim
  bool valid_state(State state,unsigned ndim,unsigned npol, std::string& reason);
  
}

#endif

