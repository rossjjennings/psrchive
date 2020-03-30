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

#include <bspline.h>

namespace Pulsar {

  class Profile;

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
      void set_alpha (double);
      double get_alpha () const { return alpha; }

      //! Get the string that describes the p-spline
      virtual std::string unload () const = 0;

      //! Load the string that describes the p-spline
      virtual void load (const std::string&) = 0;

    protected:

      //! Smoothing factor
      double alpha;

      template<typename T>
      SPLINTER::BSpline make_spline (const std::vector<T>& data_x,
                           const std::vector< Estimate<double> >& data_y);

  };

  class SplineSmooth1D : public SplineSmooth
  {
    public:

      void set_data (const std::vector< double >& data_x,
                     const std::vector< Estimate<double> >& data_y);

      double evaluate (double x);

      //! Get the string that describes the p-spline
      std::string unload () const;

      //! Load the string that describes the p-spline
      void load (const std::string&);

    private:
      SPLINTER::BSpline pspline;
  };

  class SplineSmooth2D : public SplineSmooth
  {
    public:

      void set_data (const std::vector< std::pair<double,double> >& data_x,
                     const std::vector< Estimate<double> >& data_y);

      double evaluate (double x);
      
      //! Get the string that describes the p-spline
      std::string unload () const;

      //! Load the string that describes the p-spline
      void load (const std::string&);

    private:
      SPLINTER::BSpline pspline;
  };

}

#endif
