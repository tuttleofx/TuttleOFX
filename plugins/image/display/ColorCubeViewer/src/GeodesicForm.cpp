#include "GeodesicForm.hpp"

#include <tuttle/plugin/opengl/gl.h>

namespace tuttle
{
namespace plugin
{
namespace colorCubeViewer
{

/*
 * GeodesicForm constructor
 */
GeodesicForm::GeodesicForm()
{
    // reserve memory
    _points.reserve(128); // reserve necessay points
    _faces.reserve(8);    // there are 8 big face in the Geodesic form (two pyramids)
    // initialize intersection attribute
    _hasIntersection = false; // there is not intersection
    _idFaceIntersection = 9;  // there is not intersection face
}

/*
 * Create a GeodesicForm centered in a point
 */
void GeodesicForm::createGeodesicForm(const Ofx3DPointD& center)
{
    double kRadiusFromCenter = 0.01f; // radius of GeodesicForm
    // double kRadiusFromCenter = .5f;                        // radius of GeodesicForm
    double altitude = sqrt((double)3.0) / 2 * kRadiusFromCenter; // altitude of an equilateral triangle is sqrt(3)/2*a

    // compute top and bottom point of GeodesicForm only Y value
    double topPointY = altitude;     // top
    double bottomPointY = -altitude; // bottom
    // construct top and bottom points
    Ofx3DPointD top, bottom;
    // top
    top.x = center.x;             // X axis
    top.y = center.y + topPointY; // Y axis
    top.z = center.z;             // Z axis
    // bottom
    bottom.x = center.x;                // X axis
    bottom.y = center.y + bottomPointY; // Y axis
    bottom.z = center.z;                // Z axis

    /*
    // square base
    //          z
    //         /|\  base2
    //        / | \
    //       /  |  \
    //      /   |   \
    //base1/    -----\--------- x
    //     \ center /  base3
    //      \      /
    //       \    /
    //        \  /
    //         \/base4
    //
    */
    // define square base points
    Ofx3DPointD base1, base2, base3, base4;
    // compute square diagonal
    double squareDiag = kRadiusFromCenter * sqrt((double)2.0); // sqrt(2)*a
    // base1
    base1.x = center.x - (squareDiag / 2.0); // X axis (use square diagonal to translate on x axis)
    base1.y = center.y;                      // Y axis
    base1.z = center.z;                      // Z axis
    // base2
    base2.x = center.x;                      // X axis
    base2.y = center.y;                      // Y axis
    base2.z = center.z + (squareDiag / 2.0); // Z axis
    // base3
    base3.x = center.x + (squareDiag / 2.0); // X axis
    base3.y = center.y;                      // Y axis
    base3.z = center.z;                      // Z axis
    // base4
    base4.x = center.x;                      // X axis
    base4.y = center.y;                      // Y axis
    base4.z = center.z - (squareDiag / 2.0); // Z axis

    // Add points to GeodesicForm vector
    _points.push_back(top);    // add top (first)
    _points.push_back(bottom); // add bottom
    _points.push_back(base1);  // base 1
    _points.push_back(base2);  // base 2
    _points.push_back(base3);  // base 3
    _points.push_back(base4);  // base 4

    /*
    //construct top and bottom triangles
    //          base1
    //          /\
    //         /  \         z
    //        /    \        |
    //       /______\       |
    //  base2        base3  ------x
    */
    // Define main faces (PyramidTriangles) of GeodesicForm
    PyramidTriangle Ptop1, Ptop2, Ptop3, Ptop4; // top faces
    PyramidTriangle Pbottom1, Pbottom2, Pbottom3, Pbottom4; // bottom faces
    // start with top faces
    Ptop1.point1 = &(_points[0]); // top
    Ptop1.point2 = &(_points[2]); // base1
    Ptop1.point3 = &(_points[3]); // base2
    // Ptop2
    Ptop2.point1 = &(_points[0]); // top
    Ptop2.point2 = &(_points[3]); // base2
    Ptop2.point3 = &(_points[4]); // base3
    // Ptop3
    Ptop3.point1 = &(_points[0]); // top
    Ptop3.point2 = &(_points[4]); // base3
    Ptop3.point3 = &(_points[5]); // base4
    // Ptop4
    Ptop4.point1 = &(_points[0]); // top
    Ptop4.point2 = &(_points[5]); // base4
    Ptop4.point3 = &(_points[2]); // base1

    // Bottom
    Pbottom1.point1 = &(_points[1]); // bottom
    Pbottom1.point2 = &(_points[2]); // base1
    Pbottom1.point3 = &(_points[3]); // base2
    // Pbottom2
    Pbottom2.point1 = &(_points[1]); // bottom
    Pbottom2.point2 = &(_points[3]); // base2
    Pbottom2.point3 = &(_points[4]); // base3
    // Pbottom3
    Pbottom3.point1 = &(_points[1]); // bottom
    Pbottom3.point2 = &(_points[4]); // base3
    Pbottom3.point3 = &(_points[5]); // base4
    // Pbottom4
    Pbottom4.point1 = &(_points[1]); // bottom
    Pbottom4.point2 = &(_points[5]); // base4
    Pbottom4.point3 = &(_points[2]); // base2

    // Add faces to the vector
    _faces.push_back(Ptop1);    // add top1    to faces vector
    _faces.push_back(Ptop2);    // add top2    to faces vector
    _faces.push_back(Ptop3);    // add top3    to faces vector
    _faces.push_back(Ptop4);    // add top4    to faces vector
    _faces.push_back(Pbottom1); // add bottom1 to faces vector
    _faces.push_back(Pbottom2); // add bottom2 to faces vector
    _faces.push_back(Pbottom3); // add bottom3 to faces vector
    _faces.push_back(Pbottom4); // add bottom4 to faces vector
}

/*
 *Draw the geodesic form on screen
 */
void GeodesicForm::draw(bool alpha)
{
    if(_points.empty() || _faces.empty()) // GeodesicForm has not been created
        return;

    // draw the geodesic form on screen (start with faces)
    for(unsigned int i = 0; i < _faces.size(); ++i)
    {
        // draw triangles of the face
        if(!_faces[i].triangles.empty()) // if there are triangles (subdivision) in the current face
        {
            glBegin(GL_TRIANGLES);
            for(unsigned int j = 0; j < _faces[i].triangles.size(); ++j) // for each triangle of the current face
            {
                if(alpha)
                    glColor4f(0.9, 0.9, 0.1, 0.3); // color yellow with alpha
                else
                    glColor3f(0.9, 0.5, 0.1);                       // color yellow without alpha
                Ofx3DPointD tr1 = *(_faces[i].triangles[j].point1); // get point1 of current triangle
                glVertex3d(tr1.x, tr1.y, tr1.z);                    // draw point
                Ofx3DPointD tr2 = *(_faces[i].triangles[j].point2); // get point2 of current triangle
                glVertex3d(tr2.x, tr2.y, tr2.z);                    // draw point
                Ofx3DPointD tr3 = *(_faces[i].triangles[j].point3); // get point3 of current triangle
                glVertex3d(tr3.x, tr3.y, tr3.z);                    // draw point
            }
            glEnd();
            if(alpha)
                glColor4f(0.1, 0.1, 0.1, 0.3); // color red with alpha
            else
                glColor3f(0.1, 0.1, 0.1); // color red without alpha
            glBegin(GL_LINES);
            for(unsigned int j = 0; j < _faces[i].triangles.size(); ++j) // for each triangle of the current face
            {
                // segment 1
                Ofx3DPointD tr1 = *(_faces[i].triangles[j].point1); // get point1 of current triangle
                glVertex3d(tr1.x, tr1.y, tr1.z);                    // draw point
                Ofx3DPointD tr2 = *(_faces[i].triangles[j].point2); // get point2 of current triangle
                glVertex3d(tr2.x, tr2.y, tr2.z);                    // draw point
                // segment 2
                glVertex3d(tr2.x, tr2.y, tr2.z);                    // draw point
                Ofx3DPointD tr3 = *(_faces[i].triangles[j].point3); // get point3 of current triangle
                glVertex3d(tr3.x, tr3.y, tr3.z);                    // draw point
                // segment 3
                glVertex3d(tr3.x, tr3.y, tr3.z); // draw point
                glVertex3d(tr1.x, tr1.y, tr1.z); // draw point
            }
            glEnd();
        }
    }
    // draw all of the points of the point vector
    glBegin(GL_POINTS);
    if(alpha)
        glColor3f(0.9, 0.9, 0.1); // color yellow
    else
        glColor3f(0.9, 0.3, 0.1); // color orange-red
    // draw points
    for(unsigned int i = 0; i < _points.size(); ++i)          // for each points in point vector
        glVertex3d(_points[i].x, _points[i].y, _points[i].z); // draw current point
    glEnd();

    // draw intersection test
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f); // color white
    glVertex3d(0.0648727, 0.149031, 0.0420739);
    glVertex3d(_center.x, _center.y, _center.z); // intersection line
    glEnd();

    // draw intersection triangle
    if(_hasIntersection)
    {
        // draw intersection triangle
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0, 0);                                                                 // color red
        glVertex3d(_intersection.point1->x, _intersection.point1->y, _intersection.point1->z); // point 1
        glVertex3d(_intersection.point2->x, _intersection.point2->y, _intersection.point2->z); // point 2
        glVertex3d(_intersection.point3->x, _intersection.point3->y, _intersection.point3->z); // point 3
        glEnd();

        // draw intersection point
        glBegin(GL_POINTS);
        glColor3f(0.0f, 1.0f, 1.0f); // color cyan
        glVertex3d(_intersectionPoint.x, _intersectionPoint.y, _intersectionPoint.z); // intersection point
        glEnd();
    }
}

void GeodesicForm::testOnePointFunction()
{
    // initialize test point
    Ofx3DPointD testPoint;
    // set testPoint values
    testPoint.x = 0.0848727; // x value
    testPoint.y = 0.849031;  // y value
    testPoint.z = 0.0420739; // z value

    extendOnePoint(testPoint); // extends test point

    // compute vector
    double testPointCenterVector[3];                    // initialize
    testPointCenterVector[0] = testPoint.x - _center.x; // x value
    testPointCenterVector[1] = testPoint.y - _center.y; // y value
    testPointCenterVector[2] = testPoint.z - _center.z; // z value
    // compute norm
    double normTestPointCenter = 0;                                             // initialize
    normTestPointCenter += testPointCenterVector[0] * testPointCenterVector[0]; // add x*x value
    normTestPointCenter += testPointCenterVector[1] * testPointCenterVector[1]; // add y*y value
    normTestPointCenter += testPointCenterVector[2] * testPointCenterVector[2]; // add z*z value
    normTestPointCenter = sqrt(normTestPointCenter);                            // compute norm

    std::cout << "norm Intersection normal : " << normTestPointCenter << std::endl; // display norm

    if(isPointIntoGeodesicForm(testPoint)) // test point inverse
        std::cout << "le point a été absorbé" << std::endl;
    else
        std::cout << "le point est toujours en dehors de la forme" << std::endl;
}

/*
 * Subdivise each face of the GeodesicForm
 */
void GeodesicForm::subdiviseFaces(const Ofx3DPointD& center, const int divisor)
{
    _points.clear(); // reset points vector
    _faces.clear();  // reset faces vector

    // reserve necessary memory for points
    _points.reserve(divisor * divisor * 8); // reserve necessary points

    // keep center and number of divisions
    _center = center;       // keep center
    _nbDivisions = divisor; // keep number of divisions on each face

    // reset bounding box
    _boundingBox.max = _center; // reset max value
    _boundingBox.min = _center; // reset min value

    // create face
    createGeodesicForm(center); // place geodesic form at center point

    for(unsigned int i = 0; i < _faces.size(); ++i) // for each face
    {
        createPointsOneFace(_faces[i], divisor); // create points of this face
        subdiviseOneFace(_faces[i], divisor);    // create triangles of the face
    }
    // transform geodesic form to sphere
    double kRadiusFromCenter = 0.01; // radius of GeodesicForm
    _radius = kRadiusFromCenter;     // keep radius value

    for(unsigned int i = 0; i < _points.size(); ++i)
        currentPointToSphere(_points[i], kRadiusFromCenter); // translate current point

    ///@todo:remove
    // testOnePointFunction(); //test intersection and extends with only one point
}

/*
 * Subdivise one face on the GeodesicForm
 */
void GeodesicForm::subdiviseOneFace(PyramidTriangle& f, const int divisor)
{
    // Define steps to construct triangles
    Ofx3DPointD Ystep, Xstep; // one for X axis and another for Y axis
    // Y step
    Ystep.x = double((f.point1->x - f.point2->x) / (double)divisor); // X axis
    Ystep.y = double((f.point1->y - f.point2->y) / (double)divisor); // Y axis
    Ystep.z = double((f.point1->z - f.point2->z) / (double)divisor); // Z axis
    // X step
    Xstep.x = double((f.point3->x - f.point2->x) / (double)divisor); // X axis
    Xstep.y = double((f.point3->y - f.point2->y) / (double)divisor); // Y axis
    Xstep.z = double((f.point3->z - f.point2->z) / (double)divisor); // Z axis

    // Triangle construction
    Ofx3DPointD pX, pX2, pY; // needed points to building triangles
    pX2.x = pX.x = pY.x = f.point2->x; // initialize X axis
    pX2.y = pX.y = pY.y = f.point2->y; // initialize Y axis
    pX2.z = pX.z = pY.z = f.point2->z; // initialize Z axis

    double epsilon = (double)(1.0 / 100000.0); // epsilon (for test)

    // Create all needed points
    for(unsigned int y = divisor; y > 0; --y) // Y axis
    {
        // place X cursor at the begin of the line
        pX.x = pY.x; // on X value
        pX.y = pY.y; // on Y value
        pX.z = pY.z; // on Z value
        // place X2 cursor at the begin of the line
        pX2.x = pY.x; // on X value
        pX2.y = pY.y; // on Y value
        pX2.z = pY.z; // on Z value

        // increments pX2
        pX2.x += Xstep.x; // increments X value
        pX2.y += Xstep.y; // increments Y value
        pX2.z += Xstep.z; // increments Z value

        for(unsigned int x = 0; x < y; ++x) // X axis
        {
            Ofx3DPointD up, bottom; // up and bottom
            // compute up
            up.x = (double)(pX.x + Ystep.x); // compute X axis
            up.y = (double)(pX.y + Ystep.y); // compute Y axis
            up.z = (double)(pX.z + Ystep.z); // compute Z axis

            // compute bottom
            bottom.x = (double)(pX2.x - Ystep.x); // compute X axis
            bottom.y = (double)(pX2.y - Ystep.y); // compute Y axis
            bottom.z = (double)(pX2.z - Ystep.z); // compute Z axis

            // find triangle point1, point2 and point3
            unsigned int pt1, pt2, pt3; // define indices
            // initialize indices
            pt1 = pt2 = pt3 = 0;
            // define booleans
            bool findPt1, findPt2, findPt3; // define booleans
            findPt1 = findPt2 = findPt3 = false; // set booleans to false

            // define point2
            while(!findPt2 && pt2 < _points.size()) // find point2
            {
                if(_points[pt2].x == pX.x && _points[pt2].y == pX.y && _points[pt2].z == pX.z) //_points[i] == point2
                    findPt2 = true; // point has been find
                else
                    ++pt2; // increments indice point2
            }

            // define point3
            while(!findPt3 && pt3 < _points.size()) // find point3
            {
                if(_points[pt3].x == pX2.x && _points[pt3].y == pX2.y && _points[pt3].z == pX2.z) //_points[i] == point3
                    findPt3 = true; // point has been find
                else
                    ++pt3; // increments indice point3
            }

            // define point1 (up for triangle)
            while(!findPt1 && pt1 < _points.size()) // find point 1 (up)
            {
                if(fabs(up.x - _points[pt1].x) < epsilon && fabs(up.y - _points[pt1].y) < epsilon &&
                   fabs(up.z - _points[pt1].z) < epsilon) //_points[i] == point3
                    findPt1 = true; // point has been find
                else
                    ++pt1; // increments indice point1
            }

            // Create and add triangle
            if(findPt1)
            {
                // define triangle to add
                Triangle trUp; // define
                // fill up point
                trUp.point1 = &(_points[pt1]); // point1 value
                trUp.point2 = &(_points[pt2]); // point2 value
                trUp.point3 = &(_points[pt3]); // point3 value
                // add triangle into face vector
                f.triangles.push_back(trUp); // add triangle at the end of vector
                // re-initialize findPt1
                findPt1 = false; // for next treatment
            }

            // re-initialize pt1
            pt1 = 0;
            // define point1 (bottom for triangle)
            while(!findPt1 && pt1 < _points.size()) // find point 1 (bottom)
            {
                if(fabs(bottom.x - _points[pt1].x) < epsilon && fabs(bottom.y - _points[pt1].y) < epsilon &&
                   fabs(bottom.z - _points[pt1].z) < epsilon) //_points[i] == point3
                    findPt1 = true; // point has been find
                else
                    ++pt1; // increments indice point1
            }
            // Create and add triangle
            if(findPt1)
            {
                // define triangle to add
                Triangle trBottom; // define
                // fill up point
                trBottom.point1 = &(_points[pt1]); // point1 value
                trBottom.point2 = &(_points[pt2]); // point2 value
                trBottom.point3 = &(_points[pt3]); // point3 value
                // add triangle into face vector
                f.triangles.push_back(trBottom); // add triangle at the end of vector
            }

            // increments X axis point
            pX.x += Xstep.x; // increments X value
            pX.y += Xstep.y; // increments Y value
            pX.z += Xstep.z; // increments Z value
            // increments X axis point
            pX2.x += Xstep.x; // increments X value
            pX2.y += Xstep.y; // increments Y value
            pX2.z += Xstep.z; // increments Z value

            // increments bottom on X axis
            bottom.x += Xstep.x; // increments X value
            bottom.y += Xstep.y; // increments Y value
            bottom.z += Xstep.z; // increments Z value
        }
        // increments Y axis point
        pY.x += Ystep.x; // increments X value
        pY.y += Ystep.y; // increments Y value
        pY.z += Ystep.z; // increments Z value
    }
}

/*
 * Create and add point of one sub-divised face
 */
void GeodesicForm::createPointsOneFace(const PyramidTriangle& f, const int divisor)
{
    // Stock points from face
    Ofx3DPointD facePoint1, facePoint2, facePoint3; // define
    // Create facePoint1
    facePoint1.x = f.point1->x; // x value
    facePoint1.y = f.point1->y; // y value
    facePoint1.z = f.point1->z; // z value
    // Create facePoint2
    facePoint2.x = f.point2->x; // x value
    facePoint2.y = f.point2->y; // y value
    facePoint2.z = f.point2->z; // z value
    // Create facePoint3
    facePoint3.x = f.point3->x; // x value
    facePoint3.y = f.point3->y; // y value
    facePoint3.z = f.point3->z; // z value

    // Define steps to construct triangles
    Ofx3DPointD Ystep, Xstep; // one for X axis and another for Y axis
    // Y step
    Ystep.x = double((facePoint1.x - facePoint2.x) / (double)divisor); // X axis
    Ystep.y = double((facePoint1.y - facePoint2.y) / (double)divisor); // Y axis
    Ystep.z = double((facePoint1.z - facePoint2.z) / (double)divisor); // Z axis
    // X step
    Xstep.x = double((facePoint3.x - facePoint2.x) / (double)divisor); // X axis
    Xstep.y = double((facePoint3.y - facePoint2.y) / (double)divisor); // Y axis
    Xstep.z = double((facePoint3.z - facePoint2.z) / (double)divisor); // Z axis

    // Triangle construction
    Ofx3DPointD pX, pY; // needed points to building triangles
    pX.x = pY.x = facePoint2.x; // initialize X axis
    pX.y = pY.y = facePoint2.y; // initialize Y axis
    pX.z = pY.z = facePoint2.z; // initialize Z axis

    // Create all needed points
    for(unsigned int y = divisor; y > 0; --y) // Y axis
    {
        // place X cursor at the begin of the line
        pX.x = pY.x; // on X value
        pX.y = pY.y; // on Y value
        pX.z = pY.z; // on Z value

        for(unsigned int x = 0; x <= y; ++x) // X axis
        {
            // Check current point is not one of the face
            if(pX.x == facePoint1.x && pX.y == facePoint1.y && pX.z == facePoint1.z)
            { // current point is f.point1
            }
            else if(pX.x == facePoint2.x && pX.y == facePoint2.y && pX.z == facePoint2.z)
            { // current point is f.point2
            }
            else if(pX.x == facePoint3.x && pX.y == facePoint3.y && pX.z == facePoint3.z)
            { // current point is f.point3
            }
            else
            {
                // current point is not already in the vector
                _points.push_back(pX); // add current point in vector
            }
            // increments X axis point
            pX.x += Xstep.x; // increments X value
            pX.y += Xstep.y; // increments Y value
            pX.z += Xstep.z; // increments Z value
        }
        // increments Y axis point
        pY.x += Ystep.x; // increments X value
        pY.y += Ystep.y; // increments Y value
        pY.z += Ystep.z; // increments Z value
    }
}

/*
 * scalar product between 2 vectors
 */
double DOT(const double* v1, const double* v2)
{
    return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

/*
 * compute intersection between a point and a triangle
 * based on Möller-Trumbore ray/triangle intersection detection algorithm
 * return if there is an intersection (or not) and the intersection point
 */
bool GeodesicForm::getIntersection(const Ofx3DPointD& point, const Triangle& triangle)
{
    bool isThereIntersection = false; // returned value
    static const double epsilon = 0.0000001; // epsilon value (used to test)

    // compute direction vector
    double directionVector[3]; // initialize direction vector
    directionVector[0] = _center.x - point.x; // compute X value
    directionVector[1] = _center.y - point.y; // compute Y value
    directionVector[2] = _center.z - point.z; // compute Z value

    // compute direction vectors of the triangle
    double directionVectorTriangle1[3]; // initialize first direction vector of the triangle
    double directionVectorTriangle2[3]; // initialize second direction vector of the triangle
    // compute values of first direction vector of the triangle
    directionVectorTriangle1[0] = triangle.point2->x - triangle.point1->x; // X value
    directionVectorTriangle1[1] = triangle.point2->y - triangle.point1->y; // Y value
    directionVectorTriangle1[2] = triangle.point2->z - triangle.point1->z; // Z value
    // compute values of second direction vector of the triangle
    directionVectorTriangle2[0] = triangle.point3->x - triangle.point1->x; // X value
    directionVectorTriangle2[1] = triangle.point3->y - triangle.point1->y; // Y value
    directionVectorTriangle2[2] = triangle.point3->z - triangle.point1->z; // Z value

    // Multiply directionVectorTriangle2 by directionVector
    double p[3]; // initialization
    // vector product
    p[0] = directionVector[1] * directionVectorTriangle2[2] - directionVector[2] * directionVectorTriangle2[1]; // X axis
    p[1] = directionVector[2] * directionVectorTriangle2[0] - directionVector[0] * directionVectorTriangle2[2]; // Y axis
    p[2] = directionVector[0] * directionVectorTriangle2[1] - directionVector[1] * directionVectorTriangle2[0]; // Z axis

    // First check if there is an intersection
    double firstCheck = DOT(directionVectorTriangle1, p); // initialize

    if(fabs(firstCheck) < epsilon) // if firstCheck == 0
    {
        // there is not intersection point data is wrong
        return isThereIntersection; // return false
    }
    else
    {
        // get invert of first check
        double invertFirstCheck = 1.0 / firstCheck;

        // try intersection between triangle and ray
        double s[3]; // initialize
        s[0] = point.x - triangle.point1->x; // X value
        s[1] = point.y - triangle.point1->y; // Y value
        s[2] = point.z - triangle.point1->z; // Z value

        // first test
        double u = DOT(s, p);
        u *= invertFirstCheck;
        if(u < 0 || u > 1.0)
        {
            // there is not an intersection point
            return isThereIntersection; // return false
        }

        // second try intersection between triangle and ray
        double q[3]; // initialize
        q[0] = s[1] * directionVectorTriangle1[2] - s[2] * directionVectorTriangle1[1];
        q[1] = s[2] * directionVectorTriangle1[0] - s[0] * directionVectorTriangle1[2];
        q[2] = s[0] * directionVectorTriangle1[1] - s[1] * directionVectorTriangle1[0];

        // second test
        double v = DOT(directionVector, q);
        v *= invertFirstCheck;
        if(v < 0 || u + v > 1.0)
        {
            // there is not an intersection point
            return isThereIntersection; // return false
        }

        // last test
        double t = DOT(directionVectorTriangle2, q);
        t *= invertFirstCheck;

        // there is an intersection point
        _intersectionPoint.x = (1 - u - v) * triangle.point1->x + u * triangle.point2->x + v * triangle.point3->x; // X value
        _intersectionPoint.y = (1 - u - v) * triangle.point1->y + u * triangle.point2->y + v * triangle.point3->y; // Y value
        _intersectionPoint.z = (1 - u - v) * triangle.point1->z + u * triangle.point2->z + v * triangle.point3->z; // Z value

        // change boolean value returned
        isThereIntersection = true; // return true
    }
    return isThereIntersection; // return if there is an intersection
}

/*
 * Test intersection between a ray (testPoint-center) and the geodesic form
 */
void GeodesicForm::testIntersection(const Ofx3DPointD& testPoint)
{
    _hasIntersection = false;
    bool intersectionFound = false;
    int i = 0; // initialize indice faces

    // research optimization
    Ofx3DPointD pointChangeReference; // testPoint is geodesicForm reference
    pointChangeReference.x = testPoint.x - _center.x; // X value in changed reference
    pointChangeReference.y = testPoint.y - _center.y; // Y value in changed reference
    pointChangeReference.z = testPoint.z - _center.z; // Z value in changed reference
    // select appropriated faces
    if(pointChangeReference.x < 0 && pointChangeReference.z >= 0)
    {
        i = 0;
    } // current point (X<0 && Z>0) => faces 0 and 4
    else if(pointChangeReference.x >= 0 && pointChangeReference.z >= 0)
    {
        i = 1;
    } // current point (X>0 && Z>0) => faces 1 and 5
    else if(pointChangeReference.x >= 0 && pointChangeReference.z < 0)
    {
        i = 2;
    } // current point (X>0 && Z<0) => faces 2 and 6
    else if(pointChangeReference.x < 0 && pointChangeReference.z < 0)
    {
        i = 3;
    } // current point (X<0 && Z<0) => faces 3 and 7

    while(i < 8 && !intersectionFound)
    {
        std::size_t j = 0; // initialize indice triangle
        while(j < _faces[i].triangles.size() && !intersectionFound)
        {
            // test intersection
            if(getIntersection(testPoint, _faces[i].triangles[j]))
            {
                // there is an intersection point
                intersectionFound = true; // An intersection has been found
                _idFaceIntersection = i; // Keep intersection face
                _intersection = _faces[i].triangles[j]; // Keep intersection triangle
                _hasIntersection = true; // An intersection has been found
            }
            ++j; // increments triangle indice
        }
        i += 4; // increments faces indice
    }
}

/*
 * Is point in the geodesicForm
 */
bool GeodesicForm::isPointIntoGeodesicForm(const Ofx3DPointD& testPoint)
{
    bool isPointInto = false; // initialize returned value

    // bounding box test
    if(!isIntoBoundingBox(testPoint)) // current point is not into bounding box
        return false;

    // compute first vector (between testPoint and GeodesicForm center)
    double testPointVector[3]; // initialize vector
    // compute first vector
    testPointVector[0] = testPoint.x - _center.x; // X value
    testPointVector[1] = testPoint.y - _center.y; // Y value
    testPointVector[2] = testPoint.z - _center.z; // Z value
    // compute first vector norm
    double normFirstVector = testPointVector[0] * testPointVector[0]; // initialize to x*x
    normFirstVector += testPointVector[1] * testPointVector[1]; // add y*y
    normFirstVector += testPointVector[2] * testPointVector[2]; // add z*z
    normFirstVector = sqrt(normFirstVector); // compute norm

    // Is current point into sphere
    if(normFirstVector <= _radius)
    {
        return true; // current point is into standard sphere
    }

    // test inverse intersection (if inverse intersection work point is into extended geodesic form)
    Ofx3DPointD pointChangeReference; // testPoint is geodesicForm reference
    pointChangeReference.x = testPoint.x - _center.x; // X value in changed reference
    pointChangeReference.y = testPoint.y - _center.y; // Y value in changed reference
    pointChangeReference.z = testPoint.z - _center.z; // Z value in changed reference
    // select appropriated faces
    int i = 0; // define face indice
    if(pointChangeReference.x < 0 && pointChangeReference.z >= 0)
    {
        i = 0;
    } // current point (X<0 && Z>0) => faces 0 and 4
    else if(pointChangeReference.x >= 0 && pointChangeReference.z >= 0)
    {
        i = 1;
    } // current point (X>0 && Z>0) => faces 1 and 5
    else if(pointChangeReference.x >= 0 && pointChangeReference.z < 0)
    {
        i = 2;
    } // current point (X>0 && Z<0) => faces 2 and 6
    else if(pointChangeReference.x < 0 && pointChangeReference.z < 0)
    {
        i = 3;
    } // current point (X<0 && Z<0) => faces 3 and 7
    // inverse intersection has not been found
    bool intersectionFound = false; // not found yet
    while(i < 8 && !intersectionFound)
    {
        unsigned int j = 0; // initialize triangle indice
        while(j < _faces[i].triangles.size() &&
              !intersectionFound) // while there are triangle on current faces and there is not intersection
        {
            // Construct PyramidTriangle needed to use intersection function
            PyramidTriangle tr; // initialization
            tr.point1 = _faces[i].triangles[j].point1; // set point1
            tr.point2 = _faces[i].triangles[j].point2; // set point2
            tr.point3 = _faces[i].triangles[j].point3; // set point3
            double s, t;
            if(getIntersection2(testPoint, tr, _intersectionPoint, s, t, true)) // test inverse intersection
            {
                intersectionFound = true; // there is an intersection
                isPointInto = true; // current point is into extended geodesic form
            }
            ++j; // increments triangle indice
        }
        i += 4; // increments face indice
    }
    return isPointInto; // return value
}

/*
 * Extend the geodesic form with only one point
 * Assumes that testPoint is not into the geodesicForm
 */
void GeodesicForm::extendOnePoint(Ofx3DPointD& testPoint)
{
    if(isPointIntoGeodesicForm(testPoint)) // current point is already into geodesic form (do not extends)
    {
        return; // end of function
    }
    if(!testIntersection2(testPoint)) // current point is not into geodesic form and there is not intersection
    {
        /*std::cout<<"[Problem] A ray has not intersection"<<std::endl;
        std::cout << "point : " << testPoint.x<<","<<testPoint.y<<","<<testPoint.z<<std::endl;*/
        return; // end of function
    }
    // compute angle between (testPoint-center) and (triangle points -center)
    double testPointCenterVector[3]; // initialize
    testPointCenterVector[0] = testPoint.x - _center.x; // X value
    testPointCenterVector[1] = testPoint.y - _center.y; // Y value
    testPointCenterVector[2] = testPoint.z - _center.z; // Z value
    // compute norm
    double normTestPointCenterVector = 0; // initialize
    normTestPointCenterVector += testPointCenterVector[0] * testPointCenterVector[0]; // add x*x
    normTestPointCenterVector += testPointCenterVector[1] * testPointCenterVector[1]; // add y*y
    normTestPointCenterVector += testPointCenterVector[2] * testPointCenterVector[2]; // add z*z
    normTestPointCenterVector = sqrt(normTestPointCenterVector); // compute norm
    // normalize testPointCenterVector
    testPointCenterVector[0] /= normTestPointCenterVector; // normalize X value
    testPointCenterVector[1] /= normTestPointCenterVector; // normalize Y value
    testPointCenterVector[2] /= normTestPointCenterVector; // normalize Z value

    // Extends triangle.point1
    if(extendsOneTrianglePoint(*(_intersection.point1), testPointCenterVector, normTestPointCenterVector)) // try to extends
        updateBoundingBox(*(_intersection.point1)); //_intersection.point1 has been changed => update bounding box
    // Extends triangle.point2
    if(extendsOneTrianglePoint(*(_intersection.point2), testPointCenterVector, normTestPointCenterVector)) // try to extends
        updateBoundingBox(*(_intersection.point2)); //_intersection.point2 has been changed => update bounding box
    // Extends triangle.point3
    if(extendsOneTrianglePoint(*(_intersection.point3), testPointCenterVector, normTestPointCenterVector)) // try to extends
        updateBoundingBox(*(_intersection.point3)); //_intersection.point3 has been changed => update bounding box
}

/*
 * Extends one point of the intersected triangle (call 3 times - triangle)
 */
bool GeodesicForm::extendsOneTrianglePoint(Ofx3DPointD& pointToMove, const double* testPointCenterVector,
                                           const double normTestPointCenterVector)
{

    // compute vector (center-point to Move)
    double vectorCenterPointToMove[3]; // initialize
    vectorCenterPointToMove[0] = pointToMove.x - _center.x; // X value
    vectorCenterPointToMove[1] = pointToMove.y - _center.y; // Y value
    vectorCenterPointToMove[2] = pointToMove.z - _center.z; // Z value
    // compute norm of vector (center-point to Move)
    double normVectorCenterPointToMove = 0; // initialize
    normVectorCenterPointToMove += vectorCenterPointToMove[0] * vectorCenterPointToMove[0]; // add x*x
    normVectorCenterPointToMove += vectorCenterPointToMove[1] * vectorCenterPointToMove[1]; // add y*y
    normVectorCenterPointToMove += vectorCenterPointToMove[2] * vectorCenterPointToMove[2]; // add z*z
    normVectorCenterPointToMove = sqrt(normVectorCenterPointToMove); // compute norm
    // normalize center-point to move vector
    vectorCenterPointToMove[0] /= normVectorCenterPointToMove; // normalize X value
    vectorCenterPointToMove[1] /= normVectorCenterPointToMove; // normalize Y value
    vectorCenterPointToMove[2] /= normVectorCenterPointToMove; // normalize Z value

    // compute translation triangle.point3
    double cosAngleIntersectionPointToMove = DOT(vectorCenterPointToMove, testPointCenterVector);
    // compute new norm triangle.point3
    double hypothenusCenterPointToMove = normTestPointCenterVector / cosAngleIntersectionPointToMove; // compute new norm
    if(normVectorCenterPointToMove > hypothenusCenterPointToMove) // compare current norm with existing norm
    {
        // nothing to do point to move is enough far of center
        return false; // point has not been changed
    }
    else
    {
        hypothenusCenterPointToMove -= normVectorCenterPointToMove; // compute value to add
        // compute translation
        vectorCenterPointToMove[0] *= hypothenusCenterPointToMove; // get coordinate X
        vectorCenterPointToMove[1] *= hypothenusCenterPointToMove; // get coordinate Y
        vectorCenterPointToMove[2] *= hypothenusCenterPointToMove; // get coordinate Z
        // translate triangle.point3
        pointToMove.x += vectorCenterPointToMove[0]; // translate point to move on X
        pointToMove.y += vectorCenterPointToMove[1]; // translate point to move on Y
        pointToMove.z += vectorCenterPointToMove[2]; // translate point to move on Z
        return true;
    }
}

/**
 * Transform one point of current pyramid to sphere
 * @param point current point to transform
 */
void GeodesicForm::currentPointToSphere(Ofx3DPointD& point, const double& radius)
{
    // compute vector between point-center
    double vectorCenterPoint[3]; // initialize
    vectorCenterPoint[0] = point.x - _center.x; // X value
    vectorCenterPoint[1] = point.y - _center.y; // Y value
    vectorCenterPoint[2] = point.z - _center.z; // Z value
    // compute norm
    double normVectorCenterPoint = 0; // initialize
    normVectorCenterPoint += vectorCenterPoint[0] * vectorCenterPoint[0]; // add x*x
    normVectorCenterPoint += vectorCenterPoint[1] * vectorCenterPoint[1]; // add y*y
    normVectorCenterPoint += vectorCenterPoint[2] * vectorCenterPoint[2]; // add z*z
    normVectorCenterPoint = sqrt(normVectorCenterPoint); // compute norm
    // normalize vector
    vectorCenterPoint[0] /= normVectorCenterPoint; // normalize X value
    vectorCenterPoint[1] /= normVectorCenterPoint; // normalize Y value
    vectorCenterPoint[2] /= normVectorCenterPoint; // normalize Z value
    // translate point (create sphere)
    point.x = vectorCenterPoint[0] * radius + _center.x; // translate point X
    point.y = vectorCenterPoint[1] * radius + _center.y; // translate point Y
    point.z = vectorCenterPoint[2] * radius + _center.z; // translate point Z
}

/**
 * intersection Dan Sunday : intersection of
 * @param point	point to compute ray
 * @param pyramidTriangle triangle face
 * @return if there is an intersection
 */
bool GeodesicForm::getIntersection2(const Ofx3DPointD& point, const PyramidTriangle& pyramidTriangle,
                                    Ofx3DPointD& intersectionPoint, double& s, double& t, const bool& inverse)
{
    double epsilon = 0.00000001; // define epsilon
    // Construct ray (intersection between ray and triangle)
    double ray[3]; // initialize
    if(!inverse) // normal ray
    {
        ray[0] = _center.x - point.x; // compute x value
        ray[1] = _center.y - point.y; // compute y value
        ray[2] = _center.z - point.z; // compute z value
    }
    else // inverse ray
    {
        ray[0] = point.x - _center.x; // compute x value
        ray[1] = point.y - _center.y; // compute y value
        ray[2] = point.z - _center.z; // compute z value
    }
    // Compute norm
    double normRay = 0; // initialize
    normRay += ray[0] * ray[0]; // add x*x
    normRay += ray[1] * ray[1]; // add y*y
    normRay += ray[2] * ray[2]; // add z*z
    normRay = sqrt(normRay); // compute norm
    // Normalize ray vector
    ray[0] /= normRay; // normalize x value
    ray[1] /= normRay; // normalize y value
    ray[2] /= normRay; // normalize z value

    // Compute triangle edge vectors and plane normal
    double u[3]; // initialize
    u[0] = pyramidTriangle.point2->x - pyramidTriangle.point1->x; // compute x value
    u[1] = pyramidTriangle.point2->y - pyramidTriangle.point1->y; // compute y value
    u[2] = pyramidTriangle.point2->z - pyramidTriangle.point1->z; // compute z value
    // Compute second edge
    double v[3]; // initialize
    v[0] = pyramidTriangle.point3->x - pyramidTriangle.point1->x; // compute x value
    v[1] = pyramidTriangle.point3->y - pyramidTriangle.point1->y; // compute y value
    v[2] = pyramidTriangle.point3->z - pyramidTriangle.point1->z; // compute z value

    // Cross product u*v
    double n[3]; // initialize
    n[0] = u[1] * v[2] - u[2] * v[1]; // compute x value
    n[1] = u[2] * v[0] - u[0] * v[2]; // compute y value
    n[2] = u[0] * v[1] - u[1] * v[0]; // compute z value

    if(n[0] == 0 && n[1] == 0 && n[2] == 0) // if cross product is null
    {
        // triangle is degenerate
        return false; // there is not intersection
    }
    // Test direction vector
    double wO[3]; // initialize
    wO[0] = point.x - pyramidTriangle.point1->x; // compute x value
    wO[1] = point.y - pyramidTriangle.point1->y; // compute y value
    wO[2] = point.z - pyramidTriangle.point1->z; // compute z value

    // Compute dot product
    double a = -DOT(n, wO);
    double b = DOT(n, ray);
    if(fabs(b) < epsilon)
    {
        // ray is parallel or disjoint to triangle plan
        return false; // there is not intersection
    }

    // get intersect point of ray with triangle plane
    double r = a / b;
    if(r < 0.0 || r > 1.0) // ray goes away from triangle
    {
        return false; // there is not intersection
    }
    // Compute intersection point
    intersectionPoint.x = point.x + r * ray[0]; // x value
    intersectionPoint.y = point.y + r * ray[1]; // y value
    intersectionPoint.z = point.z + r * ray[2]; // z value

    // std::cout << "intersection point : " << intersectionPoint.x <<","<<intersectionPoint.y<<","<<intersectionPoint.z <<
    // std::endl;
    // is I inside T?
    double uu, uv, vv, wu, wv, D;
    uu = DOT(u, u);
    uv = DOT(u, v);
    vv = DOT(v, v);

    double w[3]; // initialize
    w[0] = intersectionPoint.x - pyramidTriangle.point1->x; // compute x value
    w[1] = intersectionPoint.y - pyramidTriangle.point1->y; // compute y value
    w[2] = intersectionPoint.z - pyramidTriangle.point1->z; // compute z value

    wu = DOT(w, u);
    wv = DOT(w, v);
    D = uv * uv - uu * vv;

    // get and test parametric coordinates
    s = (uv * wv - vv * wu) / D;
    if(s < 0.0 || s > 1.0) // intersection point is outside pyramidTriangle
    {
        return false; // there is not intersection
    }
    t = (uv * wu - uu * wv) / D;
    if(t < 0.0 || (s + t) > 1.0) // intersection point is outside pyramidTriangle
    {
        return false; // there is not intersection
    }
    return true; // there is an intersection
}

/*
 * Test intersection between a ray and a pyramid face
 */
bool GeodesicForm::testIntersection2(const Ofx3DPointD& testPoint, const bool& inverse, const bool justIntersectionPoint)
{
    bool intersectionFound = false; // intersection has not been found
    int i = 0; // initialize indice faces

    // research optimization
    Ofx3DPointD pointChangeReference; // testPoint is geodesicForm reference
    pointChangeReference.x = testPoint.x - _center.x; // X value in changed reference
    pointChangeReference.y = testPoint.y - _center.y; // Y value in changed reference
    pointChangeReference.z = testPoint.z - _center.z; // Z value in changed reference
    // select appropriated faces
    if(pointChangeReference.x < 0 && pointChangeReference.z >= 0)
    {
        i = 0;
    } // current point (X<0 && Z>0) => faces 0 and 4
    else if(pointChangeReference.x >= 0 && pointChangeReference.z >= 0)
    {
        i = 1;
    } // current point (X>0 && Z>0) => faces 1 and 5
    else if(pointChangeReference.x >= 0 && pointChangeReference.z < 0)
    {
        i = 2;
    } // current point (X>0 && Z<0) => faces 2 and 6
    else if(pointChangeReference.x < 0 && pointChangeReference.z < 0)
    {
        i = 3;
    } // current point (X<0 && Z<0) => faces 3 and 7

    while(i < 8 && !intersectionFound)
    {
        // parametric coordinates
        double s, t; // initialize parametric coordinates
        if(getIntersection2(testPoint, _faces[i], _intersectionPoint, s, t,
                            inverse)) // test intersection with current face (stock parametric coordinates)
        {
            if(justIntersectionPoint) // if we just want to know if there is an intersection
                return true; // there is an intersection (no more computing)

            // compute pyramid side step
            double pyramidStep = (double)1 / (double)_nbDivisions; // compute pyramid side step
            // compute parametric coordinate axis X
            double parametricCoordinateX = floor(s / pyramidStep); // define parametric X axis value
            // compute parametric coordinate axis Y
            double parametricCoordinateY = floor(t / pyramidStep); // define parametric Y axis value

            // define triangle indice
            int indiceTr = 0;
            if(parametricCoordinateX == 0) // if intersected triangle is on the first line
            {
                indiceTr = 2 * parametricCoordinateY; // define intersected triangle indice on first line
            }
            else // intersected triangle indice is not on first line
            {
                unsigned int indice = 0; // define indice for previous triangle lines
                while(indice > parametricCoordinateX) // for each of previous lines
                {
                    indiceTr += (((int)_nbDivisions - indice) * 2) - 1; // increments triangle indice
                    ++indice; // increments indice
                }
                indiceTr += 2 * parametricCoordinateY; // terminate intersected triangle
            }
            if(indiceTr > 0) // triangle index starts from 0 (but not 1)
                indiceTr -= 1;
            // test intersection with computed intersection triangle
            bool hasIntersected = false; // intersection has not been done then
            int indiceIntersected = 0; // check intersection triangles (2 possibilities)
            while(!hasIntersected && indiceIntersected < 3)
            {
                // define pyramid triangle with current point
                PyramidTriangle pyTr; // define triangle pyTr
                pyTr.point1 = _faces[i].triangles[indiceTr].point1; // charge point1
                pyTr.point2 = _faces[i].triangles[indiceTr].point2; // charge point2
                pyTr.point3 = _faces[i].triangles[indiceTr].point3; // charge point3
                hasIntersected = getIntersection2(testPoint, pyTr, _intersectionPoint, s, t,
                                                  inverse); // test intersection with current triangle indice
                ++indiceTr; // increments indice triangle
                ++indiceIntersected; // increments check intersection triangle
            }
            if(!hasIntersected) // intersection has not been found yet (test hard method)
            {
                unsigned int j = 0; // declare indice to ride through triangle vector of current face
                while(j < _faces[i].triangles.size() && !intersectionFound) // test for each triangle of current face
                {
                    PyramidTriangle test; // initialize pyramid triangle
                    test.point1 = _faces[i].triangles[j].point1; // set point1
                    test.point2 = _faces[i].triangles[j].point2; // set point2
                    test.point3 = _faces[i].triangles[j].point3; // set point3
                    // parametric coordinates test
                    double sT, tT; // define parametric coordinate
                    if(getIntersection2(testPoint, test, _intersectionPoint, sT, tT))
                    {
                        // there is an intersection point
                        intersectionFound = true; // An intersection has been found
                        // set intersection triangle
                        _idFaceIntersection = i; // Keep intersection face
                        _hasIntersection = true; // An intersection has been found
                        _intersection = _faces[i].triangles[j]; // keep intersection triangle
                    }
                    ++j; // increments indice
                }
            }
        }
        i += 4; // increments faces indice
    }
    if(!intersectionFound)
        return false; // there is not intersection at this moment
    return true;
}

/*
 * update bounding box (if geodesic form point vector has changed cause of an extend)
 */
void GeodesicForm::updateBoundingBox()
{
    // reset bounding box
    _boundingBox.max = _center; // reset max value
    _boundingBox.min = _center; // reset min value

    // Compute new values
    for(unsigned int i = 0; i < _points.size(); ++i)
    {
        // test X axis
        if(_points[i].x > _boundingBox.max.x || _points[i].x < _boundingBox.min.x)
        {
            if(_points[i].x > _boundingBox.max.x)
                _boundingBox.max.x = _points[i].x; // change value max X
            else
                _boundingBox.min.x = _points[i].x; // change value min X
        }
        // test Y axis
        if(_points[i].y > _boundingBox.max.y || _points[i].y < _boundingBox.min.y)
        {
            if(_points[i].y > _boundingBox.max.y)
                _boundingBox.max.y = _points[i].y; // change value max Y
            else
                _boundingBox.min.y = _points[i].y; // change value min Y
        }
        // test Z axis
        if(_points[i].z > _boundingBox.max.z || _points[i].z < _boundingBox.min.z)
        {
            if(_points[i].z > _boundingBox.max.z)
                _boundingBox.max.z = _points[i].z; // change value max Z
            else
                _boundingBox.min.z = _points[i].z; // change value min Z
        }
    }
}

/*
 * Is current point into bounding box (optimization for process computing)
 */
bool GeodesicForm::isIntoBoundingBox(const Ofx3DPointD& testPoint)
{
    if(testPoint.x >= _boundingBox.min.x && testPoint.x <= _boundingBox.max.x) // test X axis
    {
        if(testPoint.y >= _boundingBox.min.y && testPoint.y <= _boundingBox.max.y) // test Y axis
        {
            if(testPoint.z >= _boundingBox.min.z && testPoint.z <= _boundingBox.max.z) // test Z axis
                return true; // current point is into bounding box
        }
    }
    return false; // current point is not into bounding box
}

/*
 * Update geodesic form (compare to one point)
 */
void GeodesicForm::updateBoundingBox(const Ofx3DPointD& testPoint)
{
    // test X axis
    if(testPoint.x > _boundingBox.max.x || testPoint.x < _boundingBox.min.x)
    {
        if(testPoint.x > _boundingBox.max.x)
            _boundingBox.max.x = testPoint.x; // change value max X
        else
            _boundingBox.min.x = testPoint.x; // change value min X
    }
    // test Y axis
    if(testPoint.y > _boundingBox.max.y || testPoint.y < _boundingBox.min.y)
    {
        if(testPoint.y > _boundingBox.max.y)
            _boundingBox.max.y = testPoint.y; // change value max Y
        else
            _boundingBox.min.y = testPoint.y; // change value min Y
    }
    // test Z axis
    if(testPoint.z > _boundingBox.max.z || testPoint.z < _boundingBox.min.z)
    {
        if(testPoint.z > _boundingBox.max.z)
            _boundingBox.max.z = testPoint.z; // change value max Z
        else
            _boundingBox.min.z = testPoint.z; // change value min Z
    }
}

/*
 * Recopy constructor of geodesic form
 */
void GeodesicForm::copyGeodesicForm(const GeodesicForm& copy)
{
    // Geodesic form parameters (recopy points)
    for(unsigned int i = 0; i < copy._points.size(); ++i)
        _points[i] = copy._points[i];
    // Copy bounding box parameters
    _boundingBox.max = copy._boundingBox.max; // copy max value
    _boundingBox.min = copy._boundingBox.min; // copy min value
}

/*
 * Scale geodesic form (multiplier result after extends)
 */
void GeodesicForm::scaleGeodesicForm(const double scale)
{
    for(unsigned int i = 0; i < _points.size(); ++i)
    {
        // compute vector
        double vect[3]; // initialize vector
        vect[0] = _points[i].x - _center.x; // compute x value
        vect[1] = _points[i].y - _center.y; // compute y value
        vect[2] = _points[i].z - _center.z; // compute z value

        // scale vector
        vect[0] *= scale;
        vect[1] *= scale;
        vect[2] *= scale;

        // set new values
        _points[i].x = vect[0] + _center.x; // set new X value
        _points[i].y = vect[1] + _center.y; // set new Y value
        _points[i].z = vect[2] + _center.z; // set new Z value
    }
}
}
}
}
