//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Types.h,v $
   $Revision: 1.2 $
   $Date: 2002/10/10 08:04:48 $
   $Author: straten $ */

#ifndef __Types_h
#define __Types_h

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
  enum Basis { Circular=0, Linear=1 };

  //! Sources of observed signals
  enum Source { Unknown, Pulsar, PolCal, FluxCal };

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
    //@{
    //! Stokes I,Q,U,V
    Si, Sq, Su, Sv,
    //@}
    //@{
    //! Coherency Products, Linear Basis
    XX, YY, ReXY, ImXY,
    //@}
    //@{
    //! Coherency Products, Circular Basis
    LL, RR, ReLR, ImLR,
    //@}
    //! Stokes Invariant Interval
    Inv,
    //! Coherency Matrix Determinant
    DetRho,
  };
  
  
  Component get_Component (Basis basis, State state, int ipol);
  
  int get_ipol (State state, Component poln);
  
  const char* state_string (State state);
}

#endif
