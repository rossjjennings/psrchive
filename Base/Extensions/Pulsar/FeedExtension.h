//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FeedExtension.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:41:06 $
   $Author: straten $ */

#ifndef __FeedExtension_h
#define __FeedExtension_h

#include "Pulsar/Archive.h"
#include "Jones.h"

namespace Pulsar {

  //! Stores a known feed transformation
  /*! The FeedExtension class is used to add an arbitrary transformation to
    a PolnCalibrator instance.  The transformation is included between the
    backend and the receiver platform. */
  class FeedExtension : public Archive::Extension {

  public:
    
    //! Default constructor
    FeedExtension ();

    //! Copy constructor
    FeedExtension (const FeedExtension&);

    //! Operator =
    const FeedExtension& operator= (const FeedExtension&);

    //! Destructor
    ~FeedExtension ();

    //! Clone method
    FeedExtension* clone () const { return new FeedExtension( *this ); }

    //! Return the transformation
    Jones<double> get_transformation () const { return transformation; }

    //! Set the transformation
    void set_transformation (const Jones<double>& x) { transformation = x; }

  protected:

    Jones<double> transformation;

  };
 

}

#endif
