//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/RemoveBaseline.h

#ifndef _Pulsar_RemoveBaseline_H
#define _Pulsar_RemoveBaseline_H

#include "Pulsar/Transformation.h"
#include "Pulsar/Archive.h"

namespace Pulsar
{
  //! Algorithms that remove the off-pulse baseline
  class RemoveBaseline : public Transformation<Archive>
  {
  public:

    RemoveBaseline ();

    /* Two simple baseline removal algorithms
       (Not every child of RemoveBaseline needs to be nested.) */

    class Total;
    class Each;

    //! Performs the baseline removal operation
    /*! 
      Given a Profile and its baseline window, this operation
      defines how the baseline is removed
    */
    class Operation : public Reference::Able
    {
    public:
      static Operation* factory (const std::string&);

      virtual void operate (Profile*, const PhaseWeight*) = 0;
    };

    /* Some simple baseline removal operations
       (Not every child of RemoveBaseline::Operation needs to be nested.) */

    class SubtractMean;
    class SubtractMedian;
    class NormalizeByMean;
    class NormalizeByMedian;
    class NormalizeByStdDev;
    class NormalizeByMedAbsDif;

    void set_operation (Operation*);

  protected:
    Reference::To<Operation> profile_operation;
  };

  //! Find the baseline from the total integrated total intensity profile
  /*! This algorithm assumes that both the timing model and the
    dispersion measure accurately reflect the phase of the pulsar in
    every sub-integration and frequency channel. */
  class RemoveBaseline::Total : public RemoveBaseline
  {
  public:

    //! Remove the baseline
    void transform (Archive*);

    //! Remove the baseline
    void operate (Integration*, const PhaseWeight*);
  };

  //! Find the baseline from each total intensity profile
  /*! This algorithm does not assume anything about the timing model
    or the dispersion measure.  The baseline is separately determined
    for each sub-integration and frequency channel. */
  class RemoveBaseline::Each : public RemoveBaseline
  {
  public:

    //! Remove the baseline
    void transform (Archive*);
  };

  class RemoveBaseline::SubtractMean
    : public RemoveBaseline::Operation
  {
  public:
    virtual void operate (Profile*, const PhaseWeight*);
  };

  class RemoveBaseline::SubtractMedian
    : public RemoveBaseline::Operation
  {
  public:
    virtual void operate (Profile*, const PhaseWeight*);
  };

  class RemoveBaseline::NormalizeByMean
    : public RemoveBaseline::Operation
  {
  public:
    virtual void operate (Profile*, const PhaseWeight*);
  };

  class RemoveBaseline::NormalizeByMedian
    : public RemoveBaseline::Operation
  {
  public:
    virtual void operate (Profile*, const PhaseWeight*);
  };

  class RemoveBaseline::NormalizeByStdDev
    : public RemoveBaseline::Operation
  {
  public:
    virtual void operate (Profile*, const PhaseWeight*);
  };

  class RemoveBaseline::NormalizeByMedAbsDif
    : public RemoveBaseline::Operation
  {
  public:
    virtual void operate (Profile*, const PhaseWeight*);
  };

  
}

#endif

