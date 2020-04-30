/**************************************************************************2
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Horizon.h"
#include "stringtok.h"
#include "tostring.h"
#include "strutil.h"

#include <fstream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void usage ()
{
  cout <<
    "schedule - create a schedule\n"
    "  -l lst     start LST \n"
    "  -L lst     end LST \n"
    "  -p         create lst_density.txt and exit \n"
    "  -P         add PRESS-specific constraints \n"
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
  double min_avg_density;
  void parse (string line);
};

bool by_priority (const source& a, const source& b)
{
  return a.priority > b.priority;
}

int main (int argc, char* argv[]) 
{
  // some constants for conversions
  const double rad2deg = 180.0/M_PI;
  const double rad2hr = 12.0/M_PI;

  // the Horizon object stores telescope state
  Horizon horizon;

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

  double min_elevation = 30.3;
  bool verbose = false;

  Angle start_lst;
  start_lst.setWrapPoint( 2*M_PI );
  bool start_lst_specified = false;

  Angle end_lst;
  end_lst.setWrapPoint( 2*M_PI );
  bool end_lst_specified = false;

  vector<double> lst_density (180, 0.0);

  bool press = false;

  int c;
  while ((c = getopt(argc, argv, "he:l:L:pP")) != -1)
  {
    switch (c)
    {
    case 'h':
      usage ();
      return 0;

    case 'e':
      min_elevation = atof (optarg);
      break;

    case 'l':
      start_lst.setHMS(optarg);
      start_lst_specified = true;
      break;

    case 'L':
      end_lst.setHMS(optarg);
      end_lst_specified = true;
      break;

    case 'p':
      lst_density.resize (atoi(optarg));
      break;

    case 'P':
      press = true;
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

  vector<source> sources (lines.size());
  for (unsigned i=0; i<lines.size(); i++)
    sources[i].parse( lines[i] );

  double delta_lst = 2.0 * M_PI / lst_density.size();

  for (unsigned i=0; i<lines.size(); i++)
  {
    double density = sources[i].Tint_min / ( sources[i].up_hours * 60.0 );

    double rise_lst = sources[i].rise_lst.getRadians();
    double set_lst = sources[i].set_lst.getRadians();

    if (set_lst < rise_lst)
      set_lst += 2.0*M_PI;

    unsigned nbin = lst_density.size();

    for (double lst=rise_lst; lst <= set_lst; lst += delta_lst)
    {
      unsigned ibin = (lst / (2*M_PI)) * nbin;
      lst_density[ibin%nbin] += density;
    }
  }

  ofstream out ("lst_density.txt");
  double integral = 0.0;
  for (unsigned ibin=0; ibin < lst_density.size(); ibin++)
  {
    double lst = ibin * 24.0 / lst_density.size();
    out << lst << " " << lst_density[ibin] << endl;
    integral += lst_density[ibin] * delta_lst * rad2hr;
  }

  // integral /= lst_density.size();
  cerr << "integral=" << integral << endl;

  ofstream sout ("src_density.txt");

  for (unsigned i=0; i<lines.size(); i++)
  {
    double rise_lst = sources[i].rise_lst.getRadians();
    double set_lst = sources[i].set_lst.getRadians();

    if (set_lst < rise_lst)
      set_lst += 2.0*M_PI;

    unsigned nbin = lst_density.size();

    // find the minimum density window in which the source can be observed
    double min_avg_density = 0.0;
    for (double lst1=rise_lst; lst1 <= set_lst; lst1 += delta_lst)
    {
      double sum_density = 0.0;
      double sum_time = 0.0;
      double lst2 = lst1 + sources[i].Tint_min / (60 * rad2hr);
      double lst = lst1;
      while (lst <= lst2 && lst <= set_lst)
      {
        unsigned ibin = (lst / (2*M_PI)) * nbin;
        sum_density += lst_density[ibin%nbin];
        sum_time += delta_lst * rad2hr;
        lst += delta_lst;
      }

      double avg_density = sum_density / sum_time;

      if (lst1==rise_lst || avg_density < min_avg_density)
        min_avg_density = avg_density;

      if (lst >= set_lst)
        break;
    }

    sources[i].min_avg_density += min_avg_density;
    sout << sources[i].name << " " << sources[i].up_hours << " " << sources[i].min_avg_density << endl;

    sources[i].priority *= min_avg_density;
  }

  if (!(end_lst_specified && start_lst_specified))
    return 0;

  Angle current_lst = start_lst;

  double one_minute = 1.0/(60.0*rad2hr);

  double ten_minutes = 1.0/(6.0*rad2hr);

  // assume that it takes some time to start up and slew
  current_lst += ten_minutes;

  Mount* current_pointing = &horizon;

  double tsamp4_tot = 0;
  double tsamp16_tot = 0;

  double max_tsamp4 = 20;
  double max_tsamp16 = 90;

  bool no_more_highres = false;

  while ( current_lst < end_lst )
  {
    horizon.set_local_sidereal_time( current_lst.getRadians() );

    vector<source> up;
    for (unsigned i=0; i<sources.size(); i++)
    {
      horizon.set_source_coordinates( sources[i].coord );

      double elevation = 90 - (horizon.get_zenith() * rad2deg);

      // cerr << "coord=" << sources[i].coord << " elevation=" << elevation << endl;

      if (elevation > min_elevation)
        up.push_back( sources[i] );
    }

    Angle lst = current_lst;
    lst.setWrapPoint( 2*M_PI );

    cout << "at " << lst.getHMS(0)
         << " " << up.size() << " sources above horizon" << endl;

    std::sort (up.begin(), up.end(), by_priority);

    for (unsigned i=0; i<up.size(); i++)
    {
      double desired_min = up[i].Tint_min;

      if (press)
      {
        // PRESS specific
        if (up[i].tsamp_us == 4 && desired_min > 20)
          desired_min = 20;
        else if (desired_min > 60) 
          desired_min = 60.0;

        if ( up[i].tsamp_us == 4 && 
             (tsamp16_tot > 0 || desired_min+tsamp4_tot > max_tsamp4) )
          continue;

        if (up[i].tsamp_us == 16 && 
             (tsamp4_tot > 0 || desired_min+tsamp16_tot > max_tsamp16) )
          continue;
      }

      double setting_in = (up[i].set_lst - current_lst).getRadians();

      vector< pair<double,Mount*> > slew_times;
      slew_times = current_pointing->slew_times (up[i].coord);

      double min_slew = slew_times[0].first/60.0;
      current_pointing = slew_times[0].second;

      for (unsigned i=1; i<slew_times.size(); i++)
      {
        double min = slew_times[i].first/60.0;
        if (min < min_slew)
        {
          min_slew = min;
          current_pointing = slew_times[i].second;
        }
      }

      min_slew += 1.0;  // empirically guessed

      cerr << "sets in:" << setting_in / one_minute 
           << " need:" << desired_min << " slew:" << min_slew 
           << " minutes" << endl;

      if (desired_min + min_slew < setting_in / one_minute + 10.0)
      {
        Angle lst = current_lst;
        lst.setWrapPoint( 2*M_PI );

        cout << lst.getHMS(0) << " " << up[i].name 
             << " tsamp=" << up[i].tsamp_us 
             << " Tint=" << desired_min
             << " slew=" << min_slew << endl;

        double slop = 3;  // 2 min CAL plus any other delays
        current_lst += (desired_min + min_slew + slop) * one_minute;

        if (press)
        {
          if (up[i].tsamp_us == 4) tsamp4_tot += desired_min;
          if (up[i].tsamp_us == 16) tsamp16_tot += desired_min;
        }

        for (unsigned j=0; j<sources.size(); j++)
          if (sources[j].name == up[i].name)
          {
            sources.erase (sources.begin() + j);
            break;
          }

        break;
      }
    }
  }
  return 0;
}


void source::parse (string line)
{
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

