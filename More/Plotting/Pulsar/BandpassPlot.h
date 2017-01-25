//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2009 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/BandpassPlot.h

#ifndef BANDPASSTABLE_PLOT_H
#define BANDPASSTABLE_PLOT_H

#include "Pulsar/FrequencyPlot.h"
#include <vector>

namespace Pulsar
{
    class BandpassPlot : public FrequencyPlot
    {
        public:

            BandpassPlot();

            ~BandpassPlot();

            class Interface : public TextInterface::To<BandpassPlot>
            {
                public:
                    Interface (BandpassPlot *s_instance = 0);
            };

            TextInterface::Parser *get_interface();

            void prepare(const Pulsar::Archive *);

            virtual void preprocess(Pulsar::Archive* arch) {}

            void draw(const Pulsar::Archive*);

            //! Set the crop fraction
            void set_crop( float new_crop ) { crop = new_crop; }

            //! Get the crop level
            float get_crop() const { return crop; }

            //! Get the passbands vector
            std::vector<std::vector<float> > get_passbands() { return passbands; }

        private:

            std::vector<std::vector<float> > means;

            std::pair<float, float> means_minmax;

            std::vector<std::vector<float> > passbands;

            unsigned passband_npol;

            unsigned passband_nchan;

            float crop;
    };
}

#endif
