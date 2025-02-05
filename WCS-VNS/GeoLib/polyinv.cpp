/*******************************************************************************
NAME                            POLYCONIC 

PURPOSE:        Transforms input Easting and Northing to longitude and
                latitude for the Polyconic projection.  The
                Easting and Northing must be in meters.  The longitude
                and latitude values will be returned in radians.

PROGRAMMER      DATE            REASON
----------      ----            ------
T. Mittan       Mar, 1993
S. Nelson       Jan, 1998       If an error occurs in the phi4z call from
                                polyinv, the error flag number from phi4z
                                will be returned instead of 74 as before.

ALGORITHM REFERENCES

1.  Snyder, John P., "Map Projections--A Working Manual", U.S. Geological
    Survey Professional Paper 1395 (Supersedes USGS Bulletin 1532), United
    State Government Printing Office, Washington D.C., 1987.

2.  Snyder, John P. and Voxland, Philip M., "An Album of Map Projections",
    U.S. Geological Survey Professional Paper 1453 , United State Government
    Printing Office, Washington D.C., 1989.
*******************************************************************************/
#include "cproj.h"

// Initialize the POLYCONIC projection
long Projectoid::polyinvint(
double r_maj,					// major axis
double r_min,					// minor axis
double center_lon_init,			// center longitude
double center_lat_init,			// center latitude
double false_east,				// x offset in meters
double false_north)				// y offset in meters
{
double temp;                    // temporary variable

// Place parameters in static storage for common use
r_major = r_maj;
r_minor = r_min;
lon_center = center_lon_init;
lat_origin = center_lat_init;
false_northing = false_north;
false_easting = false_east;

temp = r_minor / r_major;
es = 1.0 - SQUARE(temp);
e = sqrt(es);
e0 = e0fn(es);
e1 = e1fn(es);
e2 = e2fn(es);
e3 = e3fn(es);
ml0 = mlfn(e0, e1, e2, e3, lat_origin);

// Report parameters to the user
ptitle("POLYCONIC"); 
radius2(r_major, r_minor);
cenlonmer(lon_center);
origin(lat_origin);
offsetp(false_easting, false_northing);
InverseOK[WCS_PROJECTIONCODE_POLYC] = 1;
InverseTransform = &Projectoid::polyinv;

return(OK);

}

/*===========================================================================*/

// Polyconic inverse equations--mapping x,y to lat/long
long Projectoid::polyinv(
double x,						// (O) X projection coordinate
double y,						// (O) Y projection coordinate
double *lon,					// (I) Longitude
double *lat)					// (I) Latitude
{
double al;			// temporary values
double b;			// temporary values
double c;			// temporary values
double t;			// <<<F2 OPT>>>
long iflg;			// error flag

// Inverse equations
x -= false_easting;
y -= false_northing;
al = ml0 + y / r_major;
iflg = 0;
t = x / r_major;
if (fabs(al) <= .0000001)
	{
	*lon = t + lon_center;
	*lat = 0.0;
	}
else
	{
	b = al * al + t * t;
	iflg = phi4z(es, e0, e1, e2, e3, al, b, &c, lat);
	if (iflg != OK)
		return(iflg);
	*lon = adjust_lon((asinz(t * c) / sin(*lat)) + lon_center);
	}

return(OK);

}
