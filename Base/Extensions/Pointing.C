#include "Pulsar/Pointing.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Telescope.h"

#include "string_utils.h"
#include "coord.h"

//! Default constructor
Pulsar::Pointing::Pointing () : Extension ("Pointing")
{
  lst_sub = 0.0;
}

//! Copy constructor
Pulsar::Pointing::Pointing (const Pointing& extension) : Extension ("Pointing")
{
  operator = (extension);
}

//! Operator =
const Pulsar::Pointing&
Pulsar::Pointing::operator= (const Pointing& extension)
{
  lst_sub  = extension.lst_sub;
  ra_sub   = extension.ra_sub;
  dec_sub  = extension.dec_sub;
  glon_sub = extension.glon_sub;
  glat_sub = extension.glat_sub;
  fd_ang   = extension.fd_ang;
  pos_ang  = extension.pos_ang;
  par_ang  = extension.par_ang;
  tel_az   = extension.tel_az;
  tel_zen  = extension.tel_zen;
 
  return *this;
}

//! Destructor
Pulsar::Pointing::~Pointing ()
{
}

/*! Based on the epoch of the Integration, uses slalib to re-calculate
  the following Pointing attributes: lst_sub, par_ang, tel_az, and
  tel_zen. */
void Pulsar::Pointing::update (const Integration* subint)
{
  const Archive* archive = get_parent (subint);

  if (!archive)
    throw (InvalidState, "Pulsar::Pointing::update",
	   "Integration has no parent Archive");

  const Telescope* telescope = archive->get<Telescope>();

  if (!telescope)
    throw (InvalidState, "Pulsar::Pointing::update",
	   "parent Archive has no telescope Extension");

  // correct the Pointing LST (in hours)
  lst_sub = subint->get_epoch().LST (telescope->get_longitude().getDegrees());
    
  // correct the Pointing azimuth, zenith, and parallactic angles
  float azimuth=0, zenith=0, parallactic=0;
  if (az_zen_para (ra_sub.getRadians(), dec_sub.getRadians(), lst_sub,
		   telescope->get_latitude().getDegrees(),
		   &azimuth, &zenith, &parallactic) < 0)
    throw (FailedCall, "Pulsar::Pointing::update", "az_zen_para");
	   
  par_ang.setDegrees( parallactic );
  tel_az.setDegrees( azimuth );
  tel_zen.setDegrees( zenith );

  // correct the Pointing LST (in seconds)
  lst_sub *= 3600.0;

}


/*! Calculates the mean of the Pointing attributes. */
void Pulsar::Pointing::integrate (const Integration* subint)
{ 
  const Pointing* useful = subint->get<Pointing>();

  if (!useful) {
    if (Integration::verbose)
      cerr << "Pulsar::Pointing::integrate subint has no Pointing" << endl;
    return;
  }

  lst_sub += useful->lst_sub;
  lst_sub /= 2.0;

  ra_sub += useful->ra_sub;
  ra_sub /= 2.0;
  
  dec_sub += useful->dec_sub;
  dec_sub /= 2.0;

  glon_sub += useful->glon_sub;
  glon_sub /= 2.0;
  
  glat_sub += useful->glat_sub;
  glat_sub /= 2.0;
  
  fd_ang += useful->fd_ang;
  fd_ang /= 2.0;

  pos_ang += useful->pos_ang;
  pos_ang /= 2.0;

  par_ang += useful->par_ang;
  par_ang /= 2.0;
  
  tel_az += useful->tel_az;
  tel_az /= 2.0;

  tel_zen += useful->tel_zen;
  tel_zen /= 2.0;
}

