#include "Pulsar/Pointing.h"
#include "string_utils.h"

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

void Pulsar::Pointing::append (Extension* ext)
{
  Pulsar::Pointing* useful = 
    dynamic_cast<Pulsar::Pointing*>(ext);

  if (!useful)
    return;

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

