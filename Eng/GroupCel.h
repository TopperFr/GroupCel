#ifndef GROUPCEL_H
#define GROUPCEL_H

/******************************************************************************
**
**  GroupCel - Group of Cels, SpriteCels or AnimSpriteCels (3DO Cel Engine)
**  
**  Author : Christophe Geoffroy (Topper) - MIT License
**  Last updated : 08-05-2025
**
**  This module allows you to group Cels, SpriteCels or AnimSpriteCels into
**  a single structure. It enables movement, stretching and rotation of all
**  elements within the GroupCel as if it were a single object.
**  
**  Each Cel, SpriteCel or AnimSpriteCel retains its independence. Thus, it's
**  possible to modify the position or display of an individual element within the GroupCel.
**  
**  Other elements can be added by providing a CCB and a position,
**  such as a TextCel, for example.
** 
**  Important notes:
**
**    - The GroupCel has a rectangular frame defined by its position and size.
**      It is recommended to give this frame dimensions that enclose all elements.
**      This is not mandatory, but stretching takes the frame's corners into account.
**      Placing elements outside the frame may cause unexpected results.
**
**    - Elements are added directly into the structure without copying.
**      If a Cel is used outside of the group, use CloneCel() before adding it.
**
**    - The rotation pivot is relative to the screen. It is therefore possible to set
**      the pivot inside or outside the GroupCel frame.
**
**    - Each modification of the GroupCel (movement, stretching, rotation) must be
**      applied by calling the update function GroupCelUpdate(). This function 
**      monitors requested changes and performs only the necessary calculations.
**      A single call is sufficient for all transformations across all cels.
**
**    - All Cels in the group are automatically linked. A single call to DrawCels()
**      allows all the Cels in the GroupCel to be rendered. To add the GroupCel to a list
**      of Cels, you need to point to the main CCB. To add other Cels afterward,
**      you must continue the linking from the CCB of the last added Cel.
**
**    - When a GroupCel is initialized, it must be deleted using GroupCelCleanup().
**      Associated elements must be deleted independently.
**
**  Structure Roles :
**
**    GroupCelList
**      - position : Position of the element within the GroupCel
**      - cel : Element's CCB
**
**    GroupCel
**      - cel : Active CCB displayed on screen
**      - position : Position of the GroupCel
**      - size : Size of the GroupCel
**      - stretching : Storage for stretch parameters
**      - rotationAngle : Storage for rotation angle
**      - rotationPivot : Storage for rotation pivot point
**      - transformations : Flags to determine transformations to apply
**      - celsCount : Total number of Cels in the GroupCel
**      - cels : Dynamic array of "GroupCelList"
**
**  Main Functions :
**
**    GroupCelInitialization()
**      -> Initializes the GroupCel, its frame, and prepares the elements array.
**
**    GroupCelElementConfiguration()
**      -> Defines a GroupCel element: position and CCB.
**
**    GroupCelElementsConfiguration()
**      -> Defines multiple GroupCel elements in one pass using variadic arguments.
**
**    GroupCelMove()
**      -> Moves the GroupCel using relative values from its current position.
**
**    GroupCelSetPosition()
**      -> Moves the GroupCel using absolute values.
**
**    GroupCelElementMove()
**      -> Moves an element within the GroupCel using relative values from its current position.
**         A call to GroupCelUpdate() is necessary to apply the changes.
**
**    GroupCelElementSetPosition()
**      -> Moves an element within the GroupCel using absolute values within the GroupCel's frame.
**         A call to GroupCelUpdate() is necessary to apply the changes.
**
**    GroupCelStretch()
**      -> Stretches the GroupCel by modifying the four corners of the frame using absolute values.
**         A call to GroupCelUpdate() is necessary to apply the changes.
**
**    GroupCelRotate()
**      -> Rotates the GroupCel by specifying an angle and pivot using absolute values.
**         A call to GroupCelUpdate() is necessary to apply the changes.
**
**    GroupCelUpdate()
**      -> Updates the elements within the GroupCel.
**
**    GroupCelElementUpdatePosition()
**      -> Internal function applying position modifications.
**
**    GroupCelUpdateTransformations()
**      -> Internal function applying stretch and rotation modifications.
**
**    GroupCelCleanup()
**      -> Frees the memory used by the GroupCel structure.
**
******************************************************************************/

// CCB
#include "graphics.h"
// Point2D, Quad2D
#include "Geometry.h"
// int32
#include "types.h"

// Debug
#define DEBUG_GROUPCEL_INIT 0
#define DEBUG_GROUPCEL_SETUP 0
#define DEBUG_GROUPCEL_FUNCT 0
#define DEBUG_GROUPCEL_CLEAN 0

// Flag transformations
// bit-0 -> position
#define GROUPCEL_FLAG_MOVE (1 << 0)
// bit-1 -> stretch
#define GROUPCEL_FLAG_STRETCH (1 << 1)
// bit-2 -> rotation
#define GROUPCEL_FLAG_ROTATE (1 << 2)

typedef struct {
    // Position
    Point2D position;
    // Cel's CCB
    CCB *cel;
} GroupCelList;

typedef struct {
    // Main CCB of the group
    CCB *cel;
    // Position
    Point2D position;
    // Dimension
    Point2D size;
    // Stretch quadrilateral in pixels (local 0..x, 0..y)
    Quad2D stretching;      
    // Rotation parameters 
    // -> Angle in 256 units (90° = 64, 180° = 128, 270° = 192)
    uint32 rotationAngle;
    // -> Absolute pivot in pixels
    Point2D rotationPivot;
    // Transformations to apply (move = bit-0, stretch = bit-1, rotate = bit-2)
    int32 transformations;  
    // Total number of cels
    uint32 celsCount;
    // Array of cels
    GroupCelList *cels;
} GroupCel;

// Reference to the global context
extern GroupCel groupCel;

// Initialization of a GroupCel
GroupCel *GroupCelInitialization(int32 positionX, int32 positionY, uint32 sizeX, uint32 sizeY, uint32 celsCount);
// Configuration of a cel in a GroupCel
int32 GroupCelElementConfiguration(GroupCel *groupCel, uint32 celIndex, CCB *cel, uint32 positionX, uint32 positionY);
// Configuration of multiple cels in a GroupCel
int32 GroupCelElementsConfiguration(GroupCel *groupCel, int32 start, ...);

// Moves the GroupCel
void GroupCelMove(GroupCel *groupCel, int32 moveX, int32 moveY);
// Sets the position of the GroupCel
void GroupCelSetPosition(GroupCel *groupCel, int32 positionX, int32 positionY);

// Moves an element within the GroupCel
void GroupCelElementMove(GroupCel *groupCel, int32 celIndex, int32 moveX, int32 moveY);
// Sets the position of a cel in the GroupCel
void GroupCelElementSetPosition(GroupCel *groupCel, int32 celIndex, int32 positionX, int32 positionY);

// Stretches the group based on its 4 corner coordinates in pixels
void GroupCelStretch(GroupCel *groupCel, int32 point1X, int32 point1Y, int32 point2X, int32 point2Y, int32 point3X, int32 point3Y, int32 point4X, int32 point4Y);
// Rotates the group by an angle in 256 units
void GroupCelRotate(GroupCel *groupCel, uint32 angle, int32 pivotX, int32 pivotY);

// Updates elements within the GroupCel
void GroupCelUpdate(GroupCel *groupCel);
// Applies the position to the Cel's CCB
void GroupCelElementUpdatePosition(GroupCel *groupCel, int32 celIndex, Quad2D *quadMap);
// Applies Stretch and Rotate transformations to the CCBs of the Cels
void GroupCelUpdateTransformations(GroupCel *groupCel, int32 doStretch, int32 doRotate, Quad2D *quadMap);

// Deletes the GroupCel
int32 GroupCelCleanup(GroupCel *groupCel);

#endif // GROUPCEL_H