#ifndef CELQUADMAP_H
#define CELQUADMAP_H

/******************************************************************************
**
**  CelQuadMap
**  
**  Author: Christophe Geoffroy (Topper) – MIT License
**  Last updated: 08-02-2025
**
**  Fills HDX/HDY in 12.20 format, VDX/VDY in 16.16, HDDX/HDDY in 12.20
**  using a quadrilateral in Q16.16 format.
**
******************************************************************************/

// CCB structure
#include "graphics.h"
// Point2D, Quad2D structures
#include "Geometry.h"
// int32 type
#include "types.h"

// Applies a transformation to a CCB based on a quadrilateral in 16.16 format
void CelQuadMap(CCB *ccb, Quad2D *quadMap);

#endif // CELQUADMAP_H
