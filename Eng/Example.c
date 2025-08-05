
// Definitions of arguments
#include "DefinitionsArguments.h"
// LoadCel()
#include "celutils.h"
// Point2D, Quad2D
#include "Geometry.h"
// printf()
#include "stdio.h"

int32 main(){
    
    // Cel
    CCB *cel_1 = NULL;
    CCB *cel_2 = NULL;
    // GroupCel
    GroupCel *groupCel = NULL;
    
    // Transformations
    int32 rotationAngle = 0;
    Point2D rotationPivot = {0};
    Quad2D stretch = {0};
    
    // Loads the CEL
    // printf("LoadCel()\n");
    cel_1 = LoadCel("image_1.cel", MEMTYPE_DRAM);
    // If it fails
    if(cel_1 == NULL){
        // Return an error
        printf("Error <- LoadCel()\n");
        return -1;
    }   

    // Loads the CEL
    // printf("LoadCel()\n");
    cel_2 = LoadCel("image_2.cel", MEMTYPE_DRAM);
    // If it fails
    if(cel_2 == NULL){
        // Return an error
        printf("Error <- LoadCel()\n");
        return -1;
    }   
    
    // Creates a GroupCel
    // printf("GroupCelInitialization()\n");
    groupCel = GroupCelInitialization(
        // Position
        10, 10, 
        // Size
        20, 30, 
        // Number of Cels
        2
    );
    // If it fails
    if(groupCel == NULL){
        // Return an error
        printf("Error <- GroupCelInitialization()\n");
        return -1;
    }   
    
    // Configure the GroupCel
    printf("-> GroupCelElementsConfiguration()\n");
    if (GroupCelElementsConfiguration(
        // GroupCel
        groupCel,
        // Start of list
        LIST_START,
        // celIndex, CCB, positionX, positionY
        0, cel_1, 0, 0,
        1, cel_2, 0, 0,
        // End of list
        LIST_END
    ) == 0) {
        printf("Error <- GroupCelElementsConfiguration()\n");
        return -1;
    }   
    
    // Prepares transformations
    rotationAngle = 128;
    rotationPivot.x = 50;
    rotationPivot.y = 50;
    stretch.bottomRight.x = 10;
    
    // Rotation
    GroupCelRotate(groupCel, rotationAngle, character->position.x, character->position.y);
    
    // Stretching
    GroupCelStretch(groupCel, 
        // Point 0
        groupCel->position.x + stretch.topLeft.x,
        groupCel->position.y + stretch.topLeft.y,
        // Point 1
        groupCel->position.x + groupCel->size.x + stretch.topRight.x,
        groupCel->position.y + stretch.topRight.y,
        // Point 2
        groupCel->position.x + groupCel->size.x + stretch.bottomRight.x,
        groupCel->position.y + groupCel->size.y + stretch.bottomRight.y,
        // Point 3
        groupCel->position.x + stretch.bottomLeft.x,
        groupCel->position.y + groupCel->size.y + stretch.bottomLeft.y
    );

    // Updates the elements in the GroupCel
    GroupCelUpdate(groupCel);
    
    // Deletes the GroupCel
    SpriteCelCleanup(groupCel);
    
    // Deletes the Cels
    UnloadCel(cel_1);
    UnloadCel(cel_2);
    
    // Ends the program
    return 0;
}
