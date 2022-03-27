/***************************************************************************
 *
 *   Copyright (C) 2003-2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorStokes.h"
#include "TextInterface.h"
#include "templates.h"

using namespace Pulsar;
using namespace std;

//! Default constructor
CalibratorStokes::CalibratorStokes ()
  : Extension ("CalibratorStokes")
{
  // the default behaviour of pcm since 2002
  coupling_point = BeforeBasis;

  current = 0;
}

//! Copy constructor
CalibratorStokes::CalibratorStokes (const CalibratorStokes& extension)
  : Extension ("CalibratorStokes")
{
  operator = (extension);
  current = 0;
}

//! Assignment operator
const CalibratorStokes&
CalibratorStokes::operator= (const CalibratorStokes& other)
{
  stokes = other.stokes;
  coupling_point = other.coupling_point;
  
  return *this;
}

//! Destructor
CalibratorStokes::~CalibratorStokes ()
{
}

//! Set the name of the point where the reference source signal is coupled
void CalibratorStokes::set_coupling_point (CouplingPoint point)
{
  coupling_point = point;
}
   
//! Get the name of the point where the reference source signal is coupled
CalibratorStokes::CouplingPoint CalibratorStokes::get_coupling_point () const
{
  return coupling_point;
}

//! Set the number of frequency channels
void CalibratorStokes::set_nchan (unsigned nchan)
{
  stokes.resize (nchan);
}

//! Get the number of frequency channels
unsigned CalibratorStokes::get_nchan () const
{
  return stokes.size();
}

void CalibratorStokes::remove_chan (unsigned first, unsigned last)
{
  remove (stokes, first, last);
}

//! Set the validity flag for the specified channel
void CalibratorStokes::set_valid (unsigned ichan, bool valid)
{
  range_check (ichan, "CalibratorStokes::set_valid");

  float validity = (valid) ? 1.0 : 0.0;

  stokes[ichan][0].val = validity;
}

//! Get the validity flag for the specified channel
bool CalibratorStokes::get_valid (unsigned ichan) const
{
  range_check (ichan, "CalibratorStokes::get_valid");

  return stokes[ichan][0].val != 0.0;
}

//! Set the Stokes parameters of the specified frequency channel
void CalibratorStokes::set_stokes (unsigned ichan,
				   const Stokes< Estimate<float> >& s)
{
  range_check (ichan, "CalibratorStokes::set_stokes");

  stokes[ichan] = s;
  stokes[ichan][0] = 1.0;
}

//! Get the Stokes parameters of the specified frequency channel
Stokes< Estimate<float> > 
CalibratorStokes::get_stokes (unsigned ichan) const
{
  range_check (ichan, "CalibratorStokes::get_stokes");

  return stokes[ichan];
}

Estimate<float>
CalibratorStokes::get_Estimate ( unsigned iparam, unsigned ichan ) const
{
  assert (iparam < 3);
  return get_stokes(ichan)[iparam+1];
}

void CalibratorStokes::set_Estimate (unsigned iparam, unsigned ichan,
                                     Estimate<float>& val)
{
  range_check (ichan, "CalibratorStokes::get_Estimate");
  assert (iparam < 3);

#if _DEBUG
  cerr << "CalibratorStokes::set_Estimate iparam=" << iparam
       << " ichan=" << ichan << " val=" << val << endl;
#endif

  stokes[ichan][iparam+1] = val;
}

void CalibratorStokes::range_check (unsigned ichan, 
				    const char* method) const
{
  if (ichan >= stokes.size())
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d", 
		 ichan, stokes.size());
}
	       
void CalibratorStokes::frequency_append (Archive* to, const Archive* from)
{
  const CalibratorStokes* ext = from->get<CalibratorStokes>();
  if (!ext)
    throw Error (InvalidState, "CalibratorStokes::frequency_append",
		 "other Archive does not have a CalibratorStokes");

  stokes.insert ( (in_frequency_order (to, from))
		  ? stokes.end() : stokes.begin(),
		  ext->stokes.begin(), ext->stokes.end() );
}


class CalibratorStokes::PolnVector : public Reference::Able
{
public:

  PolnVector (CalibratorStokes* instance) 
  { parent = instance; ichan = 0; }

  void set_ichan (unsigned i)
  { ichan = i; }

  double get_value (unsigned k) const
  { return parent->get_stokes(ichan)[k+1].get_value(); }

  double get_variance (unsigned k) const
  { return parent->get_stokes(ichan)[k+1].get_variance(); }

  unsigned get_nparam () const
  { return 3; /* Q,U,V */ }

  class Interface : public TextInterface::To<PolnVector>
  {
  public:
    Interface (PolnVector* s = 0)
    {
      if (s)
	set_instance (s);

      VGenerator<double> generator;
      add_value(generator( "val", string("Polarization vector value"),
			   &PolnVector::get_value,
			   &PolnVector::get_nparam ));

      add_value(generator( "var", string("Polarization vector variance"),
			   &PolnVector::get_variance,
			   &PolnVector::get_nparam ));
    }
  };

protected:

  CalibratorStokes* parent;
  unsigned ichan;

};

//! Interface to StokesVector
CalibratorStokes::PolnVector* CalibratorStokes::get_poln (unsigned ichan)
{
  if (!current)
    current = new PolnVector (this);

  current->set_ichan (ichan);
  return current;
}

// Text interface to a CalibratorStokes instance
class CalibratorStokes::Interface : public TextInterface::To<CalibratorStokes>
{
public:
  Interface( CalibratorStokes *s_instance = NULL )
  {
    if (s_instance)
      set_instance (s_instance);

    add( &CalibratorStokes::get_coupling_point,
	 &CalibratorStokes::set_coupling_point,
	 "coupling", "Point at which reference source is coupled" );
    
    import( "p", PolnVector::Interface(),
	    &CalibratorStokes::get_poln,
	    &CalibratorStokes::get_nchan );
  }
};

TextInterface::Parser* CalibratorStokes::get_interface()
{
  return new Interface (this);
}

const string CouplingPoint2string (CalibratorStokes::CouplingPoint point)
{
  switch( point )
  {
  case CalibratorStokes::BeforeProjection: return "BeforeProjection";
  case CalibratorStokes::BeforeBasis: return "BeforeBasis";
  case CalibratorStokes::BeforeFrontend: return "BeforeFrontend";
  case CalibratorStokes::BeforeIdeal: return "BeforeIdeal";
  case CalibratorStokes::BeforeBackend: return "BeforeBackend";
  default: return "Unknown";
  }
}

CalibratorStokes::CouplingPoint string2CouplingPoint (const string& ss)
{
  if( ss=="BeforeProjection" ) return CalibratorStokes::BeforeProjection;
  if( ss=="BeforeBasis" ) return CalibratorStokes::BeforeBasis;
  if( ss=="BeforeFrontend" ) return CalibratorStokes::BeforeFrontend;
  if( ss=="BeforeIdeal" ) return CalibratorStokes::BeforeIdeal;
  if( ss=="BeforeBackend" ) return CalibratorStokes::BeforeBackend;
  if( ss=="Unknown" ) return CalibratorStokes::Unknown;
  
  throw Error (InvalidState, "Pulsar::string2CouplingPoint",
	       "Unknown point '" + ss + "'");
}

std::ostream& Pulsar::operator << (std::ostream& ostr,
				  CalibratorStokes::CouplingPoint point)
{
  return ostr << CouplingPoint2string(point);
}

std::istream& Pulsar::operator >> (std::istream& is,
				   CalibratorStokes::CouplingPoint& point)
{
  return extraction (is, point, string2CouplingPoint);
}
