#include "Types.h"

//! Given FeedType and State, returns the default Component code of ipol
Signal::Component 
Signal::get_Component (FeedType type, State state, int ipol)
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

      if (type == Linear)
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
      else if (type == Circular)
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
      if (type == Linear)
	switch (ipol) 
	  {
	  case 0:
	    return XX;
	  case 1:
	    return YY;
	  default:
	    break;
	  }
      else if (type == Circular)
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
	return InvS;
      break;
      
    default:
      break;
    }
  
  return Invalid;
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
      if (poln == InvS || poln == DetRho) 
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
