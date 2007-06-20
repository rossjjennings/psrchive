//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// Class for holding digitiser statistics

#ifndef __DigitiserStatistics_h
#define __DigitiserStatistics_h

#include "Pulsar/Archive.h"

namespace Pulsar
{

  class DigitiserStatistics : public Pulsar::Archive::Extension
  {

  public:

    //Default constructor
    DigitiserStatistics ();

    // Copy constructor
    DigitiserStatistics (const DigitiserStatistics& extension);

    // Operator =
    const DigitiserStatistics& operator= (const DigitiserStatistics&);

    // Destructor
    ~DigitiserStatistics ();

    //! Clone method
    DigitiserStatistics* clone () const
      { return new DigitiserStatistics( *this ); }

    //! Return a text interfaces that can be used to access this instance
    Reference::To< TextInterface::Class > get_text_interface();

    //! set ndigr
    void set_ndigr( int s_ndigr ) { ndigr = s_ndigr; }

    //! get_ndigr
    int get_ndigr( void ) const { return ndigr; }

    //! set npar
    void set_npar( int s_npar ) { npar = s_npar; }

    //! get npar
    int get_npar( void ) const { return npar; }

    //! set ncycsub
    void set_ncycsub( int s_ncycsub ) { ncycsub = s_ncycsub; }

    //! get ncycsub
    int get_ncycsub( void ) const { return ncycsub; }

    //! set diglev
    void set_diglev( std::string s_diglev ) { diglev = s_diglev; }

    //! get diglev
    std::string get_diglev( void ) const { return diglev; }





    ///////////////////////////////////////////////////////////////
    // The information container

    class row
    {

    public:

      row () { init(); }
      ~row ();

      int index;

      std::string dig_mode;
      int ndigr;
      int nlev;
      int ncycsub;
      std::string diglev;

      std::vector<float> data;

    private:

      void init ();

    };

    std::vector<row> rows;

    DigitiserStatistics::row& get_row (unsigned i);
    DigitiserStatistics::row& get_last ();

    void push_blank_row ();

  private:

    int ndigr;
    int npar;
    int ncycsub;
    std::string diglev;

    void init ();

  };

}

#endif
