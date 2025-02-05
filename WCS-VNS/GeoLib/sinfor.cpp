/*******************************************************************************
NAME                            SINUSOIDAL

PURPOSE:        Transforms input longitude and latitude to Easting and
                Northing for the Sinusoidal projection.  The
                longitude and latitude must be in radians.  The Easting
                and Northing values will be returned in meters.

PROGRAMMER              DATE            
----------              ----           
D. Steinwand, EROS      May, 1991     

This function was adapted from the Sinusoidal projection code (FORTRAN) in the 
General Cartographic Transformation Package software which is available from 
the U.S. Geological Survey National Mapping Division.
 
ALGORITHM REFERENCES

1.  Snyder, John P., "Map Projections--A Working Manual", U.S. Geological
    Survey Professional Paper 1395 (Supersedes USGS Bulletin 1532), United
    State Government Printing Office, Washington D.C., 1987.

2.  "Software Documentation for GCTP General Cartographic Transformation
    Package", U.S. Geological Survey National Mapping Division, May 1982.
*******************************************************************************/
#include "cproj.h"

// Initialize the Sinusoidal projection
long Projectoid::sinforint(
double r,                       // (I) Radius of the earth (sphere)
double center_long,             // (I) Center longitude
double false_east,              // x offset in meters
double false_north)             // y offset in meters
{
// Place parameters in static storage for common use
R = r;
lon_center = center_long;
false_easting = false_east;
false_northing = false_north;

// Report parameters to the user
ptitle("SINUSOIDAL");
radius(r);
cenlon(center_long);
offsetp(false_easting, false_northing);
ForwardOK[WCS_PROJECTIONCODE_SNSOID] = 1;
ForwardTransform = &Projectoid::sinfor;

return(OK);

}

/*===========================================================================*/

// Sinusoidal forward equations--mapping lat,long to x,y
long Projectoid::sinfor(
double lon,                     // (I) Longitude
double lat,                     // (I) Latitude
double *x,                      // (O) X projection coordinate
double *y)                      // (O) Y projection coordinate
{
double delta_lon;       // Delta longitude (Given longitude - center)

// Forward equations
delta_lon = adjust_lon(lon - lon_center);
*x = R * delta_lon * cos(lat) + false_easting;
*y = R * lat + false_northing;

return(OK);

}
