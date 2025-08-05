
#include "CelQuadMap.h"

// printf()
#include "stdio.h"

// Applique une transformation d'un CCB en fonction d'un quadrilatère en 16.16
void CelQuadMap(CCB *ccb, Quad2D *quadMap) {

	// Si le CCB est inconnu
	if (ccb == NULL){
		// Retourne une erreur
		printf("Error : CCB unknow.\n");
		return;
	}
	
	// Si le quad est inconnu
	if (quadMap == NULL){
		// Retourne une erreur
		printf("Error : Quad2D unknow.\n");
		return;
	}

    if (ccb->ccb_Width > 0) {
		// HDX/HDY en 12.20
        ccb->ccb_HDX = ((quadMap->topRight.x - quadMap->topLeft.x) << 4) / ccb->ccb_Width;
        ccb->ccb_HDY = ((quadMap->topRight.y - quadMap->topLeft.y) << 4) / ccb->ccb_Width;
    } else {
        ccb->ccb_HDX = ccb->ccb_HDY = 0;
    }

    if (ccb->ccb_Height > 0) {
		// VDX/VDY en 16.16
        ccb->ccb_VDX = (quadMap->bottomLeft.x - quadMap->topLeft.x) / ccb->ccb_Height;
        ccb->ccb_VDY = (quadMap->bottomLeft.y - quadMap->topLeft.y) / ccb->ccb_Height;
    } else {
        ccb->ccb_VDX = ccb->ccb_VDY = 0;
    }

    if (ccb->ccb_Width > 0 && ccb->ccb_Height > 0) {
		// HDDX/HDDY en 12.20
        ccb->ccb_HDDX = (((quadMap->topLeft.x + quadMap->bottomRight.x) - (quadMap->topRight.x + quadMap->bottomLeft.x)) << 4) / (ccb->ccb_Width * ccb->ccb_Height);
        ccb->ccb_HDDY = (((quadMap->topLeft.y + quadMap->bottomRight.y) - (quadMap->topRight.y + quadMap->bottomLeft.y)) << 4) / (ccb->ccb_Width * ccb->ccb_Height);
    } else {
        ccb->ccb_HDDX = ccb->ccb_HDDY = 0;
    }

    ccb->ccb_XPos = quadMap->topLeft.x;
    ccb->ccb_YPos = quadMap->topLeft.y;

    ccb->ccb_Flags |= CCB_LDSIZE | CCB_LDPRS;
}