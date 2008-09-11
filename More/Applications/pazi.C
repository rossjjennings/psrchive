/***************************************************************************
 *
 *   Copyright (C) 2007 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <cpgplot.h>
#include <unistd.h>
#include <string.h>

#include "Pulsar/psrchive.h"
#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"
#include "Pulsar/Archive.h"
#include "Pulsar/ProcHistory.h"

#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/LawnMower.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include "TextInterface.h"
#include "strutil.h"
#include "BoxMuller.h"
#include <vector>

#define TIME 0
#define FREQ 1
#define SUBINT 2

void usage()
{
    std::cout << std::endl <<

        "A user-interactive program for zapping subints, channels and bins.\n"
        "Usage: pazi [options] filename\n\n"
        "Options.\n"
        "  -h                         This help page.\n\n"
        "The following are the possible mouse and keyboard commands.\n"
        "  zoom:                      left click twice\n"
        "  zap:                       right click\n"
        "  zap (multiple):            left click and 'z'\n"
        "  reset zoom:                'r'\n"
        "  frequency:                 'f'\n"
        "  time:                      't'\n"
        "  save (<filename>.pazi):    's'\n"
        "  quit:                      'q'\n"
        "  print paz command:         'p'\n"
        "  center pulse:              'c'\n"
        "  undo last:                 'u'\n"
        "  toggle dedispersion:       'd'\n"
        "  binzap:                    'b'\n"
        "  mow the lawn:              'm'\n"
        "\n"

        "See "PSRCHIVE_HTTP"/manuals/pazi for more details\n" << std::endl;
}

struct archive_data {
    uint nchan;
    uint nsub;
    uint nbin;
    double centre_freq;
    double original_bandwidth;
    double bandwidth;
    double integration_length;
    bool zoommed;
    std::string time_scale;
};

int freq_get_channel(struct archive_data& data, float mY);
int time_get_channel(struct archive_data& data, float mY);
std::string join_option(float y, float y2, double bandwidth, int type);
void zap_channel(Pulsar::Archive* arch, const int chan_to_zap);
void unzap_channel(Pulsar::Archive* archive, Pulsar::Archive* backup, 
        const int zap_chan);

void zap_subint(Pulsar::Archive* archive, const int zap_subint);
void unzap_subint(Pulsar::Archive* archive, Pulsar::Archive* backup,
        int subint_to_unzap);

void redraw(const Pulsar::Archive* arch, Pulsar::Plot* orig_plot, 
        Pulsar::Plot* mod_plot, const bool zoom);

void freq_redraw(Pulsar::Archive* arch, const Pulsar::Archive* old_arch,
        Pulsar::Plot* orig_plot, Pulsar::Plot* mod_plot, const bool zoom);

void time_redraw(Pulsar::Archive* arch, const Pulsar::Archive* old_arch, 
        Pulsar::Plot* orig_plot, Pulsar::Plot* mod_plot, const bool zoom);

void update_total(Pulsar::Archive* arch, const Pulsar::Archive* old_arch,
        Pulsar::Plot* plot);

void get_limits(int& lower, int& upper);
void swap(int &chan1, int &chan2);
void add_channel(std::vector<int>& delete_channels, const int chan);
void remove_channel(std::vector<int>& delete_channels, const int chan);

void print_command(const std::vector<int>& chans, 
        const std::vector<int>& subints, std::string extension, 
        std::string filename);

void set_dedispersion(Pulsar::Archive* arch, const Pulsar::Archive* old_arch);

//void set_centre(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &centered, int type, bool dedispersed);
std::string get_scale(const Pulsar::Archive* arch);

std::string vertical_join_option(const float x, const float x2, int &upper,
        int &lower, const int nbin);

void binzap(Pulsar::Archive* arch, Pulsar::Archive* old_arch, int subint,
        int lower_range, int upper_range, int lower_bin, int upper_bin);

void mowlawn(Pulsar::Archive* arch, Pulsar::Archive* old_arch, 
        const int subint);

void build_archive_data(struct archive_data* data, 
        const Pulsar::Archive* archive);

static bool dedispersed = true;
static std::vector<int> channels_to_zap;
static std::vector<int> subints_to_zap;
static std::vector<int> bins_to_zap;
static std::vector<int> subints_to_mow;

int main(int argc, char* argv[]) try
{
    if (argc < 2) {
        usage();
        return EXIT_SUCCESS;
    }

    int gotc = 0;
    while ((gotc = getopt(argc, argv, "hvV")) != -1) {
        switch (gotc) {
            case 'h':
                usage();
                return EXIT_SUCCESS;
            case 'V':
                Pulsar::Archive::set_verbosity(3);
                break;
            case 'v':
                Pulsar::Archive::set_verbosity(2);
                break;
        }
    }

    if (optind >= argc)
    {
        std::cerr << "pazi: please specify filename" << std::endl;
        return -1;
    }

    float mX = 0, mY = 0;
    float mX2, mY2;
    int mY_chan;
    int mY2_chan;
    int lower, upper;
    int subint = 0;
    char ch;
    bool zoomed = false;
    bool fscrunched = true;
    std::string filename = tostring(argv[optind]);
    std::string extension = filename.substr(filename.length() - 2, 2);
    int plot_type = TIME; // 0 = time, 1 = freq, 2 = subint

    if ((extension != "rf") && (extension != "cf")) {
        extension = "pazi";
    } else {
        if (extension == "rf") {
            extension = "rz";
        } else {
            extension = "cz";
        }
    }

    std::string write_filename = filename + "." + extension;

    std::cerr << "pazi: loading data" << std::endl;
    Reference::To<Pulsar::Archive> base_archive =
        Pulsar::Archive::load(filename);

    struct archive_data data;
    build_archive_data(&data, base_archive);

    if (base_archive->get_npol() == 4)
        base_archive->convert_state(Signal::Stokes);

    Reference::To<Pulsar::Archive> backup_archive = base_archive->clone();
    std::cerr << "pazi: making fscrunched clone" << std::endl;
    Reference::To<Pulsar::Archive> mod_archive = base_archive->clone();
    mod_archive->pscrunch();
    mod_archive->fscrunch();
    mod_archive->dedisperse();
    mod_archive->remove_baseline();

    Reference::To<Pulsar::Archive> scrunched_archive = mod_archive->clone();
    scrunched_archive->tscrunch();

    int upper_range = data.nbin - 1;
    int lower_range = 0;

    Pulsar::PlotFactory factory;
    Pulsar::Plot* time_orig_plot = factory.construct("time");
    Pulsar::Plot* time_mod_plot = factory.construct("time");
    TextInterface::Parser* time_fui = time_mod_plot->get_frame_interface();

    Pulsar::Plot* freq_orig_plot = factory.construct("freq");
    Pulsar::Plot* freq_mod_plot = factory.construct("freq");
    TextInterface::Parser* freq_fui = freq_mod_plot->get_frame_interface();

    Pulsar::Plot* total_plot = factory.construct("flux");
    total_plot->configure("info=1");

    Pulsar::Plot* subint_orig_plot = factory.construct("flux");
    Pulsar::Plot* subint_mod_plot = factory.construct("flux");
    TextInterface::Parser* subint_fui = subint_mod_plot->get_frame_interface();
    subint_orig_plot->configure("info=1");
    subint_mod_plot->configure("info=1");

    cpgopen("1/XS");
    cpgopen("2/XS");
    cpgask(0);

    std::cerr << std::endl << "Total S/N = " << 
        scrunched_archive->get_Profile(0, 0, 0)->snr() << std::endl <<
        std::endl;

    total_plot->plot(scrunched_archive);
    cpgslct(1);
    time_orig_plot->plot(mod_archive);

     while (ch != 'q') {
        if (plot_type == SUBINT)
            cpgband(4, 0, mX, mY, &mX2, &mY2, &ch);
        else
            cpgband(3, 0, mX, mY, &mX2, &mY2, &ch);

        switch (ch) {
            case 'A': // zoom
                if (plot_type == SUBINT) {
                    if (!mX) {
                        mX = mX2;
                    } else {
                        zoomed = true;
                        subint_fui->set_value("x:win",
                                vertical_join_option(mX, mX2,
                                upper_range, lower_range, data.nbin));

                        redraw(mod_archive, subint_orig_plot, subint_mod_plot,
                                zoomed);

                        mX = 0;
                    }
                } else {
                    if (!mY) {
                        mY = mY2;
                    } else {
                        if (plot_type == FREQ) {
                            mY_chan = freq_get_channel(data, mY);
                            mY2_chan = freq_get_channel(data, mY2);
                        } else {
                            mY_chan = time_get_channel(data, mY);
                            mY2_chan = time_get_channel(data, mY2);
                        }
                        if ((mY_chan - mY2_chan < -1) || 
                                (mY_chan - mY2_chan > 1)) {

                            zoomed = true;
                            if (plot_type == FREQ) {
                                freq_fui->set_value("y:win", join_option(mY,
                                            mY2, data.original_bandwidth,
                                            plot_type));

                                redraw(mod_archive, freq_orig_plot,
                                        freq_mod_plot, zoomed);

                            } else {
                                time_fui->set_value("y:win", join_option(mY,
                                            mY2, data.bandwidth, plot_type));

                                redraw(mod_archive, time_orig_plot,
                                        time_mod_plot, zoomed);
                            }
                        }
                        mY = 0;
                    }
                } break;
            case 'h':
                usage();
                break;

            case 'b': // plot specific subint
                if (plot_type == TIME) {
                    //centered = false;
                    zoomed = false;
                    mX = 0;
                    plot_type = SUBINT;
                    mod_archive = base_archive->clone();
                    mod_archive->set_dispersion_measure(0);
                    mod_archive->fscrunch();
                    mod_archive->pscrunch();
                    mod_archive->remove_baseline();
                    subint = time_get_channel(data, mY2);

                    char add[3];
                    sprintf(add, "%d", subint);
                    std::string subint_option = "subint=";
                    subint_option += add;

                    subint_orig_plot->configure(subint_option);
                    subint_mod_plot->configure(subint_option);

                    cpgeras();
                    subint_orig_plot->plot(mod_archive);
                    update_total(scrunched_archive, base_archive, total_plot);
                } break;

            /*case 'c': // center pulse
                set_centre(mod_archive, base_archive, centered, plot_type, dedispersed);

                if (plot_type == "freq")
                    redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
                else if (plot_type == "time")
                    redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);

                update_total(scrunched_archive, base_archive, total_plot);
                break;*/

            case 'd': // toggle dedispersion on/off
                set_dedispersion(mod_archive, base_archive);
                if (plot_type == FREQ) {
                    mod_archive->tscrunch();
                    redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
                } else if (plot_type == TIME) {
                    mod_archive->fscrunch();
                    redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
                }
                update_total(scrunched_archive, base_archive, total_plot);
                break;
            case 'f': // frequency plot
                mY = 0;
                plot_type = FREQ;
                zoomed = false;
                freq_redraw(mod_archive, base_archive, freq_orig_plot,
                        freq_mod_plot, zoomed);
                break;
            case 'm':
                if (plot_type != SUBINT) {
                    std::cerr << "pazi: can only mow lawn of subint" <<
                        std::endl;
                        
                    continue;
                }
          
                std::cerr << "pazi: mowing lawn" << std::endl;
                mowlawn (mod_archive, base_archive, subint);

                std::cerr << "pazi: replotting" << std::endl;
                redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
                std::cerr << "pazi: updating total" << std::endl;
                update_total(scrunched_archive, base_archive, total_plot);
                mX = 0;
                break;
            case 'o': // toggle frequency scrunching on/off
                if (plot_type == TIME) {
                    if (fscrunched) {
                        fscrunched = false;
                        mod_archive = base_archive->clone();
                    } else {
                        fscrunched = true;
                        mod_archive->fscrunch();
                    }
                    redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
                }
                break;
            case 'q': // quit
                cpgclos();
                return EXIT_SUCCESS;
            case 'p':
                print_command(channels_to_zap, subints_to_zap, extension,
                        filename);
                break;
            case 'r': // reset zoom
                zoomed = false;
                mY = 0;
                mX = 0;
                if (plot_type == FREQ) {
                    redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
                } else if (plot_type == TIME) {
                    redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
                } else {
                    upper_range = base_archive->get_nbin() - 1;
                    lower_range = 0;
                    redraw(mod_archive, subint_orig_plot, subint_mod_plot,
                            zoomed);
                }
                break;
            case 's': {// save current archive changes
                Pulsar::ProcHistory* fitsext = 
                    base_archive->get<Pulsar::ProcHistory>();

                fitsext->set_command_str("pazi");
                base_archive->unload(write_filename);
            } break;
            case 't': // time plot
                mY = 0;
                plot_type = TIME;
                zoomed = false;
                time_redraw(mod_archive, base_archive, time_orig_plot,
                        time_mod_plot, zoomed);
                break;
            case 'u': // undo last change
                if (mY) {
                    mY = 0;
                } else {
                    if (plot_type == FREQ) {
                        const int channel = freq_get_channel(data, mY2);
                        remove_channel(channels_to_zap, channel);
                        unzap_channel(base_archive, backup_archive, channel);

                        freq_redraw(mod_archive, base_archive, freq_orig_plot,
                                freq_mod_plot, zoomed);

                    } else if (plot_type == TIME) {
                        const int subint = time_get_channel(data, mY2);
                        remove_channel(subints_to_zap, subint);

                        unzap_subint(base_archive, backup_archive, subint);
                        time_redraw(mod_archive, base_archive, time_orig_plot,
                                time_mod_plot, zoomed);

                    } else { // this most likely doesn't work as desired...
                        if (bins_to_zap.size()) { 
                            bins_to_zap.erase(bins_to_zap.end() - 5, 
                                    bins_to_zap.end());

                            base_archive = backup_archive->clone();
                            mod_archive = backup_archive->clone();

                            mod_archive->set_dispersion_measure(0);
                            mod_archive->pscrunch();
                            mod_archive->fscrunch();
                            mod_archive->remove_baseline();
                            redraw(mod_archive, subint_orig_plot,
                                    subint_mod_plot, zoomed);
                        }
                    }
                }
                update_total(scrunched_archive, base_archive, total_plot);
                break;
            case 'X': // zap single channel
                if (mY || mX) {
                    mY = 0;
                    mX = 0;
                } else {
                    *backup_archive = *base_archive;
                    if (plot_type == FREQ) {
                        const int channel = freq_get_channel(data, mY2);
                        add_channel(channels_to_zap, channel);
                        zap_channel(base_archive, channel);
                        freq_redraw(mod_archive, base_archive, freq_orig_plot,
                                freq_mod_plot, zoomed);

                    } else if (plot_type == TIME) {
                        const int subint = time_get_channel(data, mY2);
                        add_channel(subints_to_zap, subint);
                        zap_subint(base_archive, subint);
                        time_redraw(mod_archive, base_archive, time_orig_plot,
                                time_mod_plot, zoomed);

                    } else {
                        if (((upper_range + 1 == data.nbin) 
                                    && (upper_range - 1 - lower_range > 0)) || 
                                ((upper_range + 1 != data.nbin) && 
                                 (upper_range - lower_range > 0))) {

                            int bin = (int)(mX2 * data.nbin);
                            binzap(mod_archive, base_archive, subint, 
                                    lower_range, upper_range, bin, bin+1);

                            redraw(mod_archive, subint_orig_plot, 
                                    subint_mod_plot, zoomed);
                        }
                    }
                    update_total(scrunched_archive, base_archive, total_plot);
                }
                break;
            case 'z': // zap multiple channels
                if (plot_type == SUBINT) {
                    if (mX) {
                        binzap(mod_archive, base_archive, subint, lower_range,
                                upper_range, (int)(mX * data.nbin), 
                                (int)(mX2 * data.nbin));

                        redraw(mod_archive, subint_orig_plot, subint_mod_plot, 
                                zoomed);

                        update_total(scrunched_archive, base_archive,
                                total_plot);

                        mX = 0;
                    }
                } else {
                    backup_archive = base_archive->clone();
                    if (mY) {
                        get_limits(lower, upper);
                        if (plot_type == FREQ) {
                            for (int ichan = lower; ichan <= upper; ++ichan) {
                                channels_to_zap.push_back(ichan);
                                zap_channel(base_archive, ichan);
                            }
                            freq_redraw(mod_archive, base_archive, 
                                    freq_orig_plot, freq_mod_plot, zoomed);

                        } else {
                            for (int isub = lower; isub <= upper; ++isub) {
                                subints_to_zap.push_back(isub);
                                zap_subint(base_archive, isub);
                            }
                            time_redraw(mod_archive, base_archive, 
                                    time_orig_plot, time_mod_plot, zoomed);
                        }
                        mY = 0;
                    }
                }
                update_total(scrunched_archive, base_archive, total_plot);
                break;
        }
     }
     return 0;
} catch (Error& error) {
    std::cerr << "pazi: " << error << std::endl;
    return -1;
}

int freq_get_channel(struct archive_data& data, float mY)
{
    float channel = ((mY - (float)data.centre_freq - ((float)data.bandwidth
                    / 2)) / (float)data.bandwidth) * data.nchan;

    return data.nchan - (int)fabs(channel) - 1;
}

int time_get_channel(struct archive_data& data, float mY)
{
    if (data.time_scale == "days")
        return (int)((mY * 60 * 60 * 24) / data.integration_length *
                data.nsub);

    else if (data.time_scale == "hours")
        return (int)((mY * 60 * 60) / data.integration_length * data.nsub);
    else if (data.time_scale == "minutes")
        return (int)((mY * 60) / data.integration_length * data.nsub);
    else
        return (int)(mY / data.integration_length * data.nsub);
}

std::string join_option(float y, float y2, double bw, int plot_type)
{
    std::string option = "(";
    char add[5];
    if ((plot_type == 0 && y > y2) || (plot_type == 1 && y > y2 && bw > 0) || 
            (plot_type == FREQ && y < y2 && bw < 0.0)) {

        sprintf(add, "%2.2f", y2);
        option += add;
        option += ",";
        sprintf(add, "%2.2f", y);
        option += add;
        option += ")";
    } else {
        sprintf(add, "%2.2f", y);
        option += add;
        option += ",";
        sprintf(add, "%2.2f", y2);
        option += add;
        option += ")";
    }
    return option;
}

std::string vertical_join_option(const float x, const float x2, int &upper,
        int &lower, const int nbin)
{
    std::string option = "(";
    char add[3];
    if (x > x2) {
        sprintf(add, "%2.2f", x2);
        option += add;
        option += ",";
        sprintf(add, "%2.2f", x);
        option += add;
        option += ")";
        upper = (int)(x * nbin);
        lower = (int)(x2 * nbin);
    } else {
        sprintf(add, "%2.2f", x);
        option += add;
        option += ",";
        sprintf(add, "%2.2f", x2);
        option += add;
        option += ")";
        upper = (int)(x2 * nbin);
        lower = (int)(x * nbin);
    }
    return option;
}

void zap_channel(Pulsar::Archive* archive, const int chan_to_zap)
{
    const uint npol = archive->get_npol();
    const uint nsub = archive->get_nsubint();
    const uint nchan = archive->get_nchan();

    for (uint ipol = 0; ipol < npol; ++ipol) {
        for (uint isub = 0; isub < nsub; ++isub) {
            for (uint ichan = 0; ichan < nchan; ++ichan) {
                if (ichan == chan_to_zap)
                    archive->get_Profile(isub, ipol, ichan)->set_weight(0);
            }
        }
    }
}

void zap_subint(Pulsar::Archive* archive, const int subint_to_zap)
{
    const uint npol = archive->get_npol();
    const uint nchan = archive->get_nchan();

    for (uint ipol = 0; ipol < npol; ++ipol) {
        for (uint ichan = 0; ichan < nchan; ++ichan) {
            archive->get_Profile(subint_to_zap, ipol, ichan)->set_weight(0);
        }
    }
}

bool is_zapped (std::vector<int>& zapped, int index)
{
    for (unsigned i = 0; i < zapped.size(); i++) {
        if (zapped[i] == index)
            return true;
    }

    return false;
}

void unzap_channel(Pulsar::Archive* archive, Pulsar::Archive* backup,
        const int chan_to_unzap)
{
    const uint npol = archive->get_npol();
    const uint nsub = archive->get_nsubint();
    const uint nchan = archive->get_nchan();

    for (uint ipol = 0; ipol < npol; ++ipol) {
        for (uint isub = 0; isub < nsub; ++isub) {
            if (is_zapped(subints_to_zap, isub))
                continue;

            for (uint ichan = 0; ichan < nchan; ++ichan) {
                if (ichan == chan_to_unzap) {
                    float original_weight = backup->
                        get_Profile(isub, ipol, ichan)->get_weight();

                    archive->get_Profile(isub, ipol, ichan)->
                        set_weight(original_weight);
                }
            }
        }
    }
}

void unzap_subint(Pulsar::Archive* archive, Pulsar::Archive* backup, 
        int subint_to_unzap)
{
    const uint npol = archive->get_npol();
    const uint nchan = archive->get_nchan();

    for (uint ipol = 0; ipol < npol; ++ipol) {
        for (uint ichan = 0; ichan < nchan; ++ichan) {
            if (!is_zapped(channels_to_zap, ichan)) {
                float original_weight = backup->get_Profile(subint_to_unzap,
                        ipol, ichan)->get_weight();

                archive->get_Profile(subint_to_unzap, ipol, ichan)->set_weight(
                        original_weight);
            }
        }
    }
}

void redraw(const Pulsar::Archive* arch, Pulsar::Plot* orig_plot, 
        Pulsar::Plot* mod_plot, const bool zoom)
{
    cpgeras();
    zoom ? mod_plot->plot(arch) : orig_plot->plot(arch);
}

void freq_redraw(Pulsar::Archive* arch, const Pulsar::Archive* old_arch, 
        Pulsar::Plot* orig_plot, Pulsar::Plot* mod_plot, const bool zoom)
{
    arch = old_arch->clone();

    dedispersed ? arch->dedisperse() : arch->set_dispersion_measure(0);

    arch->pscrunch();
    arch->tscrunch();
    arch->remove_baseline();
    redraw(arch, orig_plot, mod_plot, zoom);
}

void time_redraw(Pulsar::Archive* arch, const Pulsar::Archive* old_arch, 
        Pulsar::Plot* orig_plot, Pulsar::Plot* mod_plot, const bool zoom)
{
    arch = old_arch->clone();
    if (!dedispersed)
        arch->set_dispersion_measure(0);

    arch->pscrunch();
    arch->fscrunch();
    arch->remove_baseline();
    redraw(arch, orig_plot, mod_plot, zoom);
}

void update_total(Pulsar::Archive* arch, const Pulsar::Archive* old_arch,
        Pulsar::Plot* plot)
{
    arch = old_arch->clone();
    if (!dedispersed)
        arch->set_dispersion_measure(0);

    arch->pscrunch();
    arch->tscrunch();
    arch->fscrunch();
    arch->remove_baseline();

    /*if (centered)
        arch->centre();*/

    cpgslct(2);
    cpgeras();
    plot->plot(arch);
    cpgslct(1);
}

void get_limits(int& lower, int& upper)
{
    if (upper < lower)
        swap(upper, lower);
}

void swap(int& chan1, int &chan2)
{
    int temp = chan1;
    chan1 = chan2;
    chan2 = temp;
}

void add_channel(std::vector<int>& delete_channels, const int chan)
{
    if (!is_zapped(delete_channels, chan))
        delete_channels.push_back(chan);
}

void remove_channel(std::vector<int>& delete_channels, const int chan)
{
    for (std::vector<int>::iterator it = delete_channels.begin();
            it != delete_channels.end(); ++it) {
        if (*it == chan) {
            it = delete_channels.erase(it);
            return;
        }
    }
}

void print_command(const std::vector<int>& chans,
        const std::vector<int>& subints, std::string extension,
        std::string filename)
{
    if (chans.size() || subints.size() ||
            bins_to_zap.size() || subints_to_mow.size()) {
        std::vector<int>::const_iterator it;
        std::cout << "paz ";
        if (chans.size()) {
            std::cout << "-z \"";
            for (it = chans.begin(); it != chans.end(); ++it) {
                std::cout << *it << " ";
            }
            std::cout << "\" ";
        }
        if (subints.size()) {
            std::cout << "-w \"";
            for (it = subints.begin(); it != subints.end(); ++it) {
                std::cout << *it << " ";
            }
            std::cout << "\" ";
        }
        if (bins_to_zap.size()) {
            std::cout << "-B \"";
            for (it = bins_to_zap.begin(); it != bins_to_zap.end(); ++it) {
                std::cout << *it << " ";
            }
            std::cout << "\" ";
        }
        if (subints_to_mow.size()) {
            for (it = subints_to_mow.begin(); it != subints_to_mow.end();
                    ++it) {

                std::cout << *it << " ";
            }
        }
        std::cout << "-e " << extension << " " << filename << std::endl;
    }
}

void set_dedispersion(Pulsar::Archive* arch, const Pulsar::Archive* old_arch)
{
    arch = old_arch->clone();
    if (!dedispersed) {
        dedispersed = true;
        arch->dedisperse();
    } else {
        dedispersed = false;
        arch->set_dispersion_measure(0);
        arch->dedisperse();
        arch->set_dispersion_measure(old_arch->get_dispersion_measure());
        arch->set_dedispersed(false);
    }
    arch->remove_baseline();
}

/*void set_centre(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &centered, int type, bool dedispersed)
{
    if (type != 2) {
        if (!centered) {
            arch->centre();
            centered = true;
        } else {
            centered = false;
            *arch = *old_arch;
            arch->pscrunch();
            arch->remove_baseline();

            if (type == 1) {
                arch->tscrunch();
            } else {
                arch->fscrunch();
            }
        }

        if (dedispersed) {
            arch->dedisperse();
        }
    }
}*/

std::string get_scale(const Pulsar::Archive* arch)
{
    double range = (arch->end_time() - arch->start_time()).in_days();
    const float mjd_hours = 1.0 / 24.0;
    const float mjd_minutes = mjd_hours / 60.0;

    if (range > 1.0)
        return "days";
    else if (range > mjd_hours)
        return "hours";
    else if (range > mjd_minutes)
        return "minutes";
    else
        return "seconds";
}

void binzap(Pulsar::Archive* arch, Pulsar::Archive* old_arch, int subint, 
        int lower_range, int upper_range, int lower_bin, int upper_bin)
{
    BoxMuller gasdev;
    int j;
    const int npol = old_arch->get_npol();
    const int nchan = old_arch->get_nchan();

    if (lower_bin > upper_bin)
        swap(upper_bin, lower_bin);

    for (int ipol = 0; ipol < npol; ++ipol) {
        for (int ichan = 0; ichan < nchan; ++ichan) {
            float *this_int = old_arch->get_Profile(subint, ipol, ichan)->
                get_amps();

            float mean = 0;
            float deviation = 0;

            for (j = lower_range; j < lower_bin; ++j)
                mean += this_int[j];

            for (j = upper_bin; j < upper_range; ++j)
                mean += this_int[j];

            mean = mean / ((upper_range - lower_range) - (upper_bin - lower_bin));

            for (j = lower_range; j < lower_bin; ++j)
                deviation += (this_int[j] - mean) * (this_int[j] - mean);

            for (j = upper_bin; j < upper_range; ++j)
                deviation += (this_int[j] - mean) * (this_int[j] - mean);

            deviation = deviation / ((upper_range - lower_range) - (upper_bin - 
                        lower_bin - 1));

            deviation = sqrt(deviation);

            for (j = lower_bin; j < upper_bin; ++j)
                this_int[j] = mean + (gasdev() * deviation);
        }
    }

    bins_to_zap.push_back(subint);
    bins_to_zap.push_back(lower_range);
    bins_to_zap.push_back(upper_range);
    bins_to_zap.push_back(lower_bin);
    bins_to_zap.push_back(upper_bin);

    arch = old_arch->clone();
    arch->set_dispersion_measure(0);
    arch->pscrunch();
    arch->fscrunch();
    arch->remove_baseline();
}

static Pulsar::LawnMower* mower = 0;
unsigned mowing_subint = 0;

bool accept_mow (Pulsar::Profile* profile, Pulsar::PhaseWeight* weight)
{
    Pulsar::ProfilePlot plotter;
    std::cerr << "mow";

    for (unsigned i = 0; i < weight->get_nbin(); i++)
        if ((*weight)[i])
            std::cerr << " " << i;

    std::cerr << std::endl;
    plotter.set_selection(weight);
    cpgeras();
    plotter.plot_profile(profile);

    std::cerr << "agreed? (y/n)" << std::endl;

    float x = 0;
    float y = 0;
    char c = 0;
    cpgcurs(&x, &y, &c);

    if (c == 'y') {
        subints_to_mow.push_back( mowing_subint );
        return true;
    } else
        return false;
}

void mowlawn(Pulsar::Archive* arch, Pulsar::Archive* old_arch, const int subint)
{
    if (!mower) {
        mower = new Pulsar::LawnMower;
        mower->add_precondition
            (Functor<bool(Pulsar::Profile*,Pulsar::PhaseWeight*)>(&accept_mow));
    }
    mowing_subint = subint;
    mower->transform(old_arch->get_Integration(subint));

    arch = old_arch->clone();
    arch->set_dispersion_measure(0);
    arch->pscrunch();
    arch->fscrunch();
    arch->remove_baseline();
}

void build_archive_data(struct archive_data* data,
        const Pulsar::Archive* archive)
{
    data->nchan = archive->get_nchan();
    data->nsub = archive->get_nsubint();
    data->nbin = archive->get_nbin();
    data->centre_freq = archive->get_centre_frequency();
    data->original_bandwidth = archive->get_bandwidth();
    data->integration_length = archive->integration_length();
    data->time_scale = get_scale(archive);

    if (data->original_bandwidth < 0)
        data->bandwidth = data->original_bandwidth * 1.0;
    else
        data->bandwidth = data->original_bandwidth;
}

