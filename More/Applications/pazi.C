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

using namespace Pulsar;
using namespace std;

static PlotFactory factory;

void usage()
{
	cout << "Pazi: A user-interactive program for zapping subints and frequency channels.\n"
		"Usage: pazi <filename>\n\n"
		"zoom:                      left click twice\n"
		"zap:                       right click\n"
		"zap (multiple):            left click and 'd'\n"
		"unzoom:                    'u'\n"
		"frequency:                 'f'\n"
		"time:                      't'\n"
		"save (<filename>.pazi):    's'\n"
		"quit:                      'q'\n"
		"print paz command:         'c'\n";
}

int freq_get_channel(float mouseY, double bandwidth, int num_chans, double centre_freq);
int time_get_channel(float mouseY, double int_length, int num_subints);
string join_option(float y, float y2, double bandwidth, string type);
void freq_zap_chan(Pulsar::Archive* arch, int zap_chan);
void time_zap_subint(Pulsar::Integration* integ);
void redraw(Pulsar::Archive* arch, Plot* orig_plot, Plot* mod_plot, bool zoom);
void freq_channel_limits(int &lower_chan, int &upper_chan, float mouseY, float mouseY2, double bandwidth, int num_chan, double centre_freq);
void time_channel_limits(int &lower_chan, int &upper_chan, float mouseY, float mouseY2, double int_length, int num_subints);
void add_channel(int chan, vector<int>& delete_channels);
void print_command(vector<int>& freq_chans, vector<int> subints);

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
	int mouseY_channel;
	int mouseY2_channel;
	int lower_channel;
	int upper_channel;
	char ch;
	bool zoomed = false;
	string plot_type = "time";
	string write_filename = argv[1];
	write_filename += ".pazi";

	vector<int> channels_to_zap;
	vector<int> subints_to_zap;

	Pulsar::Integration* integ;
	Reference::To<Pulsar::Archive> base_archive = Archive::load(argv[1]);
	base_archive->pscrunch();
	base_archive->remove_baseline();

	Reference::To<Pulsar::Archive> mod_archive = base_archive->clone();
	mod_archive->fscrunch();

	Plot *time_orig_plot = factory.construct("time");
	Plot *time_mod_plot = factory.construct("time");
	TextInterface::Class* time_fui = time_mod_plot->get_frame_interface();

	Plot *freq_orig_plot = factory.construct("freq");
	Plot *freq_mod_plot = factory.construct("freq");
	TextInterface::Class* freq_fui = freq_mod_plot->get_frame_interface();

	double centre_freq = base_archive->get_centre_frequency();
	double bandwidth = base_archive->get_bandwidth();
	int num_chan = base_archive->get_nchan();
	double int_length = base_archive->integration_length();
	int num_subints = base_archive->get_nsubint();

	if (bandwidth < 0)
		bandwidth = bandwidth * 1;

	cpgopen("/XS");
	cpgask(0);
	time_orig_plot->plot(mod_archive);

	while (1) {
	    cpgband(3, 0, mouseX, mouseY, &mouseX2, &mouseY2, &ch);
		switch (ch) {

			case 'A': // zoom
				if (!mouseY) {
					mouseY = mouseY2;
				} else {
					if (plot_type == "freq") {
						mouseY_channel = freq_get_channel(mouseY, bandwidth, num_chan, centre_freq);
						mouseY2_channel = freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq);

						if ((mouseY_channel - mouseY2_channel < -1) || (mouseY_channel - mouseY2_channel > 1)) {
							zoomed = true;
							freq_fui->set_value("y:win", join_option(mouseY, mouseY2, bandwidth, plot_type));
							redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
						}
						mouseY = 0;

					} else {
						mouseY_channel = time_get_channel(mouseY, int_length, num_subints);
						mouseY2_channel = time_get_channel(mouseY2, int_length, num_subints);

						if ((mouseY_channel - mouseY2_channel < -1) || (mouseY_channel - mouseY2_channel > 1)) {
							zoomed = true;
							time_fui->set_value("y:win", join_option(mouseY, mouseY2, bandwidth, plot_type));
							redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
						}
						mouseY = 0;
					}
				}
				break;

			case 'c':
				print_command(channels_to_zap, subints_to_zap);
				break;

			case 'd': // zap multiple channels
				if (mouseY) {
					if (plot_type == "freq") {
						freq_channel_limits(lower_channel, upper_channel, mouseY, mouseY2, bandwidth, num_chan, centre_freq);

						for (int i = lower_channel; i <= upper_channel; i++) {
							channels_to_zap.push_back(i);
							freq_zap_chan(base_archive, i);
						}

						*mod_archive = *base_archive;
						mod_archive->tscrunch();
						redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);

					} else {
						time_channel_limits(lower_channel, upper_channel, mouseY, mouseY2, int_length, num_subints);
						Pulsar::Integration* integ;

						for (int i = lower_channel; i <= upper_channel; i++) {
							subints_to_zap.push_back(i);
							integ = base_archive->get_Integration(i);
							time_zap_subint(integ);
						}

						*mod_archive = *base_archive;
						mod_archive->fscrunch();
						redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
					}
					mouseY = 0;
				}
				break;

			case 'f': // frequency plot
				mouseY = 0;
				plot_type = "freq";
				zoomed = false;
				*mod_archive = *base_archive;
				mod_archive->tscrunch();
				redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
				break;

			case 'q': // quit
				cpgclos();
				return EXIT_SUCCESS;

			case 's': // save current archive changes
				base_archive->unload(write_filename);
				break;

			case 't': // time plot
				mouseY = 0;
				plot_type = "time";
				zoomed = false;
				*mod_archive = *base_archive;
				mod_archive->fscrunch();
				redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
				break;

			case 'u': // unzoom
				zoomed = false;
				mouseY = 0;

				if (plot_type == "freq")
					redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
				else
					redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
				break;

			case 'X': // zap single channel
				if (mouseY) {
					mouseY = 0;
				} else {
					if (plot_type == "freq") {
						int freq_channel = freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq);
						add_channel(freq_channel, channels_to_zap);
						freq_zap_chan(base_archive, freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq) - 1);
						*mod_archive = *base_archive;
						mod_archive->tscrunch();
						redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
					} else {
						add_channel(time_get_channel(mouseY2, int_length, num_subints), subints_to_zap);
						integ = base_archive->get_Integration(time_get_channel(mouseY2, int_length, num_subints));
						time_zap_subint(integ);
						*mod_archive = *base_archive;
						mod_archive->fscrunch();
						redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
					}
				}
				break;
		}
	}
}

int freq_get_channel(float mouseY, double bandwidth, int num_chans, double centre_freq)
{
	double channel = ((mouseY - centre_freq - (bandwidth / 2)) / bandwidth) * num_chans;
	if (channel < 0)
		channel = channel * -1;
	return num_chans - (int)channel;
}

int time_get_channel(float mouseY, double int_length, int num_subints)
{
	double channel = (mouseY * 60) / int_length * num_subints;
	return (int)channel;
}

string join_option(float y, float y2, double bandwidth, string type)
{
	string option = "(";
	char add[5];
	if ((type == "time" && y > y2) || (type == "freq" && y < y2 && bandwidth < 0)) {
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

void freq_zap_chan(Pulsar::Archive* arch, int zap_chan)
{
	Pulsar::Integration* integ;
	int nsubint = arch->get_nsubint();
	int nchan = arch->get_nchan();

	for (int i = 0; i < nsubint; i++) {
		for (int j = 0; j < nchan; j++) {
			if (j == zap_chan) {
				integ = arch->get_Integration(i);
				integ->get_Profile(0,j)->set_weight(0);
			}
		}
	}
}

void time_zap_subint(Pulsar::Integration* integ)
{
	int nchan = integ->get_nchan();
	for (int i = 0; i < nchan; i++)
		integ->get_Profile(0,i)->set_weight(0);
}

void redraw(Pulsar::Archive* arch, Plot* orig_plot, Plot* mod_plot, bool zoom)
{
	cpgeras();
	if (zoom)
		mod_plot->plot(arch);
	else
		orig_plot->plot(arch);
}

void freq_channel_limits(int &lower_chan, int &upper_chan, float mouseY, float mouseY2, double bandwidth, int num_chan, double centre_freq)
{
	lower_chan = freq_get_channel(mouseY, bandwidth, num_chan, centre_freq);
	upper_chan = freq_get_channel(mouseY2, bandwidth, num_chan, centre_freq);

	if (upper_chan < lower_chan) {
		int temp = upper_chan;
		upper_chan = lower_chan;
		lower_chan = temp;
	}
}

void time_channel_limits(int &lower_chan, int &upper_chan, float mouseY, float mouseY2, double int_length, int num_subints)
{
	lower_chan = time_get_channel(mouseY, int_length, num_subints);
	upper_chan = time_get_channel(mouseY2, int_length, num_subints);

	if (upper_chan < lower_chan) {
		int temp = upper_chan;
		upper_chan = lower_chan;
		lower_chan = temp;
	}
}

void add_channel(int chan, vector<int>& delete_channels)
{
    bool found = 0;
	for (int i = 0; i < delete_channels.size(); i++) {
		if (chan == delete_channels[i]) {
			found = 1;
			break;
		}
	}

	if (!found) {
		delete_channels.push_back(chan);
    }
}

void print_command(vector<int>& freq_chans, vector<int> subints)
{
	if (freq_chans.size() || subints.size()) {
		cout << "paz -e zap ";
		if (freq_chans.size()) {
			cout << "-z \"";

			for (int i = 0; i < freq_chans.size() - 1; i++) {
				cout << freq_chans[i] << " ";
			}
			cout << freq_chans[freq_chans.size()-1] << "\" ";
		}

		if (subints.size()) {
			cout << "-X \"";

			for (int i = 0; i< subints.size() - 1; i++) {
				cout << subints[i] << " ";
			}
			cout << subints[subints.size() - 1] << "\"" << endl;
		}
	}
}

