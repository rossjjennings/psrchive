/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#ifndef DYNAMIC_SN_SPECTRUM_H_
#define DYNAMIC_SN_SPECTRUM_H_

#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>
#include <Pulsar/SimplePlot.h>
#include <TextInterface.h>
#include <string>



using std::pair;


namespace Pulsar
{

  //! Another Signal to Noise ratio plot.

  class DynamicSNSpectrum : public Pulsar::SimplePlot
  {
  public:
    DynamicSNSpectrum();

    pair<int,int> get_srange() const { return srange; }
    void set_srange( const pair<int,int> &s_srange ) { srange = s_srange; }
    void set_srange( int fsub, int lsub ) { set_srange( pair<int,int>( fsub, lsub ) ); }

    void prepare( const Pulsar::Archive *data );
    virtual void preprocess( Pulsar::Archive *data ) {}
    void draw( const Pulsar::Archive *data );

  class Interface : public TextInterface::To<DynamicSNSpectrum>
    {
    public:
      Interface( DynamicSNSpectrum *s_instance );
    };

    TextInterface::Parser *get_interface() { return new Interface(this); }

    std::string get_ylabel ( const Pulsar::Archive *data );
    std::string get_xlabel ( const Pulsar::Archive *data );

    void set_pol( int new_pol ) { pol = new_pol; }
    void set_subint( int new_subint ) { isubint = new_subint; }
    int get_pol( void ) const { return pol; }
    int get_subint( void ) const { return isubint; }


  private:
    int isubint;
    int pol;

    pair<int,int> srange;
  };


}


#endif



