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
#include "Pulsar/ScrunchFactor.h"

#include "ChiSquared.h"
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

    //! Compare over both dimensions
    bool compare_all;

    //! Compute the covariance matrix of the best-fit residual
    /*! Residual after fitting scale and offset */
    bool model_residual;
    bool use_null_space;
    
    //! Transpose indeces when computing results
    bool transpose;

    //! Perform an eigenanalysis and set up the generalized chi squared
    virtual void setup (unsigned start_primary, unsigned nprimary = 1);

    //! Set true when setup has completed
    bool setup_completed;

    //! Set true when setup has completed during set_setup_data
    bool is_setup;

    void set (ndArray<2,double>& result,
	      unsigned iprimary, unsigned icompare,
	      double value);

    //! Check that necessary attributes have been set
    void check ();

    //! Compute the mean of the profiles to be compared
    void compute_mean (unsigned start_primary, unsigned nprimary);
    Reference::To<Profile> mean;

    Reference::To<TimeDomainCovariance> covar;

    //! Compute covariance matrix from bscrunched clone of data
    ScrunchFactor bscrunch_factor;

    //! Set amps to the normalized and bscrunched profile amplitudes
    void get_amps (std::vector<double>& amps, const Profile* profile);

    //! Set amps to the residual after best fit of amps to mamps
    void get_residual (std::vector<double>& amps,
		       const std::vector<double>& mamps);

    //! After call to get_residual, also stores residual profile amplitudes
    Reference::To<Profile> temp;

    //! Used to compute the residual
    BinaryStatistics::ChiSquared chi;

    //! Compute the comparison summary for primary dimension
    virtual void compute (unsigned iprimary, ndArray<2,double>& result) = 0;

    //! Flags for subset of sub-integrations to be computed
    std::vector<bool> compute_mask;


    //! File to which auxiliary data will be printed
    FILE* fptr;

  public:

    CompareWith ();

    void set_statistic (BinaryStatistic*);
    void set_data (HasArchive*);

    //! Use to compute the covariance matrix before an fscrunch or tscrunch
    void set_setup_data (const Archive*);
    
    //! Compute covariance matrix from bscrunched clone of data
    void set_bscrunch (const ScrunchFactor& f) { bscrunch_factor = f; }
    
    //! Get the phase bin scrunch factor
    const ScrunchFactor get_bscrunch () const { return bscrunch_factor; }

    //! Return true if call to set_setup_data sets anything up
    /* Not all comparisons require a global set up */
    bool get_setup () { return is_setup; }

    void set_transpose (bool);
    void set_primary (unsigned n, void (HasArchive::*) (Index));
    void set_compare (unsigned n, void (HasArchive::*) (Index));

    void set_compare_all (bool flag = true) { compare_all = flag; }
    
    //! Flags for subset of primary axis to be computed
    void set_compute_mask (const std::vector<bool>& flags)
    { compute_mask = flags; }
			   
    virtual void compute (ndArray<2,double>& result);

    //! Set the file to which auxiliary data will be printed
    void set_file (FILE* f) { fptr = f; }

  };
}

#endif
