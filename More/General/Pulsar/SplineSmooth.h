//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_SplineSmooth_h
#define __Pulsar_SplineSmooth_h

#include "ReferenceAble.h"
#include "Estimate.h"

namespace Pulsar {

  //! Base class of penalized splines (p-spline) for smoothing
  /*! Derived classes interface with the SPLINT library */
  class SplineSmooth : public Reference::Able
  {
    public:

      //! Constructor
      SplineSmooth();

      //! Destructor
      ~SplineSmooth();

      //! Set the smoothing factor
      void set_alpha (double _alpha) { alpha = _alpha; }
      double get_alpha () const { return alpha; }

      //! Get the string that describes the p-spline
      std::string unload () const;

      //! Load the string that describes the p-spline
      void load (const std::string&);

    protected:

      //! evaluate method used by derived types
      double evaluate (const std::vector<double>& xval);

      //! constructor used by derived types
      template<typename T>
      void new_spline (const std::vector<T>& data_x,
                       const std::vector< Estimate<double> >& data_y);

    private:

      //! Smoothing factor
      double alpha;

      class Handle;
      Handle* handle;

  };

  class SplineSmooth1D : public SplineSmooth
  {
    public:

      void set_data (const std::vector< double >& data_x,
                     const std::vector< Estimate<double> >& data_y);

      double evaluate (double x);
  };

  class SplineSmooth2D : public SplineSmooth
  {
    public:

      void set_data (const std::vector< std::pair<double,double> >& data_x,
                     const std::vector< Estimate<double> >& data_y);

      double evaluate ( const std::pair<double,double>& );
  };

}

#endif

