//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Types.h,v $
   $Revision: 1.1 $
   $Date: 2002/10/10 05:27:41 $
   $Author: straten $ */

#ifndef __Types_h
#define __Types_h

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

//! Types of observed source
enum SourceType { Unknown, Pulsar, PolCal, FluxCal };

//! Different receiver feed configurations
enum FeedType { Circular=0, Linear=1 };

namespace Signal {

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
    Invalid = -1,
    //@{
    //! Stokes I,Q,U,V
    Si, Sq, Su, Sv,
    //@}
    //@{
    //! Coherency Products, Linear Feeds
    XX, YY, ReXY, ImXY,
    //@}
    //@{
    //! Coherency Products, Circular Feeds
    LL, RR, ReLR, ImLR,
    //@}
    //! Stokes Invariant Interval
    InvS,
    //! Coherency Matrix Determinant
    DetRho,
  };
  
  
  Component get_Component (FeedType type, State state, int ipol);
  
  int get_ipol (State state, Component poln);
  
  const char* state_string (State state);
}

#endif
