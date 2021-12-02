//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/CompareWith.h

#ifndef __CompareWith_h
#define __CompareWith_h

#include "Pulsar/HasArchive.h"
#include "Pulsar/TimeDomainCovariance.h"
#include "ndArray.h"

class BinaryStatistic;
class UnaryStatistic;

namespace Pulsar {

  //! Loops over two dimensions of an Archive, comparing profiles
  /*! Uses a BinaryStatistic to compare profiles */
  class CompareWith : public Reference::Able
  {
  protected:
    
    //! Used to perform comparison
    Reference::To<BinaryStatistic> statistic;

    //! Interface to the Archive
    HasArchive* data;

    //! Pointer to the method used to loop over the primary dimension
    void (HasArchive::* primary) (Index);
    unsigned nprimary;
    
    //! Pointer to the method used to loop over the compare dimension
    void (HasArchive::* compare) (Index);
    unsigned ncompare;

    //! Transpose indeces when computing results
    bool transpose;

    void set (ndArray<2,double>& result,
	      unsigned iprimary, unsigned icompare,
	      double value);

    //! Check that necessary attributes have been set
    void check ();

    //! Perform an eigenanalysis and set up the generalized chi squared
    virtual void setup (unsigned iprimary);
    
    Reference::To<TimeDomainCovariance> covar;

    //! Compute the comparison summary for primary dimension
    virtual void compute (unsigned iprimary, ndArray<2,double>& result) = 0;

    //! Flags for subset of sub-integrations to be computed
    std::vector<bool> compute_mask;
    
  public:

    CompareWith ();

    void set_statistic (BinaryStatistic*);
    void set_data (HasArchive*);
    void set_transpose (bool);
    void set_primary (unsigned n, void (HasArchive::*) (Index));
    void set_compare (unsigned n, void (HasArchive::*) (Index));

    //! Flags for subset of primary axis to be computed
    void set_compute_mask (const std::vector<bool>& flags)
    { compute_mask = flags; }
			   
    virtual void compute (ndArray<2,double>& result);
  };
}

#endif
