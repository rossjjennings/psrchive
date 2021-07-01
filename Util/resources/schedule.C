/***************************************************************************
 *
 *   Copyright (C) 2020 - 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Horizon.h"
#include "stringtok.h"
#include "tostring.h"
#include "strutil.h"

#include <fstream>
#include <algorithm>

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

void usage ()
{
  cout <<
    "schedule - create a schedule\n"
    "  -i         interactive mode \n"
    "  -l lst     start LST \n"
    "  -L lst     end LST \n"
    "  -m max     maximum number of sources listed in interactive mode \n"
    "  -p min     minutes per interval in lst_density.txt \n"
    "  -P         add PRESS-specific constraints \n"
    "  -s         minimize slew time \n"
    "  -S lst.txt simulate a semester, given LST ranges of sessions\n"
    "  -c         create LST-balanced list sorted by integration length\n"
    "  -C hours   cutoff LST-balanced list after specified hours\n"
    "\n"
       << endl;
}

class source
{
public:
  source ()
  {
    set_lst.setWrapPoint( 2*M_PI );
    rise_lst.setWrapPoint( 2*M_PI );
  }

  // name priority coord rise set up(hr) Tint(min) tsamp
  // J0034-0721 152.1 00:34:08.8-07:21:53.4 20:40:00 04:28:17 7.80469 64 64
  string name;
  double priority;
  sky_coord coord;
  Angle rise_lst;
  Angle set_lst;
  double up_hours;
  double Tint_min;
  double tsamp_us;

  string text;
  
  // computed
  double min_avg_density;
  double slewtime;
  double Tobs;
  Mount* mount;

  void parse (string line);
};

bool by_priority (const source& a, const source& b)
{
  return a.priority > b.priority;
}

bool by_slewtime (const source& a, const source& b)
{
  return a.slewtime < b.slewtime;
}

bool by_integration_length (const source& a, const source& b)
{
  return a.Tint_min < b.Tint_min;
}

pair<double,Mount*> best_slew (Mount* mount, const sky_coord& coord)
{
  Horizon* horizon = dynamic_cast<Horizon*> (mount);
  // cerr << "best_slew: mount azimuth = " << horizon->get_azimuth() << endl;

  vector< pair<double,Mount*> > slew_times;
  slew_times = mount->slew_times (coord);

  pair<double,Mount*> result = slew_times[0];

  for (unsigned i=1; i<slew_times.size(); i++)
  {
    if (slew_times[i].first < result.first)
      result = slew_times[i];
  }

  // cerr << "best_slew: result = " << result.first/60.0 << endl;
  return result;
}

int simulate_semester();
int simulate_session();

void output_lst_density (vector<double>& lst_density,
			 const vector<source>& sources);

void output_src_density (const vector<double>& lst_density,
			 vector<source>& sources);

void output_balanced_source_list (vector<source>& sources, float cutoff_hours);

// some constants for conversions
const double rad2deg = 180.0/M_PI;
const double rad2hr = 12.0/M_PI;

std::string semester;

Angle start_lst;
Angle end_lst;

// the Horizon object stores telescope state
Horizon horizon;

double min_elevation = 0;

vector<source> orig_sources;
vector<source> sources;

bool press = false;
bool minimize_slew_time = false;
bool interactive = false;
bool observe_once_per_session = false;

unsigned long maxshow = 10;

int main (int argc, char* argv[]) 
{
  // the Angle class does useful conversions
  Angle latitude;
  Angle longitude;

  // Parkes Observatory: 32.9983°S, 148.2636°E
  latitude.setDegrees( -32.9983 );
  longitude.setDegrees( 148.2636 );

  horizon.set_observatory_latitude( latitude.getRadians() );
  horizon.set_observatory_longitude( longitude.getRadians() );

  horizon.set_minimum_azimuth ( -135 / rad2deg );
  horizon.set_maximum_azimuth ( 315 / rad2deg );

  // 24 degrees/minute in rad/s
  horizon.set_azimuth_velocity ( 24 / (rad2deg * 60) );
  // 10 degrees/minute downwards or 12 degrees/minute upwards in rad/s
  horizon.set_elevation_velocity ( 10 / (rad2deg * 60) );

  min_elevation = 30.3;

  start_lst.setWrapPoint( 2*M_PI );
  bool start_lst_specified = false;

  end_lst.setWrapPoint( 2*M_PI );
  bool end_lst_specified = false;

  double interval = 15.0;  // minutes
  unsigned lst_bins = 60.0*24.0/interval;
  vector<double> lst_density (lst_bins, 0.0);

  bool output_sorted_list = false;
  float cutoff_hours = 0.0;
  
  int c;
  while ((c = getopt(argc, argv, "hcC:e:il:L:m:p:PsS:")) != -1)
  {
    switch (c)
    {
    case 'h':
      usage ();
      return 0;

    case 'c':
      output_sorted_list = true;
      break;

    case 'C':
      cutoff_hours = atof(optarg);
      break;
      
    case 'e':
      min_elevation = atof (optarg);
      break;

    case 'i':
      interactive = true;
      break;

    case 'l':
      start_lst.setHMS(optarg);
      start_lst_specified = true;
      break;

    case 'L':
      end_lst.setHMS(optarg);
      end_lst_specified = true;
      break;

    case 'm':
      maxshow = atoi (optarg);
      break;

    case 'p':
    {
      double interval = atof(optarg);
      unsigned lst_bins = 60.0*24.0/interval;
      lst_density.resize (lst_bins, 0.0);
      cerr << "accumulating LST density in " << 60.0*24.0/lst_density.size() 
           << " min bins" << endl;
      break;
    }

    case 'P':
      press = true;
      break;

    case 's':
      minimize_slew_time = true;
      break;

    case 'S':
      semester = optarg;
      break;

    }
  }

  if (end_lst < start_lst)
  {
    start_lst.setWrapPoint( M_PI );
    end_lst.setWrapPoint( M_PI );
  }

  if (optind == argc)
  {
    cerr << "please specify source file" << endl;
    return -1;
  }

  vector<string> lines;
  string filename = argv[optind];
  loadlines (filename, lines);

  cerr << lines.size() << " lines of text loaded from " << filename << endl;

  for (unsigned i=0; i<lines.size(); i++) try
  {
    source tmp;
    tmp.parse( lines[i] );
    sources.push_back(tmp);
  }
  catch (Error& error)
    {
      cerr << error.get_message() << endl;
    }
  
  orig_sources = sources;

  if (lst_density.size())
  {
    output_lst_density (lst_density, sources);
    output_src_density (lst_density, sources);
  }

  if (output_sorted_list)
  {
    output_balanced_source_list (sources, cutoff_hours);
    return 0;
  }
  
  if (!semester.empty())
  {
    cerr << "simulating semester=" << semester << endl;
    return simulate_semester ();
  }

  if (!start_lst_specified)
  {
    cerr << "start LST not specified" << endl;
    return 0;
  }
     
  if (!end_lst_specified)
  {
    cerr << "end LST not specified" << endl;
    return 0;
  }

  cerr << "simulating session "
          "LST start=" << start_lst << " end=" << end_lst << endl;

  return simulate_session ();
}

int simulate_session ()
{
  Angle current_lst = start_lst;

  double one_minute = 1.0/(60.0*rad2hr);

  double ten_minutes = 10.0 * one_minute;

  // assume that it takes some time to start up and slew
  current_lst += ten_minutes;

  Mount* current_pointing = 0;

  double tsamp4_tot = 0;
  double tsamp16_tot = 0;

  double max_tsamp4 = 20;
  double max_tsamp16 = 90;

  cerr << "simulate_session: sources.size=" << sources.size() << endl;
  cerr << "current_lst=" << current_lst << " end_lst=" << end_lst << endl;

  while ( current_lst < end_lst )
  {
    horizon.set_local_sidereal_time( current_lst.getRadians() );

    if (current_pointing)
    {
      Horizon* horizon = dynamic_cast<Horizon*> (current_pointing);
      cerr << "before set_lst azimuth = " << horizon->get_azimuth() << endl;
      current_pointing -> set_local_sidereal_time( current_lst.getRadians() );
      cerr << "after set_lst azimuth = " << horizon->get_azimuth() << endl;
    }

    vector<source> up;
    for (unsigned i=0; i<sources.size(); i++)
    {
      horizon.set_source_coordinates( sources[i].coord );

      double elevation = 90 - (horizon.get_zenith() * rad2deg);

      // cerr << "coord=" << sources[i].coord << " elevation=" << elevation << endl;

      if (elevation < min_elevation)
        continue;

      // source is above horizon

      double desired_min = sources[i].Tint_min;

      if (press)
      {
        // PRESS specific
        if (sources[i].tsamp_us == 4 && desired_min > 20)
          desired_min = 20;
        else if (desired_min > 60)
          desired_min = 60.0;

        if ( sources[i].tsamp_us == 4 &&
             (tsamp16_tot > 0 || desired_min+tsamp4_tot > max_tsamp4) )
          continue;

        if (sources[i].tsamp_us == 16 &&
             (tsamp4_tot > 0 || desired_min+tsamp16_tot > max_tsamp16) )
          continue;

        // there is sufficent disk space
      }

      if (desired_min < 1)
        continue;

      source add = sources[i];

      add.Tobs = desired_min;

      double setting_in = (add.set_lst - current_lst).getRadians();
      double min_slew = 5.0; // minutes

      pair<double,Mount*> trial;

      if (current_pointing)
      {
        trial = best_slew (current_pointing, add.coord);
        min_slew = trial.first/60.0;
        add.slewtime = min_slew;
        add.mount = trial.second;
      }

      min_slew += 1.0;  // empirically guessed

      cerr << "sets in:" << setting_in / one_minute
           << " need:" << desired_min << " slew:" << min_slew
           << " minutes" << endl;

      if (desired_min + min_slew > setting_in / one_minute + 10.0)
         continue;

      up.push_back( add );
    }

    Angle lst = current_lst;
    lst.setWrapPoint( 2*M_PI );

    cerr << "at " << lst.getHMS(0)
         << " " << up.size() << " available sources" << endl;

    if (up.size() == 0)
      return -1;

    unsigned choose_index = 0;

    if (interactive)
    {
      std::sort (up.begin(), up.end(), by_priority);

      unsigned nshow = std::min( maxshow, up.size() );

      cerr << "NAME\t\tRANK\tSLEW" << endl;
      for (unsigned i=0; i<nshow; i++)
        cerr << i << " " << up[i].name << "\t" << up[i].priority
             << "\t" << up[i].slewtime << endl;

      char got = 0;
      while ( (got < '0' || got > '9') && got != 's' )
        got = getchar();

      if (got == 's')
      {
        cerr << "sorting by slew time" << endl;
        std::sort (up.begin(), up.end(), by_slewtime);

        cerr << "NAME\t\tRANK\tSLEW" << endl;
        for (unsigned i=0; i<nshow; i++) 
          cerr << i << " " << up[i].name << "\t" << up[i].priority
               << "\t" << up[i].slewtime << endl;

        while ( got < '0' || got > '9')
          got = getchar();
      }

      choose_index = got - '0';
      cerr << "chosen index =" << choose_index << endl;
    }
    else if (current_pointing && minimize_slew_time)
      std::sort (up.begin(), up.end(), by_slewtime);
    else
      std::sort (up.begin(), up.end(), by_priority);

    source chosen = up[choose_index];

    cout << lst.getHMS(0) << " " << chosen.name 
         << " tsamp=" << chosen.tsamp_us 
         << " Tint=" << chosen.Tobs
         << " slew=" << chosen.slewtime << endl;

    double slop = 3;  // 2 min CAL plus any other delays
    current_lst += (chosen.Tobs + chosen.slewtime + slop) * one_minute;

    if (press)
    {
      if (chosen.tsamp_us == 4) tsamp4_tot += chosen.Tobs;
      if (chosen.tsamp_us == 16) tsamp16_tot += chosen.Tobs;
    }

    for (unsigned j=0; j<sources.size(); j++)
      if (sources[j].name == chosen.name)
      {
        if (observe_once_per_session)
          sources.erase (sources.begin() + j);
        else
          sources[j].Tint_min -= chosen.Tobs;
        break;
      }

    for (unsigned j=0; j<orig_sources.size(); j++)
      if (orig_sources[j].name == chosen.name)
      {
        orig_sources[j].Tint_min -= chosen.Tobs;
        if (orig_sources[j].Tint_min <= 0)
          orig_sources.erase (orig_sources.begin() + j);
        break;
      }

    if (current_pointing)
    {
      Horizon* horizon = dynamic_cast<Horizon*> (current_pointing);
      cerr << "old azimuth = " << horizon->get_azimuth() << endl;

      current_pointing = chosen.mount;
      horizon = dynamic_cast<Horizon*> (current_pointing);
      cerr << "new azimuth = " << horizon->get_azimuth() << endl;
    }
    else
    {
      current_pointing = new Horizon(horizon);
      current_pointing->set_source_coordinates (chosen.coord);
    }

    Horizon* horizon = dynamic_cast<Horizon*> (current_pointing);
    cerr << "current azimuth = " << horizon->get_azimuth() << endl;

  }
  return 0;
}

class session
{
  public:
  float hours;
  Angle start_lst;
  session () { start_lst.setWrapPoint( 2*M_PI ); }
  void parse (string line)
  {
    string whitespace = " \t\n";
    string temp = stringtok (line, whitespace);
    hours = fromstring<float>(temp);

    temp = stringtok (line, whitespace);
    start_lst.setHMS (temp.c_str());
  }
};

int simulate_semester ()
{
  cerr << "Simulating semester file = " << semester << endl;

  vector<string> lines;
  loadlines (semester, lines);

  cerr << lines.size() << " lines of text loaded from " << semester << endl;

  vector<session> sem;
  sem.resize (lines.size());

  for (unsigned i=0; i<lines.size(); i++)
  {
    sem[i].parse( lines[i] );

    start_lst = sem[i].start_lst;
    end_lst = start_lst + sem[i].hours / rad2hr;

    sources = orig_sources;
 
    if (simulate_session () < 0)
    {
      cerr << "session #" << i << " of " << lines.size() << " failed" << endl;
      return -1;
    }
  }

  return 0;
}

void source::parse (string line)
{
  text = line;

  string whitespace = " \t\n";
  name = stringtok (line, whitespace);

  string temp = stringtok (line, whitespace);
  priority = fromstring<double>(temp);

  temp = stringtok (line, whitespace);
  coord.setHMSDMS (temp.c_str());

  temp = stringtok (line, whitespace);
  rise_lst.setHMS (temp.c_str());

  temp = stringtok (line, whitespace);
  set_lst.setHMS (temp.c_str());

  temp = stringtok (line, whitespace);
  up_hours = fromstring<double>(temp);

  temp = stringtok (line, whitespace);
  Tint_min = fromstring<double>(temp);

  temp = stringtok (line, whitespace);

  assert (temp.length() > 0);
  tsamp_us = fromstring<double>(temp);
}

double get_lst_density (const source& src)
{
  return src.Tint_min / ( src.up_hours * 60.0 );
}

void add_lst_density (vector<double>& lst_density, const source& src)
{
  unsigned nbin = lst_density.size();

  double density = get_lst_density (src);

  double rise_lst = src.rise_lst.getRadians();
  double set_lst = src.set_lst.getRadians();

  if (set_lst < rise_lst)
    set_lst += 2.0*M_PI;

  unsigned ibin_rise = round((rise_lst / (2*M_PI)) * nbin);
  unsigned ibin_set = round((set_lst / (2*M_PI)) * nbin);
  
  for (unsigned ibin=ibin_rise; ibin <= ibin_set; ibin++)
    lst_density[ibin%nbin] += density;
}

void output_lst_density (vector<double>& lst_density,
			 const vector<source>& sources)
{
  for (unsigned i=0; i<sources.size(); i++)
    add_lst_density( lst_density, sources[i] );
    
  ofstream out ("lst_density.txt");

  unsigned nbin = lst_density.size();
  double delta_lst = 2.0 * M_PI / nbin;

  double integral = 0.0;
  for (unsigned ibin=0; ibin < lst_density.size(); ibin++)
  {
    double lst = ibin * 24.0 / lst_density.size();
    out << lst << " " << lst_density[ibin] << endl;
    integral += lst_density[ibin] * delta_lst * rad2hr;
  }

  // integral /= lst_density.size();
  cerr << "integral=" << integral << endl;
}

void set_src_density (const vector<double>& lst_density, source& src)
{
  unsigned nbin = lst_density.size();

  double rise_lst = src.rise_lst.getRadians();
  double set_lst = src.set_lst.getRadians();

  if (set_lst < rise_lst)
    set_lst += 2.0*M_PI;

  unsigned ibin_rise = round((rise_lst / (2*M_PI)) * nbin);
  unsigned ibin_set = round((set_lst / (2*M_PI)) * nbin);
  
  double sum_density = 0.0;
  for (unsigned ibin=ibin_rise; ibin <= ibin_set; ibin++)
    sum_density += lst_density[ibin%nbin];

  src.min_avg_density = sum_density / (ibin_set - ibin_rise + 1);
}

void output_src_density (const vector<double>& lst_density,
			 vector<source>& sources)
{
  ofstream out ("src_density.txt");

  for (unsigned i=0; i<sources.size(); i++)
  {
    set_src_density (lst_density, sources[i]);

    out << sources[i].name << " " << sources[i].up_hours 
	<< " " << sources[i].min_avg_density << endl;

    sources[i].priority *= sources[i].min_avg_density;
  }
}

double metric (source& src)
{
  return sqrt(src.Tint_min) * pow(src.min_avg_density, 3.0);
}

void output_balanced_source_list (vector<source>& sources, float cutoff_hours)
{
  std::sort (sources.begin(), sources.end(), by_integration_length);

  if (cutoff_hours)
  {
    double total_hours = 0.0;
    unsigned count = 0;
    while (total_hours < cutoff_hours)
    {
      total_hours += sources[count].Tint_min / 60.0;      
      count ++;
    }
    cerr << "an unbalanced schedule can fit " << count << " sources in "
	 << total_hours << " hours" << endl;
  }
  
#ifdef _DEBUG
  for (unsigned i=0; i < 20; i++)
    cerr << "IN " << sources[i].text << endl;
#endif
  
  double interval = 15.0;  // minutes
  unsigned lst_bins = 60.0*24.0/interval;
  vector<double> lst_density (lst_bins, 0.0);

  vector<source> balanced_sources;

  unsigned next_source = 0;
  double total_hours = 0.0;
 
  do {

#ifdef _DEBUG
    cerr << "next=" << next_source << " " << sources[next_source].name << endl;
    for (unsigned i=0; i < 5; i++)
      cerr << "REM " << sources[i].text << endl;
#endif
    
    balanced_sources.push_back (sources[next_source]);
    add_lst_density (lst_density, sources[next_source]);

    total_hours += sources[next_source].Tint_min / 60.0;

    if (cutoff_hours && total_hours > cutoff_hours)
      break;

    sources.erase (sources.begin() + next_source);

    bool min_set = false;
    double min_val = 0.0;
    
    for (unsigned i=0; i<sources.size(); i++)
    {
      set_src_density (lst_density, sources[i]);
      double val = metric(sources[i]);
	
      if (!min_set || val < min_val)
      {
	min_val = val;
	next_source = i;
	min_set = true;
      }
    }

    if (min_set)
    {
      source& next = sources[next_source];
      cerr << "next=" << next_source << " " << next.name
	   << " Tint_min=" << next.Tint_min
	   << " val=" << metric(next) << endl;

      source& first = sources[0];
      cerr << "first " << first.name
	   << " Tint_min=" << first.Tint_min
	   << " val=" << metric(first) << endl;
    }
  }
  while (sources.size());

  ofstream out ("balance_sorted.txt");
  for (unsigned i=0; i<balanced_sources.size(); i++)
    out << balanced_sources[i].text << endl;

}
