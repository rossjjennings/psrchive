/***************************************************************************
 *
 *   Copyright (C) 2007 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/GaussianBaseline.h"

#include "TextInterface.h"
#include "strutil.h"

#include <cpgplot.h>
#include <unistd.h>
#include <string.h>

#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include "BoxMuller.h"

using namespace Pulsar;
using namespace std;

static PlotFactory factory;

void usage()
{
	cout << "Pazi: A user-interactive program for zapping subints and frequency channels.\n"
		"Usage: pazi <filename>\n\n"
		"zoom:                      left click twice\n"
		"zap:                       right click\n"
		"zap (multiple):            left click and 'z'\n"
		"reset zoom:                'r'\n"
		"frequency:                 'f'\n"
		"time:                      't'\n"
		"save (<filename>.pazi):    's'\n"
		"quit:                      'q'\n"
		"print paz command:         'p'\n"
		"center pulse:              'c'\n"
		"undo last:                 'u'\n"
		"toggle dedispersion:       'd'\n"
		"binzap:                    'b'\n";
}

int freq_get_channel(float mouseY, double bandwidth, int num_chans, double centre_freq);
int time_get_channel(float mouseY, double int_length, int num_subints, string scale);
string join_option(float y, float y2, double bandwidth, string type);
void freq_zap_chan(Pulsar::Archive* arch, int zap_chan);
void freq_unzap_chan(Pulsar::Archive* arch, Pulsar::Archive* backup, int zap_chan);
void time_zap_subint(Pulsar::Integration* integ);
void time_unzap_subint(Pulsar::Integration* integ, Pulsar::Integration* backup);
void redraw(Pulsar::Archive* arch, Plot* orig_plot, Plot* mod_plot, bool zoom);
void freq_redraw(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* orig_plot, Plot* mod_plot, bool zoom);
void time_redraw(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* orig_plot, Plot* mod_plot, bool zoom);
void update_total(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* plot);
void freq_get_limits(int &lower_chan, int &upper_chan, float mouseY, float mouseY2, double bandwidth, int num_chan, double centre_freq);
void time_get_limits(int &lower_chan, int &upper_chan, float mouseY, float mouseY2, double int_length, int num_subints, string scale);
void swap_chans(int &chan1, int &chan2);
bool add_channel(int chan, vector<int>& delete_channels);
bool remove_channel(int chan, vector<int>& delete_channels);
void print_command(vector<int>& freq_chans, vector<int> subints, string extension, string filename);
void set_dedispersion(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &dedispersed);
void set_centre(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &centered, string type, bool dedispersed);
string get_scale(Pulsar::Archive* arch);
string vertical_join_option(float x, float x2, int &upper_chan, int &lower_chan, int num_bins);
void binzap(Pulsar::Archive* arch, Pulsar::Archive* old_arch, int subint, int lower_range, int upper_range, int lower_bin, int upper_bin);

static bool dedispersed = true;
static vector<int> channels_to_zap;
static vector<int> subints_to_zap;
static vector<int> bins_to_zap;
//static bool centered = true;
static bool centered = false;

int main(int argc, char** argv)
{
	if (argc < 2) {
		usage();
		return EXIT_SUCCESS;
	}

	float mouseX = 0;
	float mouseY = 0;
	float mouseX2;
	float mouseY2;
	int mouseY_chan;
	int mouseY2_chan;
	int lower_channel;
	int upper_channel;
	int subint;
	char ch;
	bool zoomed = false;
	bool fscrunched = true;

	string plot_type = "time";
	string filename = argv[1];

	string extension = filename.substr(filename.length() - 2, 2);

	if ((extension != "rf") && (extension != "cf")) {
		extension = "pazi";
	} else {
		if (extension == "rf") {
			extension = "rz";
		} else {
			extension = "cz";
		}
	}

	string write_filename = filename + ".";
	write_filename += extension;

	Reference::To<Pulsar::Archive> base_archive = Archive::load(argv[1]);
	base_archive->pscrunch();
	base_archive->remove_baseline();

	Reference::To<Pulsar::Archive> mod_archive = base_archive->clone();
	mod_archive->dedisperse();

	//mod_archive->centre();
	//printf("centre\n");
	
	mod_archive->fscrunch();
	
	Reference::To<Pulsar::Archive> backup_archive = base_archive->clone();
	Reference::To<Pulsar::Archive> scrunched_archive = mod_archive->clone();
	scrunched_archive->tscrunch();
	Pulsar::Integration* integ;

	double centre_freq = base_archive->get_centre_frequency();
	double bandwidth = base_archive->get_bandwidth();
	int num_chan = base_archive->get_nchan();
	double int_length = base_archive->integration_length();
	int num_subints = base_archive->get_nsubint();
	double original_bandwidth = bandwidth;
	int num_bins = base_archive->get_nbin();
	string scale = get_scale(base_archive);

	int upper_range = base_archive->get_nbin() - 1;
	int lower_range = 0;

	if (bandwidth < 0)
		bandwidth = bandwidth * 1;

	Plot *time_orig_plot = factory.construct("time");
	Plot *time_mod_plot = factory.construct("time");
	TextInterface::Class* time_fui = time_mod_plot->get_frame_interface();

	Plot *freq_orig_plot = factory.construct("freq");
	Plot *freq_mod_plot = factory.construct("freq");
	TextInterface::Class* freq_fui = freq_mod_plot->get_frame_interface();

	Plot *total_plot = factory.construct("flux");
	total_plot->configure("info=1");

	Plot *subint_orig_plot = factory.construct("flux");
	Plot *subint_mod_plot = factory.construct("flux");
	TextInterface::Class* subint_fui = subint_mod_plot->get_frame_interface();
	subint_orig_plot->configure("info=1");
	subint_mod_plot->configure("info=1");

	cpgopen("1/XS");
	cpgopen("2/XS");

	cpgask(0);

	cerr << endl << "Total S/N = " << scrunched_archive->get_Profile(0,0,0)->snr() << endl << endl;
	total_plot->plot(scrunched_archive);
	cpgslct(1);
	time_orig_plot->plot(mod_archive);

	while (1) {
		if (plot_type != "subint")
			cpgband(3, 0, mouseX, mouseY, &mouseX2, &mouseY2, &ch);
		else
			cpgband(4, 0, mouseX, mouseY, &mouseX2, &mouseY2, &ch);

		switch (ch) {
			case 'A': // zoom
				if (plot_type == "subint") {
					if (!mouseX) {
						mouseX = mouseX2;
					} else {
						zoomed = true;
						subint_fui->set_value("x:win", vertical_join_option(mouseX, mouseX2, upper_range, lower_range, num_bins));
						redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
						mouseX = 0;
					}

				} else {
					if (!mouseY) {
						mouseY = mouseY2;
					} else {
						if (plot_type == "freq") {
							mouseY_chan = freq_get_channel(mouseY, original_bandwidth, num_chan, centre_freq);
							mouseY2_chan = freq_get_channel(mouseY2, original_bandwidth, num_chan, centre_freq);
						} else {
							mouseY_chan = time_get_channel(mouseY, int_length, num_subints, scale);
							mouseY2_chan = time_get_channel(mouseY2, int_length, num_subints, scale);
						}

						if ((mouseY_chan - mouseY2_chan < -1) || (mouseY_chan - mouseY2_chan > 1)) {
							zoomed = true;
							if (plot_type == "freq") {
								freq_fui->set_value("y:win", join_option(mouseY, mouseY2, original_bandwidth, plot_type));
								redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
							} else {
								time_fui->set_value("y:win", join_option(mouseY, mouseY2, bandwidth, plot_type));
								redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
							}
						}
						mouseY = 0;
					}
				}
				break;

			case 'b': // plot specific subint
				if (plot_type == "time") {
					centered = false;
					zoomed = false;
					mouseX = 0;
					plot_type = "subint";
					*mod_archive = *base_archive;
					mod_archive->set_dispersion_measure(0);
					mod_archive->fscrunch();
					subint = time_get_channel(mouseY2, int_length, num_subints, scale);

					char add[3];
					sprintf(add, "%d", subint);
					string subint_option = "subint=";
					subint_option += add;

					subint_orig_plot->configure(subint_option);
					subint_mod_plot->configure(subint_option);

					cpgeras();
					subint_orig_plot->plot(mod_archive);
					update_total(scrunched_archive, base_archive, total_plot);
				}
					break;

			/*case 'c': // center pulse
				set_centre(mod_archive, base_archive, centered, plot_type, dedispersed);

				if (plot_type == "freq")
					redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
				else if (plot_type == "time")
					redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);

				update_total(scrunched_archive, base_archive, total_plot);
				break;*/

			case 'd': // toggle dedispersion on/off
				set_dedispersion(mod_archive, base_archive, dedispersed);

				if (plot_type == "freq") {
					mod_archive->tscrunch();
					redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
				} else if (plot_type == "time") {
					mod_archive->fscrunch();
					redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
				}

				update_total(scrunched_archive, base_archive, total_plot);
				break;

			case 'f': // frequency plot
				mouseY = 0;
				plot_type = "freq";
				zoomed = false;
				freq_redraw(mod_archive, base_archive, freq_orig_plot, freq_mod_plot, zoomed);
				break;

			case 'o':
				if (plot_type == "time") {
					if (fscrunched) {
						fscrunched = false;
						*mod_archive = *base_archive;
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
				print_command(channels_to_zap, subints_to_zap, extension, filename);
				break;

			case 'r': // reset zoom
				zoomed = false;
				mouseY = 0;
				mouseX = 0;

				if (plot_type == "freq")
					redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
				else if (plot_type == "time")
					redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
				else {
					upper_range = base_archive->get_nbin() - 1;
					lower_range = 0;
					redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
				}
				break;

			case 's': // save current archive changes
				base_archive->unload(write_filename);
				break;

			case 't': // time plot
				mouseY = 0;
				plot_type = "time";
				zoomed = false;
				time_redraw(mod_archive, base_archive, time_orig_plot, time_mod_plot, zoomed);
				break;

			case 'u': // undo last change
				if (mouseY) {
					mouseY = 0;
				} else {
					if (plot_type == "freq") {
						remove_channel(freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq), channels_to_zap);
						freq_unzap_chan(base_archive, backup_archive, freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq));
						freq_redraw(mod_archive, base_archive, freq_orig_plot, freq_mod_plot, zoomed);
					} else if (plot_type == "time") {
						remove_channel(time_get_channel(mouseY2, int_length, num_subints, scale), subints_to_zap);
						time_unzap_subint(backup_archive->get_Integration(time_get_channel(mouseY2, int_length, num_subints, scale)), backup_archive->get_Integration(time_get_channel(mouseY2, int_length, num_subints, scale)));
						time_redraw(mod_archive, base_archive, time_orig_plot, time_mod_plot, zoomed);
					} else {
						if (bins_to_zap.size()) {
							printf("harroh!\n");

							bins_to_zap.erase(bins_to_zap.end() - 5, bins_to_zap.end());

							*base_archive = *backup_archive;
							*mod_archive = *backup_archive;

							mod_archive->set_dispersion_measure(0);
							mod_archive->fscrunch();
							redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
						}
					}
				}
				update_total(scrunched_archive, base_archive, total_plot);
				break;

            case 'X': // zap single channel
				if (mouseY || mouseX) {
					mouseY = 0;
					mouseX = 0;
				} else {
					*backup_archive = *base_archive;

					if (plot_type == "freq") {
						add_channel(freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq), channels_to_zap);
						freq_zap_chan(base_archive, freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq));
						freq_redraw(mod_archive, base_archive, freq_orig_plot, freq_mod_plot, zoomed);

					} else if (plot_type == "time") {
						add_channel(time_get_channel(mouseY2, int_length, num_subints, scale), subints_to_zap);
						time_zap_subint(base_archive->get_Integration(time_get_channel(mouseY2, int_length, num_subints, scale)));
						time_redraw(mod_archive, base_archive, time_orig_plot, time_mod_plot, zoomed);

					} else {
						if (((upper_range + 1 == base_archive->get_nbin()) && (upper_range - 1 - lower_range > 0)) || ((upper_range + 1 != base_archive->get_nbin()) && (upper_range - lower_range > 0))) {
							int bin = static_cast<int>(mouseX2 * num_bins);
							binzap(mod_archive, base_archive, subint, lower_range, upper_range, bin, bin+1);
							redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
						}
					}
					update_total(scrunched_archive, base_archive, total_plot);
				}
				break;

			case 'z': // zap multiple channels
				if (plot_type == "subint") {
					if (mouseX) {
						binzap(mod_archive, base_archive, subint, lower_range, upper_range, static_cast<int>(mouseX * num_bins), static_cast<int>(mouseX2 * num_bins));
						redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
						update_total(scrunched_archive, base_archive, total_plot);
						mouseX = 0;
					}

				} else {

				*backup_archive = *base_archive;

					if (mouseY) {
						if (plot_type == "freq") {
							freq_get_limits(lower_channel, upper_channel, mouseY, mouseY2, bandwidth, num_chan, centre_freq);

							for (int i = lower_channel; i <= upper_channel; i++) {
								channels_to_zap.push_back(i);
								freq_zap_chan(base_archive, i);
							}
							freq_redraw(mod_archive, base_archive, freq_orig_plot, freq_mod_plot, zoomed);

						} else {
							time_get_limits(lower_channel, upper_channel, mouseY, mouseY2, int_length, num_subints, scale);
							Pulsar::Integration* integ;

							for (int i = lower_channel; i <= upper_channel; i++) {
								subints_to_zap.push_back(i);
								integ = base_archive->get_Integration(i);
								time_zap_subint(integ);
							}
							time_redraw(mod_archive, base_archive, time_orig_plot, time_mod_plot, zoomed);
						}
						mouseY = 0;
					}
				}
				update_total(scrunched_archive, base_archive, total_plot);
				break;
		}
	}
}

int freq_get_channel(float mouseY, double bandwidth, int num_chans, double centre_freq)
{
	double channel = ((mouseY - centre_freq - (bandwidth / 2)) / bandwidth) * num_chans;
	if (channel < 0)
		channel = channel * -1;
	return num_chans - (int)channel - 1;
}

int time_get_channel(float mouseY, double int_length, int num_subints, string scale)
{
	double channel;
	if (scale == "days")
		channel = (mouseY * 60 * 60 * 24) / int_length * num_subints;
	else if (scale == "hours")
		channel = (mouseY * 60 * 60) / int_length * num_subints;
	else if (scale == "minutes")
		channel = (mouseY * 60) / int_length * num_subints;
	else
		channel = mouseY / int_length * num_subints;

	return (int)channel;
}

string join_option(float y, float y2, double bandwidth, string type)
{
	string option = "(";
	char add[5];
	if ((type == "time" && y > y2) || (type == "freq" && y > y2 && bandwidth > 0) || (type == "freq" && y < y2 && bandwidth < 0)) {
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

string vertical_join_option(float x, float x2, int &upper_chan, int &lower_chan, int num_bins)
{
	string option = "(";
	char add[3];
	if (x > x2) {
		sprintf(add, "%2.2f", x2);
		option += add;
		option += ",";
		sprintf(add, "%2.2f", x);
		option += add;
		option += ")";
		upper_chan = static_cast<int>(x * num_bins);
		lower_chan = static_cast<int>(x2 * num_bins);
	} else {
		sprintf(add, "%2.2f", x);
		option += add;
		option += ",";
		sprintf(add, "%2.2f", x2);
		option += add;
		option += ")";
		upper_chan = static_cast<int>(x2 * num_bins);
		lower_chan = static_cast<int>(x * num_bins);
	}
	return option;
}

void freq_zap_chan(Pulsar::Archive* arch, int zap_chan)
{
	Pulsar::Integration* integ;

	for (int i = 0; i < arch->get_nsubint(); i++) {
		for (int j = 0; j < arch->get_nchan(); j++) {
			if (j == zap_chan) {
				integ = arch->get_Integration(i);
				integ->get_Profile(0,j)->set_weight(0);
			}
		}
	}
}

void time_zap_subint(Pulsar::Integration* integ)
{
	for (int i = 0; i < integ->get_nchan(); i++)
		integ->get_Profile(0,i)->set_weight(0);
}

bool is_zapped (vector<int>& zapped, int index)
{
  for (unsigned i=0; i<zapped.size(); i++)
    if (zapped[i] == index)
      return true;
  return false;
}

void freq_unzap_chan(Pulsar::Archive* arch, Pulsar::Archive* backup, int zap_chan)
{
    Pulsar::Integration* integ;
    Pulsar::Integration* binteg;

    for (int i = 0; i < arch->get_nsubint(); i++) {
		if (is_zapped(subints_to_zap, i))
			continue;

		for (int j = 0; j < arch->get_nchan(); j++) {
			if (j == zap_chan) {
				integ = arch->get_Integration(i);
				binteg = backup->get_Integration(i);
				integ->get_Profile(0,j)->set_weight(binteg->get_weight(j));
			}
		}

    }
}

void time_unzap_subint(Pulsar::Integration* integ, Pulsar::Integration* backup)
{
    for (int i = 0; i < integ->get_nchan(); i++)
		if (!is_zapped(channels_to_zap,i))
		    integ->get_Profile(0,i)->set_weight( backup->get_weight(i) );
}

void redraw(Pulsar::Archive* arch, Plot* orig_plot, Plot* mod_plot, bool zoom)
{
	if (centered) {
		arch->centre();
	}

	cpgeras();
	if (zoom)
		mod_plot->plot(arch);
	else
		orig_plot->plot(arch);
}

void freq_redraw(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* orig_plot, Plot* mod_plot, bool zoom)
{
	*arch = *old_arch;
	if (!dedispersed)
	    arch->set_dispersion_measure(0);
	else
	    arch->dedisperse();

	arch->tscrunch();
	redraw(arch, orig_plot, mod_plot, zoom);
}

void time_redraw(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* orig_plot, Plot* mod_plot, bool zoom)
{
	*arch = *old_arch;
	if (!dedispersed)
	    arch->set_dispersion_measure(0);

	arch->fscrunch();
	redraw(arch, orig_plot, mod_plot, zoom);
}

void update_total(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* plot)
{
	*arch = *old_arch;
	if (!dedispersed)
		arch->set_dispersion_measure(0);

	arch->tscrunch();
	arch->fscrunch();

	if (centered)
		arch->centre();

	cpgslct(2);
	cpgeras();
	plot->plot(arch);
	cpgslct(1);
}

void freq_get_limits(int &lower_chan, int &upper_chan, float mouseY, float mouseY2, double bandwidth, int num_chan, double centre_freq)
{
	lower_chan = freq_get_channel(mouseY, bandwidth, num_chan, centre_freq);
	upper_chan = freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq);

	if (upper_chan < lower_chan)
		swap_chans(upper_chan, lower_chan);
}

void time_get_limits(int &lower_chan, int &upper_chan, float mouseY, float mouseY2, double int_length, int num_subints, string scale)
{
	lower_chan = time_get_channel(mouseY, int_length, num_subints, scale);
	upper_chan = time_get_channel(mouseY2, int_length, num_subints, scale);

	if (upper_chan < lower_chan)
		swap_chans(upper_chan, lower_chan);
}

void swap_chans(int &chan1, int &chan2)
{
	int temp = chan1;
	chan1 = chan2;
	chan2 = temp;
}

bool add_channel(int chan, vector<int>& delete_channels)
{
  if (!is_zapped(delete_channels, chan)) {
      delete_channels.push_back(chan);
      return true;
  }
  return false;
}

bool remove_channel(int chan, vector<int>& delete_channels)
{
    for (vector<int>::iterator it = delete_channels.begin(); it != delete_channels.end(); ++it) {
        if (*it == chan) {
			it = delete_channels.erase(it);
			return true;
		}
    }
    return false;
}

void print_command(vector<int>& freq_chans, vector<int> subints, string extension, string filename)
{
	if (freq_chans.size() || subints.size() || bins_to_zap.size()) {

		cout << "paz ";

		if (freq_chans.size()) {
			cout << "-z \"";

			for (int i = 0; i < freq_chans.size() - 1; i++) {
				cout << freq_chans[i] << " ";
			}
			cout << freq_chans[freq_chans.size()-1] << "\" ";
		}

		if (subints.size()) {
			cout << "-w \"";

			for (int i = 0; i< subints.size() - 1; i++) {
				cout << subints[i] << " ";
			}
			cout << subints[subints.size() - 1] << "\" ";
		}

		if (bins_to_zap.size()) {
			cout << "-B \"";

			for (int i = 0; i < bins_to_zap.size() - 1; i++) {
				cout << bins_to_zap[i] << " ";
			}
			cout << bins_to_zap[bins_to_zap.size() - 1] << "\" ";
		}

		cout << "-e " << extension << " " << filename << endl;
	}
}

void set_dedispersion(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &dedispersed)
{
	if (!dedispersed) {
		dedispersed = true;
		*arch = *old_arch;
		arch->dedisperse();
	} else {
		dedispersed = false;
		*arch = *old_arch;
		arch->set_dispersion_measure(0);
		arch->dedisperse();
	}
}

void set_centre(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &centered, string type, bool dedispersed)
{
	if (type != "subint") {
		if (!centered) {
			arch->centre();
			centered = true;
		} else {
			centered = false;
			*arch = *old_arch;

			if (type == "freq") {
				arch->tscrunch();
			} else {
				arch->fscrunch();
			}
		}

		if (dedispersed) {
			arch->dedisperse();
		}
	}
}

string get_scale(Pulsar::Archive* arch)
{
	double range = (arch->end_time() - arch->start_time()).in_days();

	const float mjd_hours = 1.0 / 24.0;
	const float mjd_minutes = mjd_hours / 60.0;
	const float mjd_seconds = mjd_minutes / 60.0;

	if (range > 1.0)
		return "days";
	else if (range > mjd_hours)
		return "hours";
	else if (range > mjd_minutes)
		return "minutes";
	else
		return "seconds";
}

void binzap(Pulsar::Archive* arch, Pulsar::Archive* old_arch, int subint, int lower_range, int upper_range, int lower_bin, int upper_bin)
{
	BoxMuller gasdev;
	float mean;
	float deviation;
	float* this_int;
	int j;
	Pulsar::Integration* integ = old_arch->get_Integration(subint);

	if (lower_bin > upper_bin)
		swap_chans(upper_bin, lower_bin);

	for (int i = 0; i < old_arch->get_nchan(); i++) {
		this_int = integ->get_Profile(0,i)->get_amps();
		mean = 0;
		deviation = 0;

		for (j = lower_range; j < lower_bin; j++)
			mean += this_int[j];

		for (j = upper_bin; j < upper_range; j++)
			mean += this_int[j];

		mean = mean / ((upper_range - lower_range) - (upper_bin - lower_bin));

		for (j = lower_range; j < lower_bin; j++)
			deviation += (this_int[j] - mean) * (this_int[j] - mean);

		for (j = upper_bin; j < upper_range; j++)
			deviation += (this_int[j] - mean) * (this_int[j] - mean);

		deviation = deviation / ((upper_range - lower_range) - (upper_bin - lower_bin - 1));
		deviation = sqrt(deviation);

		for (j = lower_bin; j < upper_bin; j++)
			this_int[j] = mean + (gasdev() * deviation);
	}

	bins_to_zap.push_back(subint);
	bins_to_zap.push_back(lower_range);
	bins_to_zap.push_back(upper_range);
	bins_to_zap.push_back(lower_bin);
	bins_to_zap.push_back(upper_bin);

	*arch = *old_arch;
	arch->set_dispersion_measure(0);
	arch->fscrunch();
}
