/*
  This file provides C wrappers to the Fortran SLALIB bindings.  It is
  compiled and included in the library only if the C bindings are not
  present in slalib.a
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* *********************************************************************** */

#define SLA_altaz SLA_FUNC(sla_altaz,slaAltaz)
void SLA_altaz (double*, double*, double*, double*, double*, double*,
		double*, double*, double*, double*, double*, double*);

void slaAltaz (double ha, double dec, double phi,
                double *az, double *azd, double *azdd,
                double *el, double *eld, double *eldd,
                double *pa, double *pad, double *padd)
{
  SLA_altaz (&ha, &dec, &phi,
	     az, azd, azdd,
	     el, eld, eldd,
	     pa, pad, padd);
}


/* *********************************************************************** */

#define SLA_dcs2c SLA_FUNC(sla_dcs2c,slaDcs2c)
void SLA_dcs2c (double* ra, double* dec, double* v);

void slaDcs2c (double a, double b, double v[3])
{
  SLA_dcs2c (&a, &b, v);
}


/* *********************************************************************** */

#define SLA_dmxv SLA_FUNC(sla_dmxv,slaDmxv)
void SLA_dmxv (double* dm, double* va, double* vb);

void slaDmxv (double dm[3][3], double va[3], double vb[3])
{
  SLA_dmxv (&(dm[0][0]), va, vb);
}

/* *********************************************************************** */

#define SLA_dsep SLA_FUNC(sla_dsep,slaDsep)
double SLA_dsep (double *, double *, double *, double*);

double slaDsep (double a1, double b1, double a2, double b2)
{
  return SLA_dsep (&a1, &b1, &a2, &b2);
}

/* *********************************************************************** */

#define SLA_dtt SLA_FUNC(sla_dtt,slaDtt)
double SLA_dtt (double* mjd);

double slaDtt (double dju)
{
  return SLA_dtt (&dju);
}

/* *********************************************************************** */

#define SLA_dvdv SLA_FUNC(sla_dvdv,slaDvdv)
double SLA_dvdv (double* va, double* vb);

double slaDvdv (double va[3], double vb[3])
{
  return SLA_dvdv (va, vb);
}

/* *********************************************************************** */

#define SLA_epj SLA_FUNC(sla_epj,slaEpj)
double SLA_epj (double* mjd);

double slaEpj (double date)
{
  return SLA_epj (&date);
}

/* *********************************************************************** */

#define SLA_eqgal SLA_FUNC(sla_eqgal,slaEqgal)
double SLA_eqgal (double *, double *, double *, double *);

void slaEqgal (double dr, double dd, double *dl, double *db)
{
  SLA_eqgal (&dr, &dd, dl, db);
}

/* *********************************************************************** */

#define SLA_evp SLA_FUNC(sla_evp,slaEvp)
void SLA_evp (double* tdb, double* ep,
              double* dvb, double* dpb,
              double* dvh, double* dph);

void slaEvp (double date, double deqx,
             double dvb[3], double dpb[3],
             double dvh[3], double dph[3])
{
  SLA_evp (&date, &deqx, dvb, dpb, dvh, dph);
}

/* *********************************************************************** */

#define SLA_galeq SLA_FUNC(sla_galeq,slaGaleq)
double SLA_galeq (double *, double *, double *, double *);

void slaGaleq (double dl, double db, double *dr, double *dd)
{
  return SLA_galeq (&dl, &db, dr, dd);
}

/* *********************************************************************** */

#define SLA_gmst SLA_FUNC(sla_gmst,slaGmst)
double SLA_gmst (double* mjd);

double slaGmst (double ut1)
{
  return SLA_gmst (&ut1);
}

/* *********************************************************************** */

#define SLA_pa SLA_FUNC(sla_pa,slaPa)
double SLA_pa (double* HA, double* DEC, double* PHI);

double slaPa (double ha, double dec, double phi)
{
  return SLA_pa (&ha, &dec, &phi);
}

/* *********************************************************************** */

#define SLA_prec SLA_FUNC(sla_prec,slaPrec)
void SLA_prec (double* ep0, double* ep1, double* rmatp);

void slaPrec (double ep0, double ep1, double rmatp[3][3])
{
  SLA_prec (&ep0, &ep1, &(rmatp[0][0]));
}

