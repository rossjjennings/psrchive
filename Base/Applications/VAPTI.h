/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#ifndef __VAP_TI_h
#define __VAP_TI_h



#include <TextInterface.h>
#include <Pulsar/Archive.h>
#include <Pulsar/ArchiveTI.h>
#include <Pulsar/FITSSUBHdrExtension.h>
#include <Pulsar/Pointing.h>
#include <Pulsar/Integration.h>
#include <string>



using namespace std;




namespace Pulsar
{
  template< class T > class TIBase : public TextInterface::To< T >
  {
  public:
    TIBase() { setup(); }
    TIBase( T *c ) { setup(); set_instance( c ); }
    virtual  ~TIBase() {}

    void setup( void ) {}

    TextInterface::Class *clone()
    {
      if( this->instance )
        return new TIBase( this->instance );
      else
        return new TIBase();
    }
  private:
  };


  class VAPTI : public TIBase< Archive >
  {
  public:
    void setup( void );
  };


  class VapArchiveTI : public ArchiveTI
  {
  public:
    VapArchiveTI() { setup(); setup_exceptions(); }
    VapArchiveTI( Archive *c ) { setup(); setup_exceptions(); this->set_instance( c ); }

    void setup_exceptions( void );

    string get_length( void );
    string get_bw( void );
    string get_para( void );
    string get_tsub( void );

    virtual string process( const string &command );
  private:

  };

}



#endif



