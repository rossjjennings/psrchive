#include <iostream>

#include "qt_fileParams.h"

#include "psrParams.h"
#include "psr_cpp.h"

bool qt_fileParams::verbose = false;

qt_fileParams::qt_fileParams ( const QString& startname, QWidget* parent ) 
  : QFileDialog (parent, "EphFileDialog", true )
{
  setSelection (startname);

  QString intro ("TEMPO PSR Parameter Files (*");
  QString close (")");

  QStringList filter;
 
  vector<string> ephext = psrParams::extensions ();
  for (vector<string>::iterator str = ephext.begin();
       str != ephext.end(); str ++)
    filter.append ( intro + QString( str->c_str() ) + close );
  filter.append ( "Any File (*.*)" );

  setFilters ( filter );

  connect ( this, SIGNAL ( fileSelected (const QString&) ), 
	    this, SLOT ( chosen (const QString&) ) );
}

int qt_fileParams::open (psrParams* eph)
{
  if (eph == NULL)
    return -1;

  if (verbose)
    cerr << "qt_fileParams::open exec" << endl;

  setMode (QFileDialog::ExistingFile);
  if ( exec () != 1 || fileName.empty() )
    return 0;

  if (verbose)
    cerr << "qt_fileParams::open '" << fileName << "'" << endl;

  eph -> load (fileName);
  cerr << "RETURN FROM psrParms::load (string filename)" << endl;

  if (eph->empty())
    return 0;
  return 1;
}

int qt_fileParams::save (const psrParams& eph)
{
  setMode (QFileDialog::AnyFile);
  if ( exec () != 1 || fileName.empty() )
    return 0;

  if (verbose)
    cerr << "qt_fileParams::save to '" << fileName << "'" << endl;

  eph.unload (fileName);
  return 1;
}

void qt_fileParams::chosen ( const QString& name )
{
  fileName = name.ascii();
  if (verbose)
    cerr << "qt_fileParams::chosen '" << fileName << "'" << endl;
}
