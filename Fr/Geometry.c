
#include "Geometry.h"

// Convert32_F16, ConvertF16_32
#include "operamath.h"

// Obtient le point central entre deux points
// -> Modifie directement la position du centre
void CenterPoint2D(Point2D *center, Point2D *point1, Point2D *point2) {

	// Calcule le point central
    center->x = (point1->x + point2->x) / 2;
    center->y = (point1->y + point2->y) / 2;

}

// Centre le point dans le rectangle 
// -> Modifie directement la position du point
void CenterPoint2DInRectangle(Rectangle *rectangle, Point2D *point) {
	
	// Calcule le point central en X
	point->x = rectangle->topLeft.x + (rectangle->bottomRight.x - rectangle->topLeft.x) / 2;
	// Calcule le point central en Y
	point->y = rectangle->topLeft.y + (rectangle->bottomRight.y - rectangle->topLeft.y) / 2;
	
}

void RotatePoint2D(int32 xi, int32 yi, int32 cx, int32 cy, int32 sinA, int32 cosA, int32 *ox, int32 *oy) {
    // delta au pivot en Q16.16
    int32 dx = 0;
    int32 dy = 0;

    // rotation [ cos -sin ; sin  cos ] en Q16.16
    int32 rx = 0;
    int32 ry = 0;
	
    // delta au pivot en Q16.16
    dx = Convert32_F16(xi - cx);
    dy = Convert32_F16(yi - cy);

    // rotation [ cos -sin ; sin  cos ] en Q16.16
	rx = (int32)MulSF16(dx, cosA) - (int32)MulSF16(dy, sinA);
	ry = (int32)MulSF16(dx, sinA) + (int32)MulSF16(dy, cosA);
	

    // retour en pixels, recentrage
    *ox = ConvertF16_32(rx) + cx;
    *oy = ConvertF16_32(ry) + cy;
}

// Centre le rectangle sur le point 
// -> Modifie directement la position du rectangle
void CenterRectangleOverPoint2D(Point2D *point, Rectangle *rectangle) {
	
    // Calcule la largeur et la hauteur du rectangle
    int32 width = rectangle->bottomRight.x - rectangle->topLeft.x;
    int32 height = rectangle->bottomRight.y - rectangle->topLeft.y;
    
    // Calcule les nouvelles positions de topLeft et bottomRight pour centrer le rectangle sur le point
    rectangle->topLeft.x = point->x - width / 2;
    rectangle->topLeft.y = point->y - height / 2;
    rectangle->bottomRight.x = point->x + width / 2;
    rectangle->bottomRight.y = point->y + height / 2;
	
}

// Positionne un rectangle enfant dans un rectangle parent en fonction d'une référence d'alignement
// -> Modifie directement la position du rectangle enfant
void PositionRectangleInRectangle(Rectangle *rectangle1, Rectangle *rectangle2, uint32 alignX, uint32 alignY, int32 offsetX, int32 offsetY) {
	
    // Calcule la largeur et la hauteur du rectangle
    int32 width = rectangle2->bottomRight.x - rectangle2->topLeft.x;
    int32 height = rectangle2->bottomRight.y - rectangle2->topLeft.y;
    
    // Ajuste la position horizontale du coin supérieur gauche du rectangle enfant
    switch (alignX) {

        case LAYER_ALIGN_X_CENTER:
            //rectangle2->topLeft.x = rectangle1->topLeft.x + (rectangle1->bottomRight.x - rectangle1->topLeft.x) / 2 - (rectangle2->bottomRight.x - rectangle2->topLeft.x) / 2 + offsetX;
			rectangle2->topLeft.x = (rectangle1->bottomRight.x - rectangle1->topLeft.x - width) / 2 + offsetX;

            break;

        case LAYER_ALIGN_X_RIGHT:
            //rectangle2->topLeft.x = rectangle1->bottomRight.x - (rectangle2->bottomRight.x - rectangle2->topLeft.x) - offsetX;
			rectangle2->topLeft.x = rectangle1->bottomRight.x - width - offsetX;
            break;
			
        // LAYER_ALIGN_X_LEFT:
		default:
            rectangle2->topLeft.x = offsetX;
            break;
    }

    // Ajuste la position verticale du coin supérieur gauche du rectangle enfant
    switch (alignY) {

        case LAYER_ALIGN_Y_CENTER:
            //rectangle2->topLeft.y = rectangle1->topLeft.y + (rectangle1->bottomRight.y - rectangle1->topLeft.y) / 2 - (rectangle2->bottomRight.y - rectangle2->topLeft.y) / 2 + offsetY;
			rectangle2->topLeft.y = (rectangle1->bottomRight.y - rectangle1->topLeft.y - height) / 2 + offsetY;
            break;

        case LAYER_ALIGN_Y_BOTTOM:
            //rectangle2->topLeft.y = rectangle1->bottomRight.y - (rectangle2->bottomRight.y - rectangle2->topLeft.y) - offsetY;
			rectangle2->topLeft.y = rectangle1->bottomRight.y - height - offsetY;
            break;
			
        // LAYER_ALIGN_Y_TOP:
		default:
            rectangle2->topLeft.y = offsetY;
            break;
    }

    // Mets à jour le coin inférieur droit du rectangle enfant
    rectangle2->bottomRight.x = rectangle2->topLeft.x + width;
    rectangle2->bottomRight.y = rectangle2->topLeft.y + height;
}


// Contraint le rectangle dans un cadre 
// -> Modifie directement la position du rectangle
void BoundsRectangle(Rectangle *boundary, Rectangle *rectangle) {
	
	// Valeur de déplacement en X/Y
    Point2D offset;

    // Ajuste la position horizontale
    if (rectangle->topLeft.x < boundary->topLeft.x) {
        offset.x = boundary->topLeft.x - rectangle->topLeft.x;
        rectangle->topLeft.x += offset.x;
        rectangle->bottomRight.x += offset.x;
    } else if (rectangle->bottomRight.x > boundary->bottomRight.x) {
        offset.x = rectangle->bottomRight.x - boundary->bottomRight.x;
        rectangle->topLeft.x -= offset.x;
        rectangle->bottomRight.x -= offset.x;
    }

    // Ajuste la position verticale
    if (rectangle->topLeft.y < boundary->topLeft.y) {
        offset.y = boundary->topLeft.y - rectangle->topLeft.y;
        rectangle->topLeft.y += offset.y;
        rectangle->bottomRight.y += offset.y;
    } else if (rectangle->bottomRight.y > boundary->bottomRight.y) {
        offset.y = rectangle->bottomRight.y - boundary->bottomRight.y;
        rectangle->topLeft.y -= offset.y;
        rectangle->bottomRight.y -= offset.y;
    }
}



// Décale le point selon la valeur en X/Y du point offset
// -> Modifie directement la position du point
void OffsetPoint2D(Point2D *point, Point2D *offset) {
		
	// Décale la position en X
	point->x += offset->x;
	// Décale la position en Y
	point->y += offset->y;
	
}

// Décale le rectangle selon la valeur en X/Y du point
// -> Modifie directement la position du rectangle
void OffsetRectangle(Rectangle *rectangle, Point2D *offset) {
		
	// Décale les positions en X
    rectangle->topLeft.x += offset->x;
    rectangle->bottomRight.x += offset->x;
	// Décale les positions en Y
    rectangle->topLeft.y += offset->y;
    rectangle->bottomRight.y += offset->y;
	
}

// Le point est-il dans le rectangle
// -> Retourne 1 si le point est dans le rectangle, 0 si ce n'est pas le cas
int32 Point2DInRectangle(Rectangle *rectangle, Point2D *point) {
	
	// Compare la position du point avec les coins du rectangle
	return ((point->x >= rectangle->topLeft.x) && (point->x <= rectangle->bottomRight.x) && (point->y >= rectangle->topLeft.y) && (point->y <= rectangle->bottomRight.y)) ? 1 : 0;
	
}

// Intersection de deux rectangles
// -> Retourne 1 si les rectangles se chevauchent, 0 si ce n'est pas le cas
int32 RectanglesIntersection(Rectangle *rectangle1, Rectangle *rectangle2) {
	
	// Compare les coins des deux rectangles
    return ((rectangle1->bottomRight.x >= rectangle2->topLeft.x && rectangle1->topLeft.x <= rectangle2->bottomRight.x) && 
            (rectangle1->bottomRight.y >= rectangle2->topLeft.y && rectangle1->topLeft.y <= rectangle2->bottomRight.y)) ? 1 : 0;

}

// Etirement d'un Cel selon un multiplicateur
// -> Modifie directement les valeurs ccb_HDX et ccb_VDY du Cel
void CelScale(CCB *cel, int32 scaleX, int32 scaleY) {
	
	// Correction des paramètres
	scaleX = (scaleX > 2047) ? 2047 : scaleX;
	scaleX = (scaleX < -2048) ? -2048 : scaleX;
	scaleY = (scaleY > 32767) ? 32767 : scaleY;
	scaleY = (scaleY < -32768) ? -32768 : scaleY;
	
	// Modifie les valeurs
	cel->ccb_HDX = scaleX << 20;
	cel->ccb_VDY = scaleY << 16;
}

// Etirement d'un Cel selon une taille cible en pixels
// -> Modifie directement les valeurs ccb_HDX et ccb_VDY du Cel
void CelPixelScale(CCB *cel, uint32 sizeX, uint32 sizeY) {
	
    // Sécurité pour éviter division par zéro
    if (cel->ccb_Width <= 0 || cel->ccb_Height <= 0) { return; }
	
	// Correction des paramètres
	sizeX = (sizeX == 0) ? 1 : sizeX;
	sizeY = (sizeY == 0) ? 1 : sizeY;
	
    // Calcul des nouveaux coefficients de scaling en virgule fixe
    cel->ccb_HDX = (sizeX << 20) / cel->ccb_Width;
    cel->ccb_VDY = (sizeY << 16) / cel->ccb_Height;
}

// Supprime l'étirement d'un Cel
// -> Modifie directement les valeurs ccb_HDX et ccb_VDY du Cel
void CelUnscale(CCB *cel, uint32 unscaleX, uint32 unscaleY) {
	
	// Remets le Cel à sa taille d'origine à 1:1 en X
	if (unscaleX > 0) { cel->ccb_HDX = 1 << 20; }
	// Remets le Cel à sa taille d'origine à 1:1 en Y
	if (unscaleY > 0) { cel->ccb_VDY = 1 << 16; }
	
}

// Retourne un Cel horizontalement avec ou sans correction de la position
// -> Modifie directement les valeurs ccb_HDX et ccb_XPos du Cel
void CelFlipHorizontal(CCB *cel, int32 flip, uint32 correction) {
	
	// Largeur du CEL
	int32 width = 0;
	
	// Correction des paramètres
	flip = (flip < 0) ? -1 : 1;
	correction = (correction > 0) ? 1 : 0;
	
	// Récupère la largeur
	width = cel->ccb_Width << 16;
	
	// Si il n'y une correction à faire
    if (correction) {
		// Si il y a un changement d'orientation
        if ((cel->ccb_HDX >= 0 && flip == -1) || (cel->ccb_HDX < 0 && flip == 1)) {
			// Corrige la position
            cel->ccb_XPos -= width;
		// Sinon
        } else {
			// Reviens à la position d'origine
            cel->ccb_XPos += width;
        }
    }

	// Réalise le flip
	cel->ccb_HDX *= flip;
}

// Retourne un Cel verticalement avec ou sans correction de la position
// -> Modifie directement les valeurs ccb_VDY et ccb_YPos du Cel
void CelFlipVertical(CCB *cel, int32 flip, uint32 correction) {
	
	// Hauteur du CEL
	int32 height = 0;
	
	// Correction des paramètres
	flip = (flip < 0) ? -1 : 1;
	correction = (correction > 0) ? 1 : 0;

	// Récupère la largeur
	height = cel->ccb_Height << 16;
	
	// Si il n'y une correction à faire
    if (correction) {
		// Si il y a un changement d'orientation
        if ((cel->ccb_VDY >= 0 && flip == -1) || (cel->ccb_VDY < 0 && flip == 1)) {
			// Corrige la position
            cel->ccb_YPos -= height;
		// Sinon
        } else {
			// Reviens à la position d'origine
            cel->ccb_YPos += height;
        }
    }
	
	// Réalise le flip
	cel->ccb_VDY *= flip;
}

// Retourne le rectangle occupé à l'écran par le Cel transformé
// -> Modifie directement les valeurs du rectangle 
void CelProjection(CCB *cel, Rectangle *rectangle) {

    // Les coefficients (en virgule fixe)
    int32 hdx = cel->ccb_HDX; // 12.20
    int32 hdy = cel->ccb_HDY; // 12.20
    int32 vdx = cel->ccb_VDX; // 16.16
    int32 vdy = cel->ccb_VDY; // 16.16

    // Coin 1 : origine
    int32 px0 = cel->ccb_XPos;
    int32 py0 = cel->ccb_YPos;

    // Coin 2 : déplacement en X (largeur)
    int32 px1 = cel->ccb_XPos + ((hdx * cel->ccb_Width) >> 20);
    int32 py1 = cel->ccb_YPos + ((hdy * cel->ccb_Width) >> 20);

    // Coin 3 : déplacement en Y (hauteur)
    int32 px2 = cel->ccb_XPos + ((vdx * cel->ccb_Height) >> 16);
    int32 py2 = cel->ccb_YPos + ((vdy * cel->ccb_Height) >> 16);

    // Coin 4 : déplacement en X + Y (diagonale)
    int32 px3 = cel->ccb_XPos + ((hdx * cel->ccb_Width) >> 20) + ((vdx * cel->ccb_Height) >> 16);
    int32 py3 = cel->ccb_YPos + ((hdy * cel->ccb_Width) >> 20) + ((vdy * cel->ccb_Height) >> 16);

    // Cherche les min/max pour envelopper le Cel
    int32 minX = px0, maxX = px0;
    int32 minY = py0, maxY = py0;

    if (px1 < minX) minX = px1; if (px1 > maxX) maxX = px1;
    if (px2 < minX) minX = px2; if (px2 > maxX) maxX = px2;
    if (px3 < minX) minX = px3; if (px3 > maxX) maxX = px3;

    if (py1 < minY) minY = py1; if (py1 > maxY) maxY = py1;
    if (py2 < minY) minY = py2; if (py2 > maxY) maxY = py2;
    if (py3 < minY) minY = py3; if (py3 > maxY) maxY = py3;

    rectangle->topLeft.x = minX;
    rectangle->topLeft.y = minY;
    rectangle->bottomRight.x = maxX;
    rectangle->bottomRight.y = maxY;
}
