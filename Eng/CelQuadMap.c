#include "CelQuadMap.h"

// printf()
#include "stdio.h"

// Applies a transformation to a CCB based on a quadrilateral in 16.16 format
void CelQuadMap(CCB *ccb, Quad2D *quadMap) {

    // If the CCB is unknown
    if (ccb == NULL){
        // Return an error
        printf("Error : CCB unknown.\n");
        return;
    }
    
    // If the quad is unknown
    if (quadMap == NULL){
        // Return an error
        printf("Error : Quad2D unknown.\n");
        return;
    }

    if (ccb->ccb_Width > 0) {
        // HDX/HDY in 12.20 fixed-point format
        ccb->ccb_HDX = ((quadMap->topRight.x - quadMap->topLeft.x) << 4) / ccb->ccb_Width;
        ccb->ccb_HDY = ((quadMap->topRight.y - quadMap->topLeft.y) << 4) / ccb->ccb_Width;
    } else {
        ccb->ccb_HDX = ccb->ccb_HDY = 0;
    }

    if (ccb->ccb_Height > 0) {
        // VDX/VDY in 16.16 fixed-point format
        ccb->ccb_VDX = (quadMap->bottomLeft.x - quadMap->topLeft.x) / ccb->ccb_Height;
        ccb->ccb_VDY = (quadMap->bottomLeft.y - quadMap->topLeft.y) / ccb->ccb_Height;
    } else {
        ccb->ccb_VDX = ccb->ccb_VDY = 0;
    }

    if (ccb->ccb_Width > 0 && ccb->ccb_Height > 0) {
        // HDDX/HDDY in 12.20 fixed-point format
        ccb->ccb_HDDX = (((quadMap->topLeft.x + quadMap->bottomRight.x) - (quadMap->topRight.x + quadMap->bottomLeft.x)) << 4) / (ccb->ccb_Width * ccb->ccb_Height);
        ccb->ccb_HDDY = (((quadMap->topLeft.y + quadMap->bottomRight.y) - (quadMap->topRight.y + quadMap->bottomLeft.y)) << 4) / (ccb->ccb_Width * ccb->ccb_Height);
    } else {
        ccb->ccb_HDDX = ccb->ccb_HDDY = 0;
    }

    ccb->ccb_XPos = quadMap->topLeft.x;
    ccb->ccb_YPos = quadMap->topLeft.y;

    // Set flags to load size and perspective
    ccb->ccb_Flags |= CCB_LDSIZE | CCB_LDPRS;
}
