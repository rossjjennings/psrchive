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
  case PolCal:
    return "PolCal";
  case FluxCalOff:
    return "FluxCal-Off";
  case FluxCalOn:
    return "FluxCal-On";
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
  default:
    return "Invalid";
  }
}

const string Signal::Source2string (Source source){ return source_string(source); }
const string Signal::Basis2string (Basis basis){ return basis_string(basis); }


Signal::Basis Signal::string2Basis(string ss){
  if(ss=="Circular")
    return Signal::Circular;
  return Signal::Linear;
}

Signal::Source Signal::string2Source(string ss){
  if(ss=="Pulsar")
    return Pulsar;
  else if(ss=="PolCal")
    return PolCal;
  else if(ss=="FluxCal-Off")
    return FluxCalOff;
  else if(ss=="FluxCal-On")
    return FluxCalOn;
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


