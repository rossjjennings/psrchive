#include "Error.h"

#include "Types.h"

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
  case Invariant:
    return "Invariant Interval";
  default:
    return "Invalid";
  }
}

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
  case Invariant:
    return "Invariant";
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

const string Signal::Source2string (Source source){ return source_string(source); }
const string Signal::Basis2string (Basis basis){ return basis_string(basis); }


Signal::Basis Signal::string2Basis(string ss){
  if(ss=="Circular")
    return Signal::Circular;
  if(ss=="Linear")
    return Signal::Linear;
  if(ss=="Elliptical")
    return Signal::Elliptical;
  return (Signal::Basis) -1;
}

Signal::Source Signal::string2Source(string ss){
  if(ss=="Pulsar")
    return Pulsar;
  else if(ss=="PolnCal")
    return PolnCal;
  else if(ss=="FluxCal-Off")
    return FluxCalOff;
  else if(ss=="FluxCal-On")
    return FluxCalOn;
  else if(ss=="Calibrator")
    return Calibrator;
  return Unknown;
}

Signal::State Signal::string2State(string ss){
  if(ss=="Nyquist")
    return Nyquist;
  else if(ss=="Analytic")
    return Analytic;
  else if(ss=="Intensity")
    return Intensity;
  else if(ss=="PPQQ")
    return PPQQ;
  else if(ss=="Coherence")
    return Coherence;
  else if(ss=="Stokes")
    return Stokes;
  else if(ss=="Invariant")
    return Invariant;
  
  throw Error(InvalidState,"string2State()",
	      "Unknown state- '%s'",ss.c_str());
  return Invariant;  // Because you gotta return something
}

//! Returns how many polarisations the State most likely is
//! This function is for when a user specifies a desired output state and you need to convert to an npol
//! Try and not use this function when you actually have data
unsigned Signal::State2npol(State s){
  if( s==Nyquist || s==Analytic || s==PPQQ )
    return 2;
  if( s==Coherence || s==Stokes || s==Invariant )
    return 4;
  if( s==Intensity )
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

  default:
    reason = "unknown state";
    return false;
  }

  return true;
}

const string Signal::Scale2string (Scale scale){
  switch( scale ) {

    //! Uncalibrated voltage
  case EMF:
    return "EMF";
    
    //! Calibrated voltage
  case Volts:
    return "Volts";
    
      //! Uncalibrated energy
  case Energy:
    return "Energy";

      //! Calibrated energy
  case Joules:
    return "Joules";

      //! Uncalibrated flux density
  case FluxDensity:
    return "FluxDensity";
    
    //! Reference flux density
  case ReferenceFluxDensity:
    return "ReferenceFluxDensity";
    
    //! Calibrated flux density
  case Jansky:
    return "Jansky";

  default:
    return "Unknown";

  }
}

Signal::Scale Signal::string2Scale(string ss){
  if( ss=="EMF" ) return EMF;
  else if( ss=="Volts" ) return Volts;
  else if( ss=="Energy" ) return Energy;
  else if( ss=="Joules" ) return Joules;
  else if( ss=="FluxDensity" ) return FluxDensity;
  else if( ss=="ReferenceFluxDensity" ) return ReferenceFluxDensity;
  else if( ss=="Jansky" ) return Jansky;

  throw Error(InvalidParam,"Signal::string2Scale()",
	      "Could not parse '%s' as a Signal::Scale",
	      ss.c_str());
}

