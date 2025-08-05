
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

// Initialization of a GroupCel
GroupCel *GroupCelInitialization(int32 positionX, int32 positionY, uint32 sizeX, uint32 sizeY, uint32 celsCount) {

    // GroupCel
    GroupCel *groupCel = NULL;
        
    if (DEBUG_GROUPCEL_INIT == 1) { printf("*GroupCelInitialization()*\n"); }

    // Allocates memory for the GroupCel
    groupCel = (GroupCel *)AllocMem(sizeof(GroupCel), MEMTYPE_DRAM);
    // If it's a failure
    if (groupCel == NULL) {
        // Displays an error message
        printf("Error : Failed to allocate memory for GroupCel.\n");
        return NULL;
    }

    // Corrects parameters 
    // Minimum number of cels = 2
    celsCount = (celsCount > 2) ? celsCount : 2;
    // Minimum dimension = 1 x 1 pixel
    sizeX = (sizeX > 1) ? sizeX : 1;
    sizeY = (sizeY > 1) ? sizeY : 1;

    // Group position
    groupCel->position.x = positionX;
    groupCel->position.y = positionY;
    // Group dimension
    groupCel->size.x = sizeX;
    groupCel->size.y = sizeY;

    // Quadrilateral intended for stretching
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

    // Total number of cels
    groupCel->celsCount = celsCount;

    // Allocates memory for the array of cels
    groupCel->cels = (GroupCelList *)AllocMem(celsCount * sizeof(GroupCelList), MEMTYPE_DRAM);
    // If it's a failure
    if (groupCel->cels == NULL) {
        // Frees previously allocated memory
        FreeMem(groupCel, sizeof(GroupCel));
        // Displays an error message
        printf("Error : Failed to allocate memory for GroupCel cels.\n");
        return NULL;
    }

    // Initializes the values of the cels to 0 
    memset(groupCel->cels, 0, (size_t)celsCount * sizeof(GroupCelList));

    // Returns the created GroupCel
    return groupCel;
}

// Configuration of a Cel in a GroupCel
int32 GroupCelElementConfiguration(GroupCel *groupCel, uint32 celIndex, CCB *cel, uint32 positionX, uint32 positionY) {

    // Quadrilateral for Cel projection in 16.16
    Quad2D quadMap = {0};

    if (DEBUG_GROUPCEL_SETUP == 1) { printf("*GroupCelElementConfiguration()*\n"); }

    // If the group is unknown
    if (groupCel == NULL){
        // Returns an error
        printf("Error : GroupCel unknown.\n");
        return -1;
    }

    // If the list of Cels is unknown
    if (groupCel->cels == NULL){
        // Returns an error
        printf("Error : GroupCel list unknown.\n");
        return -1;
    }

    // If the Cel is unknown
    if (cel == NULL){
        // Returns an error
        printf("Error : Cel unknown.\n");
        return -1;
    }

    // Corrects parameters
    if (celIndex >= groupCel->celsCount) { 
        // Displays a warning
        printf("Warning : GroupCel celIndex %u out of bounds. Clamped to last index.\n", celIndex);
        // Changes index to the last available
        celIndex = groupCel->celsCount - 1; 
    }

    // Stores the Cel in the list
    groupCel->cels[celIndex].position.x = positionX;
    groupCel->cels[celIndex].position.y = positionY;
    groupCel->cels[celIndex].cel = (CCB *)cel;

    // If it's the first Cel in the list
    if(celIndex == 0){
        // Updates the group's main CCB
        groupCel->cel = groupCel->cels[celIndex].cel;
    // Otherwise
    } else {
        // Informs the Cel Engine that this is the next Cel to render
        LinkCel(groupCel->cels[celIndex - 1].cel, groupCel->cels[celIndex].cel);
    }

    // Updates the position in the Cel's CCB
    GroupCelElementUpdatePosition(groupCel, celIndex, &quadMap);

    // Returns success
    return 1;
}

// Configuration of the cels in a GroupCel
int32 GroupCelElementsConfiguration(GroupCel *groupCel, int32 start, ...) {

    // Function argument list
    va_list args;
    // Cel index
    uint32 celIndex = 0;
    // Cel pointer
    CCB *cel = NULL;
    // Coordinates
    uint32 positionX = 0;
    uint32 positionY = 0;
    
    if (DEBUG_GROUPCEL_SETUP == 1) { printf("*GroupCelElementsConfiguration()*\n"); }       

    // Start reading arguments
    va_start(args, start);

    // Loop until the sentinel value
    while (1) {

        // Retrieve the index
        celIndex = va_arg(args, uint32);

        // If this is the end of the list
        if (celIndex == LIST_END) {
            // Exit the loop
            break;
        }

        // Retrieve the rest of the arguments
        cel = va_arg(args, CCB *);
        positionX = va_arg(args, uint32);
        positionY = va_arg(args, uint32);

        // Sprite frame configuration
        if (DEBUG_GROUPCEL_SETUP == 1) { printf("-> GroupCelElementConfiguration(%p, %u, %p, %u, %u)\n", groupCel, celIndex, cel, positionX, positionY); }
        if (GroupCelElementConfiguration(groupCel, celIndex, cel, positionX, positionY) < 0) {
            // End of the argument list
            va_end(args);
            // Return an error
            printf("Error <- GroupCelElementConfiguration()\n");
            return -1;
        }
    }

    // End of the argument list
    va_end(args);

    // Return success
    return 1;
}

// Moves the GroupCel
void GroupCelMove(GroupCel *groupCel, int32 moveX, int32 moveY) {
    
    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelMove()*\n"); }    
    
    // If the group is unknown
    if (groupCel == NULL){
        // Return an error
        printf("Error : GroupCel unknown.\n");
        return;
    }
    
    // Modify the GroupCel's position
    GroupCelSetPosition(groupCel, groupCel->position.x + moveX, groupCel->position.y + moveY);  
}

// Sets the position of the GroupCel
void GroupCelSetPosition(GroupCel *groupCel, int32 positionX, int32 positionY) {
    
    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelSetPosition()*\n"); } 
    
    // If the group is unknown
    if (groupCel == NULL){
        // Returns an error
        printf("Error : GroupCel unknown.\n");
        return;
    }
    
    // Applies the changes
    groupCel->position.x = positionX;
    groupCel->position.y = positionY;

    // Activates movement
    groupCel->transformations |= GROUPCEL_FLAG_MOVE;
}

// Moves an element within the GroupCel
void GroupCelElementMove(GroupCel *groupCel, int32 celIndex, int32 moveX, int32 moveY) {
    
    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelElementMove()*\n"); } 
    
    // If the group is unknown
    if (groupCel == NULL){
        // Returns an error
        printf("Error : GroupCel unknown.\n");
        return;
    }
    
    // If the list of Cels is unknown
    if (groupCel->cels == NULL){
        // Returns an error
        printf("Error : GroupCel list unknown.\n");
        return;
    }
    
    // Modifies the element's position in the group
    GroupCelElementSetPosition(groupCel, celIndex, groupCel->cels[celIndex].position.x + moveX, groupCel->cels[celIndex].position.y + moveY);   
}

// Sets the position of a cel within the GroupCel
void GroupCelElementSetPosition(GroupCel *groupCel, int32 celIndex, int32 positionX, int32 positionY) {
    
    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelElementSetPosition()*\n"); }  
    
    // If the group is unknown
    if (groupCel == NULL){
        // Returns an error
        printf("Error : GroupCel unknown.\n");
        return;
    }
    
    // If the list of Cels is unknown
    if (groupCel->cels == NULL){
        // Returns an error
        printf("Error : GroupCel list unknown.\n");
        return;
    }
    
    // Applies the changes
    groupCel->cels[celIndex].position.x = positionX;
    groupCel->cels[celIndex].position.y = positionY;
    
    printf("positionX : %d\n", positionX);
    printf("positionY : %d\n", positionY);

    // Activates movement
    groupCel->transformations |= GROUPCEL_FLAG_MOVE;
}

// Stretches the group according to its 4 corners in pixels
void GroupCelStretch(GroupCel *groupCel, int32 point0X, int32 point0Y, int32 point1X, int32 point1Y, int32 point2X, int32 point2Y, int32 point3X, int32 point3Y) {
    
    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelStretch()*\n"); }

    // If the group is unknown
    if (groupCel == NULL){
        // Return an error
        printf("Error : GroupCel unknown.\n");
        return;
    }
    
    // Modify transformation values
    groupCel->stretching.topLeft.x = point0X - groupCel->position.x;
    groupCel->stretching.topLeft.y = point0Y - groupCel->position.y;
    groupCel->stretching.topRight.x = point1X - groupCel->position.x;
    groupCel->stretching.topRight.y = point1Y - groupCel->position.y;
    groupCel->stretching.bottomRight.x = point2X - groupCel->position.x;
    groupCel->stretching.bottomRight.y = point2Y - groupCel->position.y;
    groupCel->stretching.bottomLeft.x = point3X - groupCel->position.x;
    groupCel->stretching.bottomLeft.y = point3Y - groupCel->position.y;
    
    // Activate stretching
    groupCel->transformations |= GROUPCEL_FLAG_STRETCH;
}

// Rotates the group by an angle in 256 units
void GroupCelRotate(GroupCel *groupCel, uint32 angle, int32 pivotX, int32 pivotY) {
    
    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelRotate()*\n"); }

    // If the group is unknown
    if (groupCel == NULL){
        // Return an error
        printf("Error : GroupCel unknown.\n");
        return;
    }
    
    // Modify angle within range [0 - 255]
    groupCel->rotationAngle = angle % 256;
    // Modify transformation values
    groupCel->rotationPivot.x = pivotX;
    groupCel->rotationPivot.y = pivotY;
    
    // Activate rotation
    groupCel->transformations |= GROUPCEL_FLAG_ROTATE;
}

// Updates the elements inside the GroupCel
void GroupCelUpdate(GroupCel *groupCel) {   

    // Index to iterate through elements
    int32 celIndex = 0;
    // Indicates if transformations are to be applied
    int32 doMove = 0;
    int32 doStretch = 0;
    int32 doRotate = 0;
    // Quadrilateral for Cel projection in 16.16
    Quad2D quadMap = {0};
    
    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelUpdate()*\n"); }  
    
    // If the group is unknown
    if (groupCel == NULL){
        // Return an error
        printf("Error : GroupCel unknown.\n");
        return;
    }
    
    // If the list of Cels is unknown
    if (groupCel->cels == NULL){
        // Return an error
        printf("Error : GroupCel list unknown.\n");
        return;
    }
    
    // Determine if movement needs to be applied
    doMove = (groupCel->transformations & GROUPCEL_FLAG_MOVE) != 0;
    // Determine if stretching needs to be applied
    doStretch = (groupCel->transformations & GROUPCEL_FLAG_STRETCH) != 0;
    // Determine if rotation needs to be applied
    doRotate = (groupCel->transformations & GROUPCEL_FLAG_ROTATE) != 0;
    
    // If there’s nothing to update
    if ((doMove == 0) && (doStretch == 0) && (doRotate == 0)) {
        // Exit early
        return;
    }
    
    // If stretching or rotation is needed
    if ((doStretch == 1) || (doRotate == 1)) {
        // Call complex update function
        GroupCelUpdateTransformations(groupCel, doStretch, doRotate, &quadMap);
        // Exit immediately after
        return;
    }
    
    // For each element
    for (celIndex = 0; celIndex < groupCel->celsCount; celIndex++) {
        // Update the position in the Cel's CCB
        GroupCelElementUpdatePosition(groupCel, celIndex, &quadMap);
    }
    
    // Disable movement
    groupCel->transformations &= ~GROUPCEL_FLAG_MOVE;   
}

// Applies the position to the Cel's CCB
void GroupCelElementUpdatePosition(GroupCel *groupCel, int32 celIndex, Quad2D *quadMap) {

    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelElementUpdatePosition()*\n"); } 
    
    // Creates the quadrilateral for Cel projection in 16.16
    quadMap->topLeft.x = Convert32_F16(groupCel->position.x + groupCel->cels[celIndex].position.x);
    quadMap->topLeft.y = Convert32_F16(groupCel->position.y + groupCel->cels[celIndex].position.y);
    quadMap->topRight.x = Convert32_F16(groupCel->position.x + groupCel->cels[celIndex].position.x + groupCel->cels[celIndex].cel->ccb_Width);
    quadMap->topRight.y = Convert32_F16(groupCel->position.y + groupCel->cels[celIndex].position.y);
    quadMap->bottomRight.x = Convert32_F16(groupCel->position.x + groupCel->cels[celIndex].position.x + groupCel->cels[celIndex].cel->ccb_Width);
    quadMap->bottomRight.y = Convert32_F16(groupCel->position.y + groupCel->cels[celIndex].position.y + groupCel->cels[celIndex].cel->ccb_Height);
    quadMap->bottomLeft.x = Convert32_F16(groupCel->position.x + groupCel->cels[celIndex].position.x);
    quadMap->bottomLeft.y = Convert32_F16(groupCel->position.y + groupCel->cels[celIndex].position.y + groupCel->cels[celIndex].cel->ccb_Height);

    // Modifies the Cel's CCB
    CelQuadMap(groupCel->cels[celIndex].cel, quadMap);
}

// Applies Stretch and Rotate transformations to the CCBs of the Cels
void GroupCelUpdateTransformations(GroupCel *groupCel, int32 doStretch, int32 doRotate, Quad2D *quadMap) {

    // Index to loop through elements
    int32 celIndex = 0;
    // Position in 16.16
    Point2D position16 = {0};
    // Pre-computation for stretching
    int32 invW, invH;              
    int32 Ax, Ay, Bx, By, Cx, Cy;
    int32 delta16X = 0;
    int32 delta16Y = 0;
    // Pre-computation for rotation in 16.16
    int32 sinus16 = 0;
    int32 cosinus16 = 0;
    Point2D rotationPivot16 = {0};
    int32 rotationDelta16X = 0;
    int32 rotationDelta16Y = 0;
    // Local positions of the 4 corners (int32)
    int32 localX[4] = {0};
    int32 localY[4] = {0};
    // Index to loop through corners
    int32 indexCorner = 0;
    int32 px, py, u, v, uv;
    int32 rx, ry;   

    if (DEBUG_GROUPCEL_FUNCT == 1) { printf("*GroupCelUpdate()*\n"); }  

    // Position in 16.16
    position16.x = Convert32_F16(groupCel->position.x);
    position16.y = Convert32_F16(groupCel->position.y);
    
    // If stretching is enabled
    if (doStretch == 1) {
        // Inverse width/height
        invW = (1 << 16) / groupCel->size.x;
        invH = (1 << 16) / groupCel->size.y;

        // Corner A
        Ax = Convert32_F16(groupCel->stretching.topLeft.x);
        Ay = Convert32_F16(groupCel->stretching.topLeft.y);

        // Vectors B and C
        Bx = Convert32_F16(groupCel->stretching.topRight.x - groupCel->stretching.topLeft.x);
        By = Convert32_F16(groupCel->stretching.topRight.y - groupCel->stretching.topLeft.y);
        Cx = Convert32_F16(groupCel->stretching.bottomLeft.x - groupCel->stretching.topLeft.x);
        Cy = Convert32_F16(groupCel->stretching.bottomLeft.y - groupCel->stretching.topLeft.y);

        // Term D (bilinearity)
        delta16X = Convert32_F16(groupCel->stretching.topLeft.x + groupCel->stretching.bottomRight.x - groupCel->stretching.topRight.x - groupCel->stretching.bottomLeft.x);
        delta16Y = Convert32_F16(groupCel->stretching.topLeft.y + groupCel->stretching.bottomRight.y - groupCel->stretching.topRight.y - groupCel->stretching.bottomLeft.y);
    }

    // If rotation is enabled
    if (doRotate == 1) {
        // Calculate sine and cosine in 16.16
        sinus16 = SinF16(Convert32_F16(groupCel->rotationAngle));
        cosinus16 = CosF16(Convert32_F16(groupCel->rotationAngle));
        // Locate the pivot
        rotationPivot16.x = Convert32_F16(groupCel->rotationPivot.x - groupCel->position.x);
        rotationPivot16.y = Convert32_F16(groupCel->rotationPivot.y - groupCel->position.y);
    }
    
    // For each element
    for (celIndex = 0; celIndex < groupCel->celsCount; celIndex++) {

        // Local positions of the 4 corners (int32)
        localX[0] = groupCel->cels[celIndex].position.x;
        localX[1] = groupCel->cels[celIndex].position.x + groupCel->cels[celIndex].cel->ccb_Width;
        localX[2] = groupCel->cels[celIndex].position.x + groupCel->cels[celIndex].cel->ccb_Width;
        localX[3] = groupCel->cels[celIndex].position.x;
        localY[0] = groupCel->cels[celIndex].position.y;
        localY[1] = groupCel->cels[celIndex].position.y;
        localY[2] = groupCel->cels[celIndex].position.y + groupCel->cels[celIndex].cel->ccb_Height;
        localY[3] = groupCel->cels[celIndex].position.y + groupCel->cels[celIndex].cel->ccb_Height;

        // For each corner
        for (indexCorner = 0; indexCorner < 4; indexCorner++) {
            
            // If stretching is enabled
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

            // If rotation is enabled
            if (doRotate == 1) {
                rotationDelta16X = px - rotationPivot16.x;
                rotationDelta16Y = py - rotationPivot16.y;
                rx = MulSF16(cosinus16, rotationDelta16X) - MulSF16(sinus16, rotationDelta16Y) + rotationPivot16.x;
                ry = MulSF16(sinus16, rotationDelta16X) + MulSF16(cosinus16, rotationDelta16Y) + rotationPivot16.y;
            } else {
                rx = px;
                ry = py;
            }
            
            // Final translation + storage
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

        // Modifies the Cel's CCB
        CelQuadMap(groupCel->cels[celIndex].cel, quadMap);
    }
    
    // Disables stretching
    groupCel->transformations &= ~GROUPCEL_FLAG_STRETCH;
    // Disables rotation
    groupCel->transformations &= ~GROUPCEL_FLAG_ROTATE;
}

// Deletes the GroupCel
int32 GroupCelCleanup(GroupCel *groupCel) {
        
    if (DEBUG_GROUPCEL_CLEAN == 1) { printf("*GroupCelCleanup()*\n"); } 
    
    // If the GroupCel doesn't exist
    if (groupCel == NULL){
        // Display an error
        printf("Error : GroupCel unknown.\n");
        return -1;  
    } 
    
    // If there are cels
    if (groupCel->cels != NULL) {
        // Free the memory used for the cels array
        FreeMem(groupCel->cels, groupCel->celsCount * sizeof(GroupCelList));
        groupCel->cels = NULL;
    }
    // Free the memory used for the GroupCel
    FreeMem(groupCel, sizeof(GroupCel));
    groupCel->cel = NULL;
    
    // Finalize cleanup
    groupCel = NULL;

    // Return success
    return 1;
}

