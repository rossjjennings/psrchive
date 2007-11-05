



#ifndef DYNAMIC_SN_SPECTRUM_H_
#define DYNAMIC_SN_SPECTRUM_H_

#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>
#include <Pulsar/SimplePlot.h>
#include <TextInterface.h>
#include <string>






namespace Pulsar
{
  
  //! - Another Signal to Noise ratio plot.
  
class DynamicSNSpectrum : public Pulsar::SimplePlot
{
public:
  DynamicSNSpectrum();
  
  void set_fsub( int s_fsub ) { fsub = s_fsub; }
  void set_lsub( int s_lsub ) { lsub = s_lsub; }
  
  int get_fsub( void ) const { return fsub; }
  int get_lsub( void ) const { return lsub; }
  
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

  int fsub;
  int lsub;
};


}


#endif



