# GroupCel - Group of Cels, SpriteCels or AnimSpriteCels (3DO Cel Engine)

**Author**: Christophe Geoffroy (Topper)  
**License**: MIT  
**Last Updated**: 2025-08-05

## 🎬 Overview

**GroupCel** allows you to group Cels, SpriteCels or AnimSpriteCels into a single structure. It enables movement, stretching and rotation of all elements within the GroupCel as if it were a single object.

Each Cel, SpriteCel or AnimSpriteCel retains its independence. Thus, it's possible to modify the position or display of an individual element within the GroupCel.

Other elements can be added by providing a CCB and a position, such as a TextCel, for example.

## ⚠️ Important Notes

- The GroupCel has a rectangular frame defined by its position and size. It is recommended to give this frame dimensions that enclose all elements. This is not mandatory, but stretching takes the frame's corners into account. Placing elements outside the frame may cause unexpected results.
- Elements are added directly into the structure without copying. If a Cel is used outside of the group, use CloneCel() before adding it.
- The rotation pivot is relative to the screen. It is therefore possible to set the pivot inside or outside the GroupCel frame.
- Each modification of the GroupCel (movement, stretching, rotation) must be applied by calling the update function GroupCelUpdate(). This function monitors requested changes and performs only the necessary calculations. A single call is sufficient for all transformations across all cels.
- All Cels in the group are automatically linked. A single call to DrawCels() allows all the Cels in the GroupCel to be rendered. To add the GroupCel to a list of Cels, you need to point to the main CCB. To add other Cels afterward, you must continue the linking from the CCB of the last added Cel.
- When a GroupCel is initialized, it must be deleted using GroupCelCleanup(). Associated elements must be deleted independently.

## 🏗️ Structures

### `GroupCelList`

- `position`: Position of the element within the GroupCel
- `cel`: Element's CCB

### `GroupCel`

- `cel`: Active CCB displayed on screen
- `position`: Position of the GroupCel
- `size`: Size of the GroupCel
- `stretching`: Storage for stretch parameters
- `rotationAngle`: Storage for rotation angle
- `rotationPivot`: Storage for rotation pivot point
- `transformations`: Flags to determine transformations to apply
- `celsCount`: Total number of Cels in the GroupCel
- `cels`: Dynamic array of "GroupCelList"

## 🛠️ Core Functions

### `GroupCelInitialization()`
Initializes the GroupCel, its frame, and prepares the elements array.

### `GroupCelElementConfiguration()`
Defines a GroupCel element: position and CCB.

### `GroupCelElementsConfiguration()`
Defines multiple GroupCel elements in one pass using variadic arguments.

### `GroupCelMove()`
Moves the GroupCel using relative values from its current position.

### `GroupCelSetPosition()`
Moves the GroupCel using absolute values.

### `GroupCelElementMove()`
Moves an element within the GroupCel using relative values from its current position. A call to GroupCelUpdate() is necessary to apply the changes.

### `GroupCelElementSetPosition()`
Moves an element within the GroupCel using absolute values within the GroupCel's frame. A call to GroupCelUpdate() is necessary to apply the changes.

### `GroupCelStretch()`
Stretches the GroupCel by modifying the four corners of the frame using absolute values. A call to GroupCelUpdate() is necessary to apply the changes.

### `GroupCelRotate()`
Rotates the GroupCel by specifying an angle and pivot using absolute values. A call to GroupCelUpdate() is necessary to apply the changes.

### `GroupCelUpdate()`
Updates the elements within the GroupCel.

### `GroupCelElementUpdatePosition()`
Internal function applying position modifications.

### `GroupCelUpdateTransformations()`
Internal function applying stretch and rotation modifications.

### `GroupCelCleanup()`
Frees the memory used by the GroupCel structure.