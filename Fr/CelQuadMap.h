#ifndef CELQUADMAP_H
#define CELQUADMAP_H

/******************************************************************************
**
**  CelQuadMap
**  
**  Auteur : Christophe Geoffroy (Topper) - Licence MIT
**  Dernière mise à jour : 02-08-2025
**
**  Remplit HDX/HDY en 12.20, VDX/VDY en 16.16, HDDX/HDDY en 12.20
**  à partir d'un quadrilatère en Q16.16.
**
******************************************************************************/

// CCB
#include "graphics.h"
// Point2D, Quad2D
#include "Geometry.h"
// int32
#include "types.h"

// Applique une transformation d'un CCB en fonction d'un quadrilatère en 16.16 
void CelQuadMap(CCB *ccb, Quad2D *quadMap);

#endif // CELQUADMAP_H