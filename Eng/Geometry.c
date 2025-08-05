
#include "Geometry.h"

// Convert32_F16, ConvertF16_32
#include "operamath.h"

// Gets the center point between two points
// -> Directly modifies the center position
void CenterPoint2D(Point2D *center, Point2D *point1, Point2D *point2) {

    // Calculates the center point
    center->x = (point1->x + point2->x) / 2;
    center->y = (point1->y + point2->y) / 2;

}

// Centers the point inside the rectangle
// -> Directly modifies the point position
void CenterPoint2DInRectangle(Rectangle *rectangle, Point2D *point) {
    
    // Calculates the center point in X
    point->x = rectangle->topLeft.x + (rectangle->bottomRight.x - rectangle->topLeft.x) / 2;
    // Calculates the center point in Y
    point->y = rectangle->topLeft.y + (rectangle->bottomRight.y - rectangle->topLeft.y) / 2;
    
}

void RotatePoint2D(int32 xi, int32 yi, int32 cx, int32 cy, int32 sinA, int32 cosA, int32 *ox, int32 *oy) {
    // delta to the pivot in Q16.16 format
    int32 dx = 0;
    int32 dy = 0;

    // rotation matrix [ cos -sin ; sin cos ] in Q16.16 format
    int32 rx = 0;
    int32 ry = 0;
    
    // delta to the pivot in Q16.16 format
    dx = Convert32_F16(xi - cx);
    dy = Convert32_F16(yi - cy);

    // rotation matrix [ cos -sin ; sin cos ] in Q16.16 format
    rx = (int32)MulSF16(dx, cosA) - (int32)MulSF16(dy, sinA);
    ry = (int32)MulSF16(dx, sinA) + (int32)MulSF16(dy, cosA);
    
    // convert back to pixels, recenter
    *ox = ConvertF16_32(rx) + cx;
    *oy = ConvertF16_32(ry) + cy;
}

// Centers the rectangle over the point
// -> Directly modifies the rectangle's position
void CenterRectangleOverPoint2D(Point2D *point, Rectangle *rectangle) {
    
    // Calculates the width and height of the rectangle
    int32 width = rectangle->bottomRight.x - rectangle->topLeft.x;
    int32 height = rectangle->bottomRight.y - rectangle->topLeft.y;
    
    // Calculates the new topLeft and bottomRight positions to center the rectangle over the point
    rectangle->topLeft.x = point->x - width / 2;
    rectangle->topLeft.y = point->y - height / 2;
    rectangle->bottomRight.x = point->x + width / 2;
    rectangle->bottomRight.y = point->y + height / 2;
    
}

// Positions a child rectangle inside a parent rectangle based on an alignment reference
// -> Directly modifies the position of the child rectangle
void PositionRectangleInRectangle(Rectangle *rectangle1, Rectangle *rectangle2, uint32 alignX, uint32 alignY, int32 offsetX, int32 offsetY) {
    
    // Calculates the width and height of the rectangle
    int32 width = rectangle2->bottomRight.x - rectangle2->topLeft.x;
    int32 height = rectangle2->bottomRight.y - rectangle2->topLeft.y;
    
    // Adjusts the horizontal position of the child rectangle’s top-left corner
    switch (alignX) {

        case LAYER_ALIGN_X_CENTER:
            rectangle2->topLeft.x = (rectangle1->bottomRight.x - rectangle1->topLeft.x - width) / 2 + offsetX;
            break;

        case LAYER_ALIGN_X_RIGHT:
            rectangle2->topLeft.x = rectangle1->bottomRight.x - width - offsetX;
            break;
            
        // LAYER_ALIGN_X_LEFT:
        default:
            rectangle2->topLeft.x = offsetX;
            break;
    }

    // Adjusts the vertical position of the child rectangle’s top-left corner
    switch (alignY) {

        case LAYER_ALIGN_Y_CENTER:
            rectangle2->topLeft.y = (rectangle1->bottomRight.y - rectangle1->topLeft.y - height) / 2 + offsetY;
            break;

        case LAYER_ALIGN_Y_BOTTOM:
            rectangle2->topLeft.y = rectangle1->bottomRight.y - height - offsetY;
            break;
            
        // LAYER_ALIGN_Y_TOP:
        default:
            rectangle2->topLeft.y = offsetY;
            break;
    }

    // Updates the bottom-right corner of the child rectangle
    rectangle2->bottomRight.x = rectangle2->topLeft.x + width;
    rectangle2->bottomRight.y = rectangle2->topLeft.y + height;
}


// Constrains a rectangle within a boundary
// -> Directly modifies the position of the rectangle
void BoundsRectangle(Rectangle *boundary, Rectangle *rectangle) {
    
    // Displacement values in X/Y
    Point2D offset;

    // Adjusts the horizontal position
    if (rectangle->topLeft.x < boundary->topLeft.x) {
        offset.x = boundary->topLeft.x - rectangle->topLeft.x;
        rectangle->topLeft.x += offset.x;
        rectangle->bottomRight.x += offset.x;
    } else if (rectangle->bottomRight.x > boundary->bottomRight.x) {
        offset.x = rectangle->bottomRight.x - boundary->bottomRight.x;
        rectangle->topLeft.x -= offset.x;
        rectangle->bottomRight.x -= offset.x;
    }

    // Adjusts the vertical position
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


// Offsets the point according to the X/Y value of the offset point
// -> Directly modifies the position of the point
void OffsetPoint2D(Point2D *point, Point2D *offset) {
        
    // Offsets the X position
    point->x += offset->x;
    // Offsets the Y position
    point->y += offset->y;
    
}

// Offsets the rectangle according to the X/Y value of the point
// -> Directly modifies the position of the rectangle
void OffsetRectangle(Rectangle *rectangle, Point2D *offset) {
        
    // Offsets the X positions
    rectangle->topLeft.x += offset->x;
    rectangle->bottomRight.x += offset->x;
    // Offsets the Y positions
    rectangle->topLeft.y += offset->y;
    rectangle->bottomRight.y += offset->y;
    
}

// Is the point inside the rectangle
// -> Returns 1 if the point is inside the rectangle, 0 if not
int32 Point2DInRectangle(Rectangle *rectangle, Point2D *point) {
    
    // Compares the position of the point with the rectangle's corners
    return ((point->x >= rectangle->topLeft.x) && (point->x <= rectangle->bottomRight.x) && (point->y >= rectangle->topLeft.y) && (point->y <= rectangle->bottomRight.y)) ? 1 : 0;
    
}

// Intersection of two rectangles
// -> Returns 1 if the rectangles overlap, 0 if not
int32 RectanglesIntersection(Rectangle *rectangle1, Rectangle *rectangle2) {
    
    // Compares the corners of the two rectangles
    return ((rectangle1->bottomRight.x >= rectangle2->topLeft.x && rectangle1->topLeft.x <= rectangle2->bottomRight.x) && 
            (rectangle1->bottomRight.y >= rectangle2->topLeft.y && rectangle1->topLeft.y <= rectangle2->bottomRight.y)) ? 1 : 0;

}

// Scaling of a Cel based on a multiplier
// -> Directly modifies the ccb_HDX and ccb_VDY values of the Cel
void CelScale(CCB *cel, int32 scaleX, int32 scaleY) {
    
    // Parameter correction
    scaleX = (scaleX > 2047) ? 2047 : scaleX;
    scaleX = (scaleX < -2048) ? -2048 : scaleX;
    scaleY = (scaleY > 32767) ? 32767 : scaleY;
    scaleY = (scaleY < -32768) ? -32768 : scaleY;
    
    // Modifies the values
    cel->ccb_HDX = scaleX << 20;
    cel->ccb_VDY = scaleY << 16;
}

// Scaling of a Cel based on a target size in pixels
// -> Directly modifies the ccb_HDX and ccb_VDY values of the Cel
void CelPixelScale(CCB *cel, uint32 sizeX, uint32 sizeY) {
    
    // Safety check to avoid division by zero
    if (cel->ccb_Width <= 0 || cel->ccb_Height <= 0) { return; }
    
    // Parameter correction
    sizeX = (sizeX == 0) ? 1 : sizeX;
    sizeY = (sizeY == 0) ? 1 : sizeY;
    
    // Calculates new scaling coefficients in fixed-point
    cel->ccb_HDX = (sizeX << 20) / cel->ccb_Width;
    cel->ccb_VDY = (sizeY << 16) / cel->ccb_Height;
}

// Removes scaling from a Cel
// -> Directly modifies the ccb_HDX and ccb_VDY values of the Cel
void CelUnscale(CCB *cel, uint32 unscaleX, uint32 unscaleY) {
    
    // Resets the Cel to its original size at 1:1 in X
    if (unscaleX > 0) { cel->ccb_HDX = 1 << 20; }
    // Resets the Cel to its original size at 1:1 in Y
    if (unscaleY > 0) { cel->ccb_VDY = 1 << 16; }
    
}

// Flips a Cel horizontally with or without position correction
// -> Directly modifies the ccb_HDX and ccb_XPos values of the Cel
void CelFlipHorizontal(CCB *cel, int32 flip, uint32 correction) {
    
    // Width of the CEL
    int32 width = 0;
    
    // Parameter correction
    flip = (flip < 0) ? -1 : 1;
    correction = (correction > 0) ? 1 : 0;
    
    // Retrieves the width
    width = cel->ccb_Width << 16;
    
    // If correction is needed
    if (correction) {
        // If there is a change in orientation
        if ((cel->ccb_HDX >= 0 && flip == -1) || (cel->ccb_HDX < 0 && flip == 1)) {
            // Adjust the position
            cel->ccb_XPos -= width;
        // Otherwise
        } else {
            // Return to original position
            cel->ccb_XPos += width;
        }
    }

    // Perform the flip
    cel->ccb_HDX *= flip;
}

// Flips a Cel vertically with or without position correction
// -> Directly modifies the ccb_VDY and ccb_YPos values of the Cel
void CelFlipVertical(CCB *cel, int32 flip, uint32 correction) {
    
    // Height of the CEL
    int32 height = 0;
    
    // Parameter correction
    flip = (flip < 0) ? -1 : 1;
    correction = (correction > 0) ? 1 : 0;

    // Retrieves the height
    height = cel->ccb_Height << 16;
    
    // If correction is needed
    if (correction) {
        // If there is a change in orientation
        if ((cel->ccb_VDY >= 0 && flip == -1) || (cel->ccb_VDY < 0 && flip == 1)) {
            // Adjust the position
            cel->ccb_YPos -= height;
        // Otherwise
        } else {
            // Return to original position
            cel->ccb_YPos += height;
        }
    }
    
    // Perform the flip
    cel->ccb_VDY *= flip;
}

// Returns the rectangle occupied on screen by the transformed Cel
// -> Directly modifies the rectangle values
void CelProjection(CCB *cel, Rectangle *rectangle) {

    // The coefficients (in fixed-point)
    int32 hdx = cel->ccb_HDX; // 12.20
    int32 hdy = cel->ccb_HDY; // 12.20
    int32 vdx = cel->ccb_VDX; // 16.16
    int32 vdy = cel->ccb_VDY; // 16.16

    // Corner 1: origin
    int32 px0 = cel->ccb_XPos;
    int32 py0 = cel->ccb_YPos;

    // Corner 2: move in X (width)
    int32 px1 = cel->ccb_XPos + ((hdx * cel->ccb_Width) >> 20);
    int32 py1 = cel->ccb_YPos + ((hdy * cel->ccb_Width) >> 20);

    // Corner 3: move in Y (height)
    int32 px2 = cel->ccb_XPos + ((vdx * cel->ccb_Height) >> 16);
    int32 py2 = cel->ccb_YPos + ((vdy * cel->ccb_Height) >> 16);

    // Corner 4: move in X + Y (diagonal)
    int32 px3 = cel->ccb_XPos + ((hdx * cel->ccb_Width) >> 20) + ((vdx * cel->ccb_Height) >> 16);
    int32 py3 = cel->ccb_YPos + ((hdy * cel->ccb_Width) >> 20) + ((vdy * cel->ccb_Height) >> 16);

    // Find min/max to enclose the Cel
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
