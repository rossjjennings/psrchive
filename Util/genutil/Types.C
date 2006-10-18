/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Types.h"
#include "Error.h"
#include "tostring.h"

using namespace std;

//! Given Basis and State, returns the default Component code of ipol
Signal::Component 
Signal::get_Component (Basis basis, State state, int ipol)
{
  switch (state) 
    {
    case Stokes:
      switch (ipol)
	{
	case 0:
	  return Si;
	case 1:
	  return Sq;
	case 2:
	  return Su;
	case 3:
	  return Sv;
	default:
	  break;
	}
      break;

    case Coherence:

      if (basis == Linear)
	switch (ipol) 	{
	case 0:
	  return XX;
	case 1:
	  return YY;
	case 2:
	  return ReXY;
	case 3:
	  return ImXY;
	default:
	  break;
	}
      else if (basis == Circular)
	switch (ipol) 	{
	case 0:
	  return LL;
	case 1:
	  return RR;
	case 2:
	  return ReLR;
	case 3:
	  return ImLR;
	default:
	  break;
	}
      break;
  
    case PPQQ:
      if (basis == Linear)
	switch (ipol) 
	  {
	  case 0:
	    return XX;
	  case 1:
	    return YY;
	  default:
	    break;
	  }
      else if (basis == Circular)
	switch (ipol) 
	  {
	  case 0:
	    return LL;
	  case 1:
	    return RR;
	  default:
	    break;
	  }
      break;
      
    case Intensity:
      if (ipol == 0)
	return Si; 
      break;
    
    case NthPower:
      if (ipol == 0)
	return Si; 
      break;


    case Invariant:
      if (ipol == 0)
	return Inv;
      break;
      
    default:
      break;
    }
  
  return None;
}

//! Given State, returns the default ipol of Component
int Signal::get_ipol (State state, Component poln)
{
  switch (state) 
    {

    case Stokes:
      switch (poln)
	{
	case Si:
	  return 0;
	case Sq:
	  return 1;
	case Su:
	  return 2;
	case Sv:
	  return 3;
	default:
	  break;
	}
      break;

    case Coherence:
      switch (poln) 	{
	case LL:
	case XX:
	  return 0;
	case RR:
	case YY:
	  return 1;
	case ReLR:
	case ReXY:
	  return 2;
	case ImLR:
	case ImXY:
	  return 3;
	default:
	  break;
	}
      break;
  
    case PPQQ:
      switch (poln) 
	{
	case LL:
	case XX:
	  return 0;
	case RR:
	case YY:
	  return 1;
	default:
	  break;
	}
      break;
      
    case Intensity:
      if (poln == Si) 
	return 0;
      break;
   
    case NthPower:
      if (poln == Si) 
	return 0;
      break;


    case Invariant:
      if (poln == Inv || poln == DetRho) 
	return 0;
      break;

    default:
      break;
    }

  return -1;
}

const char* Signal::state_string (State state)
{
  switch (state)  {
  case Nyquist:
    return "Baseband Nyquist";
  case Analytic:
    return "Baseband Quadrature";
  case Stokes:
    return "Stokes Parameters";
  case Coherence:
    return "Coherency Products";
  case PPQQ:
    return "Polarimetric Intensity";
  case Intensity:
    return "Total Intensity";
  case NthPower:
    return "Stokes I to some power";
  case Invariant:
    return "Invariant Interval";
  default:
    return "Invalid";
  }
}

const char* Signal::source_string(Source source)
{
  switch( source ){
  case Unknown:
    return "Unknown";
  case Pulsar:
    return "Pulsar";
  case PolnCal:
    return "PolnCal";
  case FluxCalOff:
    return "FluxCal-Off";
  case FluxCalOn:
    return "FluxCal-On";
  case Calibrator:
    return "Calibrator";
  default:
    return "Invalid";
  }
}

const char* Signal::basis_string(Basis basis)
{
  switch( basis ){
  case Circular:
    return "Circular";
  case Linear:
    return "Linear";
  case Elliptical:
    return "Elliptical";
  default:
    return "Invalid";
  }
}

unsigned Signal::State2npol (State s)
{
  if( s==Nyquist || s==Analytic || s==PPQQ )
    return 2;
  if( s==Coherence || s==Stokes || s==Invariant )
    return 4;
  if( s==Intensity )
    return 1;
  if( s==NthPower )
    return 1;
  if( s==PP_State || s==QQ_State )
    return 1;

  throw Error(InvalidState,"Signal::State2npol()",
	      "State unknown!");

  return 1;
}

//! Tells you if your state is consistent with your npol and ndim
bool Signal::valid_state(Signal::State state,unsigned ndim,unsigned npol, string& reason){

  switch (state) {
  case Signal::Nyquist:
    if (ndim != 1)  {
      reason = "state=" + string(state_string(state)) + " and ndim!=1";
      return false;
    }
    break;

  case Signal::Analytic:
    if (ndim != 2) {
      reason = "state=" + string(state_string(state)) + " and ndim!=2";
      return false;
    }
    break;

  case Signal::Invariant:
  case Signal::Intensity:
  case Signal::NthPower:
    if (npol != 1) {
      reason = "state=" + string(state_string(state)) + " and npol!=1";
      return false;
    }
    break;

  case Signal::PPQQ:
    if (npol != 2) {
      reason = "state=" + string(state_string(state)) + " and npol!=2";
      return false;
    }
    break;

  case Signal::Coherence:
  case Signal::Stokes:
    if (ndim*npol != 4) {
      reason = "state=" + string(state_string(state)) + " and ndim*npol!=4";
      return false;
    }
    break;
  case Signal::PP_State:
  case Signal::QQ_State:
    if( npol != 1 || ndim != 1 ) {
      reason = "state=" + string(state_string(state)) + " and (ndim or npol) is not 1";
      return false;
    }	
    break;

  default:
    reason = "unknown state";
    return false;
  }

  return true;
}

// //////////////////////////////////////////////////////////////////////////
//
// Signal::State
//
// //////////////////////////////////////////////////////////////////////////

const string Signal::State2string (State state)
{
  switch (state)  {
  case Nyquist:
    return "Nyquist";
  case Analytic:
    return "Analytic";
  case Stokes:
    return "Stokes";
  case Coherence:
    return "Coherence";
  case PPQQ:
    return "PPQQ";
  case Intensity:
    return "Intensity";
  case NthPower:
    return "NthPower";
  case Invariant:
    return "Invariant";
  case PP_State:
    return "PP";
  case QQ_State:
    return "QQ";
  default:
    return "Invalid";
  }
}

Signal::State Signal::string2State (const string& ss)
{
  if(ss=="Nyquist")
    return Nyquist;
  if(ss=="Analytic")
    return Analytic;
  if(ss=="Intensity")
    return Intensity;
  if (ss=="NthPower")
    return NthPower;
  if(ss=="PPQQ")
    return PPQQ;
  if(ss=="Coherence")
    return Coherence;
  if(ss=="Stokes")
    return Stokes;
  if(ss=="PP")
    return PP_State;
  if(ss=="QQ")
    return QQ_State;
  
  throw Error (InvalidState, "Signal::string2State",
	       "Unknown state '" + ss + "'");
}

std::ostream& operator<< (std::ostream& ostr, Signal::State state)
{
  return ostr << State2string(state);
}

std::istream& operator >> (std::istream& is, Signal::State& state)
{
  return extraction (is, state, Signal::string2State);
}

// //////////////////////////////////////////////////////////////////////////
//
// Signal::Basis
//
// //////////////////////////////////////////////////////////////////////////

const string Signal::Basis2string (Basis basis)
{
  return basis_string(basis);
}

Signal::Basis Signal::string2Basis (const string& ss)
{
  if(ss=="Circular")
    return Signal::Circular;
  if(ss=="Linear")
    return Signal::Linear;
  if(ss=="Elliptical")
    return Signal::Elliptical;

  throw Error (InvalidState, "Signal::string2Basis",
	       "Unknown basis '" + ss + "'");
}

// //////////////////////////////////////////////////////////////////////////
//
// Signal::Source
//
// //////////////////////////////////////////////////////////////////////////

const string Signal::Source2string (Source source)
{
  return source_string(source);
}

Signal::Source Signal::string2Source (const string& ss)
{
  if(ss=="Pulsar")
    return Pulsar;
  if(ss=="PolnCal")
    return PolnCal;
  if(ss=="FluxCal-Off")
    return FluxCalOff;
  if(ss=="FluxCal-On")
    return FluxCalOn;
  if(ss=="Calibrator")
    return Calibrator;

  throw Error (InvalidState, "Signal::string2Source",
	       "Unknown source '" + ss + "'");
}

std::ostream& operator<< (std::ostream& ostr, Signal::Source source)
{
  return ostr << Source2string(source);
}

std::istream& operator >> (std::istream& is, Signal::Source& source)
{
  return extraction (is, source, Signal::string2Source);
}

// //////////////////////////////////////////////////////////////////////////
//
// Signal::Scale
//
// //////////////////////////////////////////////////////////////////////////

const string Signal::Scale2string (Scale scale)
{
  switch( scale ) {
  case EMF:
    return "EMF";
  case Volts:
    return "Volts";
  case Energy:
    return "Energy";
  case Joules:
    return "Joules";
  case FluxDensity:
    return "FluxDensity";
  case ReferenceFluxDensity:
    return "ReferenceFluxDensity";
  case Jansky:
    return "Jansky";
  default:
    return "Unknown";
  }
}

Signal::Scale Signal::string2Scale (const string& ss)
{
  if( ss=="EMF" )
    return EMF;
  if( ss=="Volts" ) 
    return Volts;
  if( ss=="Energy" )
    return Energy;
  if( ss=="Joules" )
    return Joules;
  if( ss=="FluxDensity" )
    return FluxDensity;
  if( ss=="ReferenceFluxDensity" )
    return ReferenceFluxDensity;
  if( ss=="Jansky" )
    return Jansky;

  throw Error (InvalidState, "Signal::string2Scale",
	       "Unknown scale '" + ss + "'");
}

std::ostream& operator<< (std::ostream& ostr, Signal::Scale scale)
{
  return ostr << Scale2string(scale);
}

std::istream& operator >> (std::istream& is, Signal::Scale& scale)
{
  return extraction (is, scale, Signal::string2Scale);
}
