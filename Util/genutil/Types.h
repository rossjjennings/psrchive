//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Types.h,v $
   $Revision: 1.9 $
   $Date: 2003/02/12 16:20:11 $
   $Author: straten $ */

#ifndef __Types_h
#define __Types_h

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


  //! Different receiver feed configurations
  //! Be sure to modify 
  enum Basis { Circular=0, Linear=1 };

  //! Sources of observed signals
  enum Source { Unknown, Pulsar, PolnCal, FluxCalOn, FluxCalOff };

  //! Possible polarimetric states of the data
  enum State {
    //! Nyquist sampled voltages (real)
    Nyquist,
    //! In-phase and Quadrature sampled voltages (complex)
    Analytic,
    //! Square-law detected total power
    Intensity,
    //! Square-law detected, two polarizations
    PPQQ,
    //! PP, QQ, Re[PQ], Im[PQ]
    Coherence,
    //! Stokes I,Q,U,V
    Stokes,
    //! Stokes invariant interval
    Invariant
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

  const string Source2string (Source source);
  const string State2string (State state);
  const string Basis2string (Basis basis);

  Basis string2Basis(string ss);
  Source string2Source(string ss);
  State string2State(string ss);
  
}

#endif
