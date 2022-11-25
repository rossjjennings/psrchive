
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Observatory.h"
#include "strutil.h"
#include "debug.h"

#include <strings.h>
#include <fstream>
#include <sstream>

#include <math.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

Tempo2::Observatory::Observatory ()
{
  code = 0;
  x = y = z = 0;
}

char Tempo2::Observatory::get_code () const
{
  return code;
}

void Tempo2::Observatory::set_code (char c)
{
  code = c;
}

bool Tempo2::Observatory::is_alias_of(const std::string& nn) const {
    const char* c = nn.c_str();
    if(nn.size() == 1 && this->get_code()==nn[0]) {
        return true;
    }
    if(strcasecmp(this->name.c_str(),c)==0) {
        return true;
    }
    if(strcasecmp(this->old_code.c_str(),c)==0) {
        return true;
    }
    for(std::vector<std::string>::const_iterator it = this->aliases.begin(); it != this->aliases.end(); ++it) {
        if (strcasecmp(it->c_str(),c) == 0) {
            return true;
        }
    }
    return false;
}

std::string Tempo2::Observatory::get_old_code () const
{
    return old_code;
}

void Tempo2::Observatory::set_old_code (const std::string& c)
{
    old_code = c;
}

std::string Tempo2::Observatory::get_name () const
{
    return name;
}

void Tempo2::Observatory::set_name (const std::string& n)
{
    name = n;
}

void Tempo2::Observatory::set_xyz (double _x, double _y, double _z)
{
    x = _x; y = _y; z = _z;
}

void Tempo2::Observatory::get_xyz (double& _x, double& _y, double& _z) const
{
    _x = x; _y = y; _z = z;
}

//! Get the latitude and longitude in radians
void Tempo2::Observatory::get_sph (double& lat,
        double& lon,
        double& rad) const
{
#ifdef _DEBUG
    cerr << "Tempo2::Observatory::get_sph"
        " x=" << x << " y=" << y << " z=" << z << endl;
#endif

    rad = sqrt(x*x + y*y + z*z);
    lat = asin (z/rad);
    lon = atan2 (y, x);
}

static vector< Reference::To<Tempo2::Observatory> > antennae;

static void load_observatories ();

const Tempo2::Observatory*
Tempo2::observatory (const string& telescope_name)
{
    load_observatories ();

    DEBUG("Tempo2::observatory name='" << telescope_name << "'");

    // simpler ask the object to check if it is an alias
    for (unsigned i=0; i < antennae.size(); i++) {
        if (antennae[i]->is_alias_of(telescope_name)){
            return antennae[i];
        }
    }

    throw Error (InvalidParam, "Tempo2::observatory",
            "no antennae named '" + telescope_name + "'");
}

static void load_aliases ()
{
    char* tempo2_dir = getenv ("TEMPO2");
    if (!tempo2_dir)
        throw Error (FailedSys, "Tempo2::load_aliases",
                "TEMPO2 environment variable not defined");

    string filename = tempo2_dir + string("/observatory/aliases");

    ifstream input (filename.c_str());
    if (!input)
        throw Error (FailedSys, "Tempo2::load_aliases",
                "ifstream (" + filename + ")");

    std::string line;

    while (!input.eof())
    {
        getline (input, line);
        line = stringtok (line, "#\n", false);  // get rid of comments

        if (!line.length())
            continue;

        std::string buf; // Have a buffer string
        std::stringstream ss(line); // Insert the string into a stream

        std::vector<std::string> tokens; // Create vector to hold our words

        while (ss >> buf)
            tokens.push_back(buf);

        for (unsigned i=0; i < antennae.size(); i++){
            if (antennae[i]->is_alias_of(tokens.front())){
                // append the content of the alias list
                antennae[i]->aliases.insert(antennae[i]->aliases.end(),tokens.begin()+1,tokens.end());
                for(std::vector<std::string>::iterator it = tokens.begin()+1; it != tokens.end(); ++it) {
                    if (it->size() == 1){
                        antennae[i]->set_code((*it)[0]);
                        break;
                    }
                }
                break;
            }
        }
    }
}

void load_observatories ()
{
    static bool observatories_loaded = false;

    if (observatories_loaded)
        return;

    char* tempo2_dir = getenv ("TEMPO2");
    if (!tempo2_dir)
        throw Error (FailedSys, "Tempo2::load_observatories",
                "TEMPO2 environment variable not defined");

    string filename = tempo2_dir + string("/observatory/observatories.dat");

    ifstream input (filename.c_str());
    if (!input)
        throw Error (FailedSys, "Tempo2::load_observatories",
                "ifstream (" + filename + ")");

    string line;

    while (!input.eof())
    {
        getline (input, line);
        line = stringtok (line, "#\n", false);  // get rid of comments

        if (!line.length())
            continue;

        istringstream istr (line);

        double x, y, z;
        istr >> x >> y >> z;

        string name, code;
        istr >> name >> code;

        if (istr.fail())
            throw Error (InvalidParam, "Tempo::observatories",
                    "failed to parse '" + line + "'");

        Reference::To<Tempo2::Observatory> observatory = new Tempo2::Observatory;

        observatory->set_xyz (x, y, z);
        observatory->set_name( name );
        observatory->set_old_code( code );

        antennae.push_back( observatory );
    }

    load_aliases ();

    observatories_loaded = true;
}
