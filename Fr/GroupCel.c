
#include "GroupCel.h"

// UNDEFINED, LIST_START, LIST_END
#include "DefinitionsArguments.h"
// AllocMem(), FreeMem(), MEMTYPE_DRAM
#include "mem.h"
// CloneCel
#include "celutils.h"
// memset()
#include "string.h"
// CelQuadMap
#include "CelQuadMap.h"
// Point2D, Quad2D
#include "Geometry.h"
// Convert32_F16, ConvertF16_32
#include "operamath.h"
// printf()
#include "stdio.h"

// Initialisation d'un GroupCel
GroupCel *GroupCelInitialization(int32 positionX, int32 positionY, uint32 sizeX, uint32 sizeY, uint32 celsCount) {

	// GroupCel
	GroupCel *groupCel = NULL;
		
	if (DEBUG_GROUPCEL_INIT == 1) { printf("*GroupCelInitialization()*\n");	}
	
	// Alloue de la mémoire pour le GroupCel
	groupCel = (GroupCel *)AllocMem(sizeof(GroupCel), MEMTYPE_DRAM);
	// Si c'est un échec
    if (groupCel == NULL) {
		// Affiche un message d'erreur
        printf("Error : Failed to allocate memory for GroupCel.\n");
        return NULL;
    }

	// Corrige les paramètres 
	// Nombre de cels minimum = 2
	celsCount = (celsCount > 2) ? celsCount : 2;
	// Dimension minimum = 1 x 1 pixel
	sizeX = (sizeX > 1) ? sizeX : 1;
	sizeY = (sizeY > 1) ? sizeY : 1;
	
	// Position du groupe
	groupCel->position.x = positionX;
	groupCel->position.y = positionY;
	// Dimension du groupe
	groupCel->size.x = sizeX;
	groupCel->size.y = sizeY;

    // Quadrilatère destiné à recevoir les étirements
    groupCel->stretching.topLeft.x = 0;
	groupCel->stretching.topLeft.y = 0;
    groupCel->stretching.topRight.x = sizeX;
	groupCel->stretching.topRight.y = 0;
    groupCel->stretching.bottomRight.x = sizeX;
	groupCel->stretching.bottomRight.y = sizeY;
    groupCel->stretching.bottomLeft.x = 0;
	groupCel->stretching.bottomLeft.y = sizeY;  
	
	// Rotation
	groupCel->rotationAngle = 0;
	groupCel->rotationPivot.x = 0;
	groupCel->rotationPivot.y = 0;

	// Nombre total de cels
	groupCel->celsCount = celsCount;

    // Alloue de la mémoire pour le tableau de cels
    groupCel->cels = (GroupCelList *)AllocMem(celsCount * sizeof(GroupCelList), MEMTYPE_DRAM);
	// Si c'est un échec
    if (groupCel->cels == NULL) {
		// Libère la mémoire précédemment allouée
        FreeMem(groupCel, sizeof(GroupCel));
		// Affiche un message d'erreur
        printf("Error : Failed to allocate memory for GroupCel cels.\n");
        return NULL;
    }

    // Initialise les valeurs des cels à 0 
    memset(groupCel->cels, 0, (size_t)celsCount * sizeof(GroupCelList));

	// Retourne le GroupCel créé
    return groupCel;
}

// Configuration d'un Cel d'un GroupCel
int32 GroupCelElementConfiguration(GroupCel *groupCel, uint32 celIndex, CCB *cel, uint32 positionX, uint32 positionY) {

	// Quadrilatère pour la projection du Cel en 16.16
	Quad2D quadMap = {0};
	
	if (DEBUG_GROUPCEL_SETUP == 1) { printf("*GroupCelElementConfiguration()*\n"); }	

	// Si le groupe est inconnu
	if (groupCel == NULL){
		// Retourne une erreur
		printf("Error : GroupCel unknow.\n");
		return -1;
	}
	
	// Si la liste de Cels est inconnue
	if (groupCel->cels == NULL){
		// Retourne une erreur
		printf("Error : GroupCel list unknow.\n");
		return -1;
	}
	
	// Si le cel est inconnu
	if (cel == NULL){
		// Retourne une erreur
		printf("Error : Cel unknow.\n");
		return -1;
	}

	// Corrige les paramètres
	if (celIndex >= groupCel->celsCount) { 
		// Affiche un avertissement
		printf("Warning : GroupCel celIndex %u out of bounds. Clamped to last index.\n", celIndex);
		// Modifie l'index au dernier disponible
		celIndex = groupCel->celsCount - 1; 
	}

    // Stocke le cel dans la liste
    groupCel->cels[celIndex].position.x = positionX;
    groupCel->cels[celIndex].position.y = positionY;
    groupCel->cels[celIndex].cel = (CCB *)cel;
	
	// Si c'est le premier Cel de la liste
	if(celIndex == 0){
		// Mets à jour le CCB principal du groupe
		groupCel->cel = groupCel->cels[celIndex].cel;
	// Sinon
	} else {
		// Indique au Cel Engine que c'est le Cel suivant à rendre
		LinkCel(groupCel->cels[celIndex - 1].cel, groupCel->cels[celIndex].cel);
	}

	// Mets à jour la position dans le CCB du Cel
	GroupCelElementUpdatePosition(groupCel, celIndex, &quadMap);
	
	// Retourne un succès
	return 1;
}

// Configuration des cels d'un GroupCel
int32 GroupCelElementsConfiguration(GroupCel *groupCel, int32 start, ...) {

	// Liste des arguments de la fonction
    va_list args;
	// Index du cel
	uint32 celIndex = 0;
	// Index du cel
	CCB *cel = NULL;
	// Coordonnées
    uint32 positionX = 0;
	uint32 positionY = 0;
	
	if (DEBUG_GROUPCEL_SETUP == 1) { printf("*GroupCelElementsConfiguration()*\n"); }		

    // Débute la lecture des arguments
    va_start(args, start);

    // Boucle jusqu'à la valeur sentinelle
    while (1) {

        // Récupère l'index
        celIndex = va_arg(args, uint32);

        // Si c'est la fin de la liste
        if (celIndex == LIST_END) {
			// Sort de la boucle
            break;
        }

        // Récupère le reste des arguments
		cel = va_arg(args, CCB *);
		positionX = va_arg(args, uint32);
		positionY = va_arg(args, uint32);

		// Configuration d'une frame d'un sprite
		if (DEBUG_GROUPCEL_SETUP == 1) { printf("-> GroupCelElementConfiguration(%p, %u, %p, %u, %u)\n", groupCel, celIndex, cel, positionX, positionY); }
		if (GroupCelElementConfiguration(groupCel, celIndex, cel, positionX, positionY) < 0) {
			// Fin de la liste des arguments 
			va_end(args);
            // Retourne une erreur
            printf("Error <- GroupCelElementConfiguration()\n");
            return -1;
        }
    }

    // Fin de la liste des arguments 
    va_end(args);

    // Retourne un succès
    return 1;
}

// Déplace le GroupCel
void GroupCelMove(GroupCel *groupCel, int32 moveX, int32 moveY) {
	
	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelMove()*\n"); }	
	
	// Si le groupe est inconnu
	if (groupCel == NULL){
		// Retourne une erreur
		printf("Error : GroupCel unknow.\n");
		return;
	}
	
	// Modifie la position du GroupCel
	GroupCelSetPosition(groupCel, groupCel->position.x + moveX, groupCel->position.y + moveY);	
}

// Modifie la position du GroupCel
void GroupCelSetPosition(GroupCel *groupCel, int32 positionX, int32 positionY) {
	
	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelSetPosition()*\n"); }	
	
	// Si le groupe est inconnu
	if (groupCel == NULL){
		// Retourne une erreur
		printf("Error : GroupCel unknow.\n");
		return;
	}
	
	// Applique les changements
	groupCel->position.x = positionX;
	groupCel->position.y = positionY;

	// Active le déplacement
	groupCel->transformations |= GROUPCEL_FLAG_MOVE;
}

// Déplace un élément dans le GroupCel
void GroupCelElementMove(GroupCel *groupCel, int32 celIndex, int32 moveX, int32 moveY) {
	
	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelElementMove()*\n"); }	
	
	// Si le groupe est inconnu
	if (groupCel == NULL){
		// Retourne une erreur
		printf("Error : GroupCel unknow.\n");
		return;
	}
	
	// Si la liste de Cels est inconnue
	if (groupCel->cels == NULL){
		// Retourne une erreur
		printf("Error : GroupCel list unknow.\n");
		return;
	}
	
	// Modifie la position du de l'élément dans le groupe
	GroupCelElementSetPosition(groupCel, celIndex, groupCel->cels[celIndex].position.x + moveX, groupCel->cels[celIndex].position.y + moveY);	
}

// Modifie la position d'un cel du GroupCel
void GroupCelElementSetPosition(GroupCel *groupCel, int32 celIndex, int32 positionX, int32 positionY) {
	
	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelElementSetPosition()*\n"); }	
	
	// Si le groupe est inconnu
	if (groupCel == NULL){
		// Retourne une erreur
		printf("Error : GroupCel unknow.\n");
		return;
	}
	
	// Si la liste de Cels est inconnue
	if (groupCel->cels == NULL){
		// Retourne une erreur
		printf("Error : GroupCel list unknow.\n");
		return;
	}
	
	// Applique les changements
	groupCel->cels[celIndex].position.x = positionX;
	groupCel->cels[celIndex].position.y = positionY;
	
	printf("positionX : %d\n", positionX);
	printf("positionY : %d\n", positionY);

	// Active le déplacement
	groupCel->transformations |= GROUPCEL_FLAG_MOVE;
}


// Etire le groupe selon ses 4 coins en pixels
void GroupCelStretch(GroupCel *groupCel, int32 point0X, int32 point0Y, int32 point1X, int32 point1Y, int32 point2X, int32 point2Y, int32 point3X, int32 point3Y) {
	
	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelStretch()*\n"); }

	// Si le groupe est inconnu
	if (groupCel == NULL){
		// Retourne une erreur
		printf("Error : GroupCel unknow.\n");
		return;
	}
	
	// Modifie les valeurs de transformation
    groupCel->stretching.topLeft.x = point0X - groupCel->position.x;
    groupCel->stretching.topLeft.y = point0Y - groupCel->position.y;
    groupCel->stretching.topRight.x = point1X - groupCel->position.x;
    groupCel->stretching.topRight.y = point1Y - groupCel->position.y;
    groupCel->stretching.bottomRight.x = point2X - groupCel->position.x;
    groupCel->stretching.bottomRight.y = point2Y - groupCel->position.y;
    groupCel->stretching.bottomLeft.x = point3X - groupCel->position.x;
    groupCel->stretching.bottomLeft.y = point3Y - groupCel->position.y;
	
	// Active l'étirement
	groupCel->transformations |= GROUPCEL_FLAG_STRETCH;
}

// Tourne le groupe selon un angle en 256 unités
void GroupCelRotate(GroupCel *groupCel, uint32 angle, int32 pivotX, int32 pivotY) {
	
	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelRotate()*\n"); }

	// Si le groupe est inconnu
	if (groupCel == NULL){
		// Retourne une erreur
		printf("Error : GroupCel unknow.\n");
		return;
	}
	
	// Modifie l'angle en restant dans la plage [0 - 255]
    groupCel->rotationAngle = angle % 256;
	// Modifie les valeurs de transformation
    groupCel->rotationPivot.x = pivotX;
    groupCel->rotationPivot.y = pivotY;
	
	// Active la rotation
	groupCel->transformations |= GROUPCEL_FLAG_ROTATE;
}

// Mets à jour les élément dans le GroupCel
void GroupCelUpdate(GroupCel *groupCel) {	

	// Index pour parcourir les éléments
	int32 celIndex = 0;
	// Indique si des transformations sont à appliquer
	int32 doMove = 0;
	int32 doStretch = 0;
	int32 doRotate = 0;
	// Quadrilatère pour la projection du Cel en 16.16
	Quad2D quadMap = {0};
	
	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelUpdate()*\n"); }	
	
	// Si le groupe est inconnu
	if (groupCel == NULL){
		// Retourne une erreur
		printf("Error : GroupCel unknow.\n");
		return;
	}
	
	// Si la liste de Cels est inconnue
	if (groupCel->cels == NULL){
		// Retourne une erreur
		printf("Error : GroupCel list unknow.\n");
		return;
	}
	
	// Détermine si il faut appliquer un déplacement
    doMove = (groupCel->transformations & GROUPCEL_FLAG_MOVE) != 0;
	// Détermine si il faut appliquer un étirement
    doStretch = (groupCel->transformations & GROUPCEL_FLAG_STRETCH) != 0;
	// Détermine si il faut appliquer une rotation
    doRotate = (groupCel->transformations & GROUPCEL_FLAG_ROTATE) != 0;
	
    // Si il n'y a aucune mise à jour à faire
    if ((doMove == 0) && (doStretch == 0) && (doRotate == 0)) {
		// Quitte prématurément
		return;
	}
	
    // Si il n'y a un étirement ou une rotation
    if ((doStretch == 1) || (doRotate == 1)) {
		// Passe à la fonction de mise à jour complexes
		GroupCelUpdateTransformations(groupCel, doStretch, doRotate, &quadMap);
		// Quitte tout de suite après
		return;
	}
	
	// Pour chaque élément
	for (celIndex = 0; celIndex < groupCel->celsCount; celIndex++) {
		// Mets à jour la position dans le CCB du Cel
		GroupCelElementUpdatePosition(groupCel, celIndex, &quadMap);
	}
	
	// Désactive le déplacement
	groupCel->transformations &= ~GROUPCEL_FLAG_MOVE;	
}

// Applique la position dans le CCB du Cel
void GroupCelElementUpdatePosition(GroupCel *groupCel, int32 celIndex, Quad2D *quadMap) {

	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelElementUpdatePosition()*\n"); }	
	
	// Crée le quadrilatère pour la projection du Cel en 16.16
	quadMap->topLeft.x = Convert32_F16(groupCel->position.x + groupCel->cels[celIndex].position.x);
	quadMap->topLeft.y = Convert32_F16(groupCel->position.y + groupCel->cels[celIndex].position.y);
	quadMap->topRight.x = Convert32_F16(groupCel->position.x + groupCel->cels[celIndex].position.x + groupCel->cels[celIndex].cel->ccb_Width);
	quadMap->topRight.y = Convert32_F16(groupCel->position.y + groupCel->cels[celIndex].position.y);
	quadMap->bottomRight.x = Convert32_F16(groupCel->position.x + groupCel->cels[celIndex].position.x + groupCel->cels[celIndex].cel->ccb_Width);
	quadMap->bottomRight.y = Convert32_F16(groupCel->position.y + groupCel->cels[celIndex].position.y + groupCel->cels[celIndex].cel->ccb_Height);
	quadMap->bottomLeft.x = Convert32_F16(groupCel->position.x + groupCel->cels[celIndex].position.x);
	quadMap->bottomLeft.y = Convert32_F16(groupCel->position.y + groupCel->cels[celIndex].position.y + groupCel->cels[celIndex].cel->ccb_Height);

	// Modifie le CCB du Cel
	CelQuadMap(groupCel->cels[celIndex].cel, quadMap);
}

// Applique les transformations Stretch et Rotate dans le CCB des Cels
void GroupCelUpdateTransformations(GroupCel *groupCel, int32 doStretch, int32 doRotate, Quad2D *quadMap) {

	// Index pour parcourir les éléments
	int32 celIndex = 0;
    // Position en 16.16
	Point2D position16 = {0};
    // Pré-calculs pour l'étirement
    int32 invW, invH;              
    int32 Ax, Ay, Bx, By, Cx, Cy;
    int32 delta16X = 0;
	int32 delta16Y = 0;
    // Pré-calculs pour la rotation en 16.16
    int32 sinus16 = 0;
	int32 cosinus16 = 0;
	Point2D rotationPivot16 = {0};
    int32 rotationDelta16X = 0;
	int32 rotationDelta16Y = 0;
	// Positions locales des 4 coins (en int32)
	int32 localX[4] = {0};
	int32 localY[4] = {0};
	// Index pour parcourir les coins
	int32 indexCorner = 0;
	int32 px, py, u, v, uv;
	int32 rx, ry;	

	if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelUpdate()*\n"); }	

    // Position en 16.16
    position16.x = Convert32_F16(groupCel->position.x);
    position16.y = Convert32_F16(groupCel->position.y);
	
	// Si il y a un étirement
    if (doStretch == 1) {
        // inverses largeur/hauteur
        invW = (1 << 16) / groupCel->size.x;
        invH = (1 << 16) / groupCel->size.y;

        // coin A
        Ax = Convert32_F16(groupCel->stretching.topLeft.x);
        Ay = Convert32_F16(groupCel->stretching.topLeft.y);

        // vecteurs B et C
        Bx = Convert32_F16(groupCel->stretching.topRight.x - groupCel->stretching.topLeft.x);
        By = Convert32_F16(groupCel->stretching.topRight.y - groupCel->stretching.topLeft.y);
        Cx = Convert32_F16(groupCel->stretching.bottomLeft.x - groupCel->stretching.topLeft.x);
        Cy = Convert32_F16(groupCel->stretching.bottomLeft.y - groupCel->stretching.topLeft.y);

        // terme D (bilinéarité)
        delta16X = Convert32_F16(groupCel->stretching.topLeft.x + groupCel->stretching.bottomRight.x - groupCel->stretching.topRight.x - groupCel->stretching.bottomLeft.x);
        delta16Y = Convert32_F16(groupCel->stretching.topLeft.y + groupCel->stretching.bottomRight.y - groupCel->stretching.topRight.y - groupCel->stretching.bottomLeft.y);
    }

	// Si il y a une rotation
    if (doRotate == 1) {
		// Calcule le sinus et cosinus en 16.16
        sinus16 = SinF16(Convert32_F16(groupCel->rotationAngle));
        cosinus16 = CosF16(Convert32_F16(groupCel->rotationAngle));
		// Localise le pivot
        rotationPivot16.x = Convert32_F16(groupCel->rotationPivot.x - groupCel->position.x);
        rotationPivot16.y = Convert32_F16(groupCel->rotationPivot.y - groupCel->position.y);
    }
	
	// Pour chaque élément
	for (celIndex = 0; celIndex < groupCel->celsCount; celIndex++) {

        // Positions locales des 4 coins (en int32)
        localX[0] = groupCel->cels[celIndex].position.x;
		localX[1] = groupCel->cels[celIndex].position.x + groupCel->cels[celIndex].cel->ccb_Width;
		localX[2] = groupCel->cels[celIndex].position.x + groupCel->cels[celIndex].cel->ccb_Width;
		localX[3] = groupCel->cels[celIndex].position.x;
		localY[0] = groupCel->cels[celIndex].position.y;
		localY[1] = groupCel->cels[celIndex].position.y;
		localY[2] = groupCel->cels[celIndex].position.y + groupCel->cels[celIndex].cel->ccb_Height;
		localY[3] = groupCel->cels[celIndex].position.y + groupCel->cels[celIndex].cel->ccb_Height;

		// Pour chaque coin
        for (indexCorner = 0; indexCorner < 4; indexCorner++) {
			
            // Si il y a un étirement
            if (doStretch == 1) {
                u = MulSF16(Convert32_F16(localX[indexCorner]), invW);
                v = MulSF16(Convert32_F16(localY[indexCorner]), invH);
                uv = MulSF16(u, v);
                px = Ax + MulSF16(Bx, u) + MulSF16(Cx, v) + MulSF16(delta16X, uv);
                py = Ay + MulSF16(By, u) + MulSF16(Cy, v) + MulSF16(delta16Y, uv);
            } else {
                px = Convert32_F16(localX[indexCorner]);
                py = Convert32_F16(localY[indexCorner]);
            }

            // Si il y a une rotation
            if (doRotate == 1) {
                rotationDelta16X = px - rotationPivot16.x;
                rotationDelta16Y = py - rotationPivot16.y;
                rx = MulSF16(cosinus16, rotationDelta16X) - MulSF16(sinus16, rotationDelta16Y) + rotationPivot16.x;
                ry = MulSF16(sinus16, rotationDelta16X) + MulSF16(cosinus16, rotationDelta16Y) + rotationPivot16.y;
            } else {
                rx = px;
                ry = py;
            }
			
            // Translation finale + stockage
            switch (indexCorner) {
                case 0:
                    quadMap->topLeft.x = position16.x + rx;
                    quadMap->topLeft.y = position16.y + ry;
                    break;
                case 1:
                    quadMap->topRight.x = position16.x + rx;
                    quadMap->topRight.y = position16.y + ry;
                    break;
                case 2:
                    quadMap->bottomRight.x = position16.x + rx;
                    quadMap->bottomRight.y = position16.y + ry;
                    break;
                case 3:
                    quadMap->bottomLeft.x = position16.x + rx;
                    quadMap->bottomLeft.y = position16.y + ry;
                    break;
            }
        }

		// Modifie le CCB du Cel
        CelQuadMap(groupCel->cels[celIndex].cel, quadMap);
    }
	
	// Désactive l'étirement
	groupCel->transformations &= ~GROUPCEL_FLAG_STRETCH;
	// Désactive la rotation
	groupCel->transformations &= ~GROUPCEL_FLAG_ROTATE;
}

// Supprime le GroupCel
int32 GroupCelCleanup(GroupCel *groupCel) {
		
	if (DEBUG_GROUPCEL_CLEAN == 1) { printf("*GroupCelCleanup()*\n"); }	
	
	// Si le GroupCel n'existe pas
    if (groupCel == NULL){
		// Affiche une erreur
		printf("Error : GroupCel unknow.\n");
		return -1;	
	} 
	
	// Si il y a des cels
    if (groupCel->cels != NULL) {
		// Libère la mémoire utilisée pour le tableau de cels
        FreeMem(groupCel->cels, groupCel->celsCount * sizeof(GroupCelList));
        groupCel->cels = NULL;
    }
	// Libère la mémoire utilisée pour le GroupCel
    FreeMem(groupCel, sizeof(GroupCel));
	groupCel->cel = NULL;
	
	// Finalise le nettoyage
	groupCel = NULL;

	// Retourne un succès
    return 1;
}
