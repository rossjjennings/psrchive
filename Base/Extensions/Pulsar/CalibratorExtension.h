//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/CalibratorExtension.h

#ifndef __CalibratorExtension_h
#define __CalibratorExtension_h

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/CalibratorType.h"

namespace Pulsar {

  //! Stores Calibrator parameters in an Archive
  /*! This Archive::Extension class implements the storage of Calibrator
    data, enabling a calibrator to be constructed from file. */
  class CalibratorExtension : public Archive::Extension
  {   
  public:
    
    //! Default constructor
    CalibratorExtension (const char* name = "CalibratorExtension");

    //! Copy constructor
    CalibratorExtension (const CalibratorExtension&);

    //! Operator =
    const CalibratorExtension& operator= (const CalibratorExtension&);

    //! Destructor
    ~CalibratorExtension ();

    //! Clone method
    CalibratorExtension* clone () const
    { return new CalibratorExtension( *this ); }

    //! Construct from a Calibrator instance
    void build (const Calibrator*);

    //! Set the type of the calibrator
    virtual void set_type (const Calibrator::Type* type);
    //! Get the type of the calibrator
    virtual const Calibrator::Type* get_type () const;

    //! Set the reference epoch of the calibration experiment
    virtual void set_epoch (const MJD& epoch);
    //! Get the reference epoch of the calibration experiment
    virtual MJD get_epoch () const;

    //! Set the number of frequency channels
    virtual void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    virtual unsigned get_nchan () const;

    //! Remove the inclusive range of channels
    virtual void remove_chan (unsigned first, unsigned last);

    //! Set the weight of the specified channel
    virtual void set_weight (unsigned ichan, float weight);
    //! Get the weight of the specified channel
    virtual float get_weight (unsigned ichan) const;

    //! Set the centre frequency of the specified channel
    virtual void set_centre_frequency (unsigned ichan, double frequency);
    //! Get the centre frequency of the specified channel
    virtual double get_centre_frequency (unsigned ichan) const;

    //! Return a short name
    std::string get_short_name () const { return "cal"; }

    class Interface;

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    //! Append Calibrator Extension data from another Archive
    void frequency_append (Archive* to, const Archive* from);

    protected:

    //! Type of the calibrator
    Reference::To<const Calibrator::Type> type;
    
    //! The reference epoch of the calibration experiment
    MJD epoch;

    //! The weight of each channel
    std::vector<float> weight;

    //! The centre frequency of each channel
    std::vector<double> centre_frequency;

    //! Ensure that ichan < get_nchan
    void range_check (unsigned ichan, const char* method) const;

    //! For use by children
    void frequency_append (const CalibratorExtension*, bool in_order);

  };
 

}

#endif
