//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Telescopes_h
#define __Telescopes_h

namespace Pulsar {

  class Archive;
  class Telescope;

  //! Namespace contains general info for each telescope
  namespace Telescopes {

    //! Fill the Telescope Extension based on tempo code in Archive.
    void set_telescope_info (Telescope *t, Archive *a);

    //! Initialize the Telescope Extension with GBT info
    void GBT (Telescope* t);

    //! Initialize the Telescope Extension with GB 85-3 info
    void GB85_3 (Telescope* t);

    //! Initialize the Telescope Extension with GB 140ft info
    void GB140 (Telescope* t);

    //! Initialize the Telescope Extension with Arecibo info
    void Arecibo (Telescope* t);

    //! Initialize the Telescope Extension with Nancay info
    void Nancay (Telescope* t);

    //! Initialize the Telescope Extension with Effelsberg info
    void Effelsberg (Telescope* t);

    //! Initialize the Telescope Extension with LOFAR info
    void LOFAR (Telescope* t);

    //! Initialize the Telescope Extension with DE601 info
    void DE601 (Telescope* t);

    //! Initialize the Telescope Extension with DE602 info
    void DE602 (Telescope* t);

    //! Initialize the Telescope Extension with DE603 info
    void DE603 (Telescope* t);

    //! Initialize the Telescope Extension with DE604 info
    void DE604 (Telescope* t);

    //! Initialize the Telescope Extension with DE605 info
    void DE605 (Telescope* t);

    //! Initialize the Telescope Extension with FR606 info
    void FR606 (Telescope* t);

    //! Initialize the Telescope Extension with SE607 info
    void SE607 (Telescope* t);

    //! Initialize the Telescope Extension with UK608 info
    void UK608 (Telescope* t);

    //! Initialize the Telescope Extension with DE609 info
    void DE609 (Telescope* t);

    //! Initialize the Telescope Extension with PL610 info
    void PL610 (Telescope* t);

    //! Initialize the Telescope Extension with PL611 info
    void PL611 (Telescope* t);

    //! Initialize the Telescope Extension with PL612 info
    void PL612 (Telescope* t);

    //! Initialize the Telescope Extension with SE607 info
    void SE607 (Telescope* t);

    //! Initialize the Telescope Extension with IE613 info
    void IE613 (Telescope* t);

    //! Initialize the Telescope Extension with UTR2 info
    void UTR2 (Telescope* t);

    //! Initialize the Telescope Extension with MeerKAT info
    void MeerKAT (Telescope* t);

    //! Initialize the Telescope Extension with MOST info
    void MOST (Telescope* t);

    //! Initialize the Telescope Extension with Parkes info
    void Parkes (Telescope* t);

    //! Initialize the Telescope Extension with Jodrell Bank info
    void Jodrell (Telescope* t);

    //! Initialize the Telescope Extension with Mt Pleasant 26m info
    void MtPleasant26 (Telescope *t);

    //! Initialize the Telescope Extension with WSRT info
    void WSRT (Telescope* t);

    //! Initialize the Telescope Extension with VLA info
    void VLA (Telescope* t);

    //! Initialize the Telescope Extension with SHAO info
    void SHAO (Telescope* t);

    //! Initialize the Telescope Extension with LWA info
    void LWA (Telescope* t);

    //! Initialize the Telescope Extension with CHIME info
    void CHIME (Telescope* t);

    //! Initialize the Telescope Extension with SRT info
    void SRT (Telescope* t);

    //! Initialize the Telescope Extension with NenuFAR info
    void NenuFAR (Telescope* t);

    //! Initialize the Telescope Extension with MWA info
    void MWA (Telescope* t);

  } 

}

#endif
