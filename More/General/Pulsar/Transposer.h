//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Transposer.h,v $
   $Revision: 1.1 $
   $Date: 2004/07/16 07:27:47 $
   $Author: straten $ */

#ifndef __Pulsar_Transposer_h
#define __Pulsar_Transposer_h

#include "Reference.h"
#include "Types.h"

namespace Pulsar {
  
  class Archive;
  class Integration;
  class Profile;
  class Dimensions;

  //! Provides access to transposed Archive data
  class Transposer {

  public:

    //! Default constructor
    Transposer (const Archive* archive = 0);

    //! Destructor
    ~Transposer ();

    //! Set the Archive from which data will be read
    void set_Archive (const Pulsar::Archive* archive);

    //! Set the first dimension
    void set_dim1 (Signal::Dimension dim1) { x1 = dim1; }

    //! Set the second dimension
    void set_dim2 (Signal::Dimension dim2) { x2 = dim2; }

    //! Set the third dimension
    void set_dim3 (Signal::Dimension dim3) { x3 = dim3; }

    //! Returns amplitude data; ordered according to the specified dimension
    void get_amps (vector<float>& amps) const;

    //! verbosity flag
    bool verbose;

  protected:

    void get_amps (const Integration* , float* , const Dimensions& dim) const;
    void get_amps (const Profile* , float* , unsigned jbin) const;

    Reference::To<const Archive> archive;
    Signal::Dimension x1;
    Signal::Dimension x2;
    Signal::Dimension x3;

  };

  //! Provides enumerated access to the dimensions of an Archive
  class Dimensions {

  public:

    //! Default constructor
    Dimensions (const Pulsar::Archive* archive = 0);
    
    //! Get the size of the specified dimension
    unsigned get_ndim (Signal::Dimension axis) const;
    
  protected:

    friend class Transposer;

    //! Set the size of the specified dimension
    void set_ndim (Signal::Dimension axis, unsigned ndim);

    //! Set the size of the remaining dimension
    void set_if_zero (unsigned ndim);

    unsigned nsub;
    unsigned npol;
    unsigned nchan;
    unsigned nbin;

  };

}

#endif
