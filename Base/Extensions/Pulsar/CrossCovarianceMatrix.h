//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Aditya Parthasarathy & Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/CrossCovarianceMatrix.h

#ifndef __CrossCovarianceMatrix_h
#define __CrossCovarianceMatrix_h

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/Profile.h"

#include <vector> 

namespace Pulsar {

  class CrossCovarianceMatrix : public Pulsar::Archive::Extension
  {

  public:
 
    //! Default constructor
    CrossCovarianceMatrix ();

    //! Copy constructor
    CrossCovarianceMatrix (const CrossCovarianceMatrix& extension);  

    //! Assignment operator
    const CrossCovarianceMatrix& operator= (const CrossCovarianceMatrix&);

    //!Destructor
    ~CrossCovarianceMatrix ();

    //! Clone method
    CrossCovarianceMatrix* clone () const 
    { return new CrossCovarianceMatrix( *this ); }

    //! Set the number of phase bins
    void set_nbin (unsigned);
    
    //! Get the number of phase bins
    unsigned get_nbin () const;

    //! Set the number of polarizations
    void set_npol (unsigned);
    
    //! Get the number of polarizations
    unsigned get_npol () const;

    //! Set the number of lags
    void set_nlag (unsigned);

    //! Get the number of lags
    unsigned get_nlag () const;

    //! Get the number of cross-covariances for the specified lag
    unsigned get_ncross (unsigned ilag) const;

    //! Get the total number of cross-covariances stored for all lags
    unsigned get_ncross_total () const;

    class Stream;

    //! Return true if covariances are stored in a stream of blocks
    bool has_stream () const;

    //! Set the stream of covariances stored in blocks
    void set_stream (Stream*);

    //! Get the stream of covariances stored in blocks
    const Stream* get_stream () const;

    //! Resize the data array according to nbin, npol, and nlag
    void resize_data ();
    
    //! Get the covariance matrix data
    std::vector<double>& get_data () { return covariance; }
    
    //! Get the covariance matrix data
    const std::vector<double>& get_data () const { return covariance; }
    
  protected:

    unsigned nbin;
    unsigned npol;
    unsigned nlag;

    std::vector<double> covariance;

    Reference::To<Stream> stream;
            
  };

  //! Pure virtual base class of a stream of covariances stored in blocks
  class CrossCovarianceMatrix::Stream : public Reference::Able
  {
    public:
      typedef double Type;

      virtual unsigned get_ndat () const = 0;
      virtual void get_data (unsigned off, unsigned n, double*) const = 0;
      virtual void verify_end_of_data () const = 0;
  };

}

#endif

