#ifndef LIBRARY_GEOMETRY_H
#define LIBRARY_GEOMETRY_H

// int32
#include "types.h"
// CCB
#include "graphics.h"

// Alignement pour le positionnement
#define LAYER_ALIGN_X_LEFT 0
#define LAYER_ALIGN_X_CENTER 1
#define LAYER_ALIGN_X_RIGHT 2
#define LAYER_ALIGN_Y_TOP 0
#define LAYER_ALIGN_Y_CENTER 1
#define LAYER_ALIGN_Y_BOTTOM 2

typedef struct Point2D {
	int32 x;
	int32 y;
} Point2D;

typedef struct Quad2D {
	Point2D	topLeft;
	Point2D	topRight;
	Point2D	bottomRight;
	Point2D	bottomLeft;
} Quad2D;

typedef struct Rectangle {
	Point2D	topLeft;
	Point2D	bottomRight;
} Rectangle;

// Obtient le point central entre deux points
void CenterPoint2D(Point2D *center, Point2D *point1, Point2D *point2);

// Centre le point dans le rectangle 
// -> Modifie directement la position du point
void CenterPoint2DInRectangle(Rectangle *rectangle, Point2D *point);

void RotatePoint2D(int32 xi, int32 yi, int32 cx, int32 cy, int32 sinA, int32 cosA, int32 *ox, int32 *oy);

// Centre le rectangle sur le point 
// -> Modifie directement la position du rectangle
void CenterRectangleOverPoint2D(Point2D *point, Rectangle *rectangle);

// Positionne un rectangle enfant dans un rectangle parent en fonction d'une référence d'alignement
// -> Modifie directement la position du rectangle enfant
void PositionRectangleInRectangle(Rectangle *rectangle1, Rectangle *rectangle2, uint32 alignX, uint32 alignY, int32 offsetX, int32 offsetY);

// Contraint le rectangle dans un cadre 
// -> Modifie directement la position du rectangle
void BoundsRectangle(Rectangle *boundary, Rectangle *rectangle);

// Décale le point selon la valeur en X/Y du point offset
// -> Modifie directement la position du point
void OffsetPoint2D(Point2D *point, Point2D *offset);

// Décale le rectangle selon la valeur en X/Y du point
// -> Modifie directement la position du rectangle
void OffsetRectangle(Rectangle *rectangle, Point2D *offset);

// Le point est-il dans le rectangle
// -> Retourne 1 si le point est dans le rectangle, 0 si ce n'est pas le cas
int32 Point2DInRectangle(Rectangle *rectangle, Point2D *point);

// Intersection de deux rectangles
// -> Retourne 1 si les rectangles se chevauchent, 0 si ce n'est pas le cas
int32 RectanglesIntersection(Rectangle *rectangle1, Rectangle *rectangle2);

/* Manipulation de Cel */

// Etirement d'un Cel selon un multiplicateur
// -> Modifie directement les valeurs ccb_HDX et ccb_VDY du Cel
void CelScale(CCB *cel, int32 scaleX, int32 scaleY);

// Etirement d'un Cel selon une taille cible en pixels
// -> Modifie directement les valeurs ccb_HDX et ccb_VDY du Cel
void CelPixelScale(CCB *cel, uint32 sizeX, uint32 sizeY);

// Supprime l'étirement d'un Cel
// -> Modifie directement les valeurs ccb_HDX et ccb_VDY du Cel
void CelUnscale(CCB *cel, uint32 unscaleX, uint32 unscaleY);

// Retourne un Cel horizontalement avec ou sans correction de la position
// -> Modifie directement les valeurs ccb_HDX et ccb_XPos du Cel
void CelFlipHorizontal(CCB *cel, int32 flip, uint32 correction);

// Retourne un Cel verticalement avec ou sans correction de la position
// -> Modifie directement les valeurs ccb_VDY et ccb_YPos du Cel
void CelFlipVertical(CCB *cel, int32 flip, uint32 correction);

// Retourne le rectangle occupé à l'écran par le Cel transformé
// -> Modifie directement les valeurs du rectangle 
void CelProjection(CCB *cel, Rectangle *rectangle);

#endif // LIBRARY_GEOMETRY_H