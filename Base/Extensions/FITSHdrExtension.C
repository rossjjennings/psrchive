#include "Pulsar/FITSHdrExtension.h"

//! Default constructor
Pulsar::FITSHdrExtension::FITSHdrExtension ()
{
  start_time.Construct("00000.00000");
  hdrver = "UNSET";
  creation_date = "UNSET";
  coordmode = "UNSET";
  trk_mode = "UNSET";
  stt_date = "UNSET";
  stt_time = "UNSET";
  stt_lst = 0.0;
}

//! Copy constructor
Pulsar::FITSHdrExtension::FITSHdrExtension (const FITSHdrExtension& extension)
{
  start_time = extension.start_time;
  hdrver = extension.hdrver;
  creation_date = extension.creation_date;
  coordmode = extension.coordmode;
  trk_mode = extension.trk_mode;
  stt_date = extension.stt_date;
  stt_time = extension.stt_time;
  stt_lst = extension.stt_lst;
}

//! Operator =
const Pulsar::FITSHdrExtension&
Pulsar::FITSHdrExtension::operator= (const FITSHdrExtension& extension)
{
  start_time = extension.start_time;
  hdrver = extension.hdrver;
  creation_date = extension.creation_date;
  coordmode = extension.coordmode;
  trk_mode = extension.trk_mode;
  stt_date = extension.stt_date;
  stt_time = extension.stt_time;
  stt_lst = extension.stt_lst;
  
  return *this;
}

//! Destructor
Pulsar::FITSHdrExtension::~FITSHdrExtension ()
{
}

