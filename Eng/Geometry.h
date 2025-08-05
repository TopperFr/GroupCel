#ifndef LIBRARY_GEOMETRY_H
#define LIBRARY_GEOMETRY_H

// int32
#include "types.h"
// CCB
#include "graphics.h"

// Alignment for positioning
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
    Point2D topLeft;
    Point2D topRight;
    Point2D bottomRight;
    Point2D bottomLeft;
} Quad2D;

typedef struct Rectangle {
    Point2D topLeft;
    Point2D bottomRight;
} Rectangle;

// Gets the center point between two points
void CenterPoint2D(Point2D *center, Point2D *point1, Point2D *point2);

// Centers the point within the rectangle 
// -> Directly modifies the point's position
void CenterPoint2DInRectangle(Rectangle *rectangle, Point2D *point);

void RotatePoint2D(int32 xi, int32 yi, int32 cx, int32 cy, int32 sinA, int32 cosA, int32 *ox, int32 *oy);

// Centers the rectangle over the point 
// -> Directly modifies the rectangle's position
void CenterRectangleOverPoint2D(Point2D *point, Rectangle *rectangle);

// Positions a child rectangle inside a parent rectangle using alignment reference
// -> Directly modifies the child rectangle's position
void PositionRectangleInRectangle(Rectangle *rectangle1, Rectangle *rectangle2, uint32 alignX, uint32 alignY, int32 offsetX, int32 offsetY);

// Constrains the rectangle within a frame 
// -> Directly modifies the rectangle's position
void BoundsRectangle(Rectangle *boundary, Rectangle *rectangle);

// Offsets the point by the X/Y value of the offset point
// -> Directly modifies the point's position
void OffsetPoint2D(Point2D *point, Point2D *offset);

// Offsets the rectangle by the X/Y value of the point
// -> Directly modifies the rectangle's position
void OffsetRectangle(Rectangle *rectangle, Point2D *offset);

// Is the point inside the rectangle
// -> Returns 1 if the point is inside the rectangle, 0 otherwise
int32 Point2DInRectangle(Rectangle *rectangle, Point2D *point);

// Intersection of two rectangles
// -> Returns 1 if the rectangles overlap, 0 otherwise
int32 RectanglesIntersection(Rectangle *rectangle1, Rectangle *rectangle2);

/* Cel manipulation */

// Scales a Cel using a multiplier
// -> Directly modifies the ccb_HDX and ccb_VDY values of the Cel
void CelScale(CCB *cel, int32 scaleX, int32 scaleY);

// Scales a Cel to a target size in pixels
// -> Directly modifies the ccb_HDX and ccb_VDY values of the Cel
void CelPixelScale(CCB *cel, uint32 sizeX, uint32 sizeY);

// Removes scaling from a Cel
// -> Directly modifies the ccb_HDX and ccb_VDY values of the Cel
void CelUnscale(CCB *cel, uint32 unscaleX, uint32 unscaleY);

// Flips a Cel horizontally with or without position correction
// -> Directly modifies the ccb_HDX and ccb_XPos values of the Cel
void CelFlipHorizontal(CCB *cel, int32 flip, uint32 correction);

// Flips a Cel vertically with or without position correction
// -> Directly modifies the ccb_VDY and ccb_YPos values of the Cel
void CelFlipVertical(CCB *cel, int32 flip, uint32 correction);

// Returns the rectangle occupied on screen by the transformed Cel
// -> Directly modifies the rectangle values
void CelProjection(CCB *cel, Rectangle *rectangle);

#endif // LIBRARY_GEOMETRY_H
