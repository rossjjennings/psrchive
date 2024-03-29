//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Formats/PSRFITS/Pulsar/ProfileColumn.h

#ifndef __Pulsar_ProfileColumn_h
#define __Pulsar_ProfileColumn_h

#include "Pulsar/Config.h"
#include <fitsio.h>

namespace Pulsar {

  class Profile;

  //! Loads and unloads Profile vector from PSRFITS archives

  class ProfileColumn : public Reference::Able {

  public:

    static Option<bool> output_floats;

    //! Default constructor
    ProfileColumn ();

    //! Set the fitsfile to/from which data are written/read
    void set_fitsfile (fitsfile* fptr);

    //! Set the name of the data column
    void set_data_colname (const std::string&);

    //! Get the index of the data column
    unsigned get_data_colnum ();

    //! Set the name of the offset column
    void set_offset_colname (const std::string&);

    //! Get the index of the data column
    unsigned get_offset_colnum ();
    
    //! Set the name of the scale column
    void set_scale_colname (const std::string&);

    //! Get the index of the data column
    unsigned get_scale_colnum ();

    //! Set the number of phase bins
    void set_nbin (unsigned);

    //! Set the number of frequency channels
    void set_nchan (unsigned);

    //! Set the number of profiles in each frequency channel
    void set_nprof (unsigned);

    //! Resize the columns
    void resize ();

    //! Resize the columns for floating point output
    void resize_floats ();

    //! Get the number of rows required to unload data (after resize)
    unsigned get_nrow () const { return nrow; }
    
    //! Insert the columns and resize
    void create (unsigned start_column);

    //! Unload the given vector of profiles
    void unload (int row, const std::vector<const Profile*>&);

    //! Unload the given vector of profiles as single-precision floats
    void unload_floats (int row, const std::vector<const Profile*>&);

    //! Load the given vector of profiles
    void load (int row, const std::vector<Profile*>&);

    bool verbose;

  protected:

    fitsfile* fptr;

    std::string data_colname;
    std::string offset_colname;
    std::string scale_colname;

    int data_colnum;
    int offset_colnum;
    int scale_colnum;

    unsigned nbin, nchan, nprof;
    unsigned nrow;
    
    //! reset the column indeces
    void reset ();

    //! Get the column number for the specified column name
    int get_colnum (const std::string& name);

    template<typename T, typename C>
    void load_amps (int row, C&, bool must_have_scloffs = true);
  };

}

#endif
