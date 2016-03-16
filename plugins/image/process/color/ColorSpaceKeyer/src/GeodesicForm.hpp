#ifndef GEODESICFORM_HPP
#define GEODESICFORM_HPP

#include "ColorSpaceKeyerDefinitions.hpp"
#include <vector>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

// Basic triangle structure
struct Triangle
{
    Ofx3DPointD* point1; // first position of the triangle
    Ofx3DPointD* point2; // second position of the triangle
    Ofx3DPointD* point3; // third position of the triangle
};

// Face triangle structure there are 6 big triangles (two pyramids)
struct PyramidTriangle
{
    Ofx3DPointD* point1; // first position of the face
    Ofx3DPointD* point2; // second position of the face
    Ofx3DPointD* point3; // third position of the face

    std::vector<Triangle> triangles; // contains all of the triangle of the face
};

// Bounding box structure (min and max of each axes)
struct BoundingBox
{
    Ofx3DPointD min; // min of X, Y and Z
    Ofx3DPointD max; // max of X, Y and Z
};

class GeodesicForm
{
public:
    // Geodesic form parameters
    std::size_t _nbDivisions; // number of divisions of each faces
    double _radius; // radius of geodesic form
    Ofx3DPointD _center; // center of the geodesic form
    std::vector<Ofx3DPointD> _points; // contains all of necessary points
    std::vector<PyramidTriangle> _faces; // contains triangles
    // Intersection parameters
    std::size_t _idFaceIntersection; // face of the current intersection
    Triangle _intersection; // triangle of the current intersection
    Ofx3DPointD _intersectionPoint; // point of intersection between ray and geodesic form
    bool _hasIntersection; // is there an intersection
    // Bounding box parameters
    BoundingBox _boundingBox; // bounding box of the geodesic form
    // Scale value
    double _scale; // scale geodesic form

public:
    // Constructor of geodesic form
    GeodesicForm();

    // Create a geodesic form centered in a point
    void createGeodesicForm(const Ofx3DPointD& center);
    // subdivise each face of GeodesicForm
    void subdiviseFaces(const Ofx3DPointD& center, const int divisor);

    // Draw the geodesic form on screen
    void draw(bool alpha = true);

    // Intersection test
    void testIntersection(const Ofx3DPointD& testPoint); // intersection (Moller) test between a ray and the geodesic form
    bool testIntersection2(
        const Ofx3DPointD& testPoint, const bool& inverse = false,
        const bool justIntersectionPoint = false); // intersection (Dan Sunday) test between a ray and the geodesic form
    bool isPointIntoGeodesicForm(const Ofx3DPointD& testPoint); // return if testPoint is contained into GeodesicForm

    // Extend geodesicForm
    void extendOnePoint(Ofx3DPointD& testPoint); // extend the geodesic form to contain testPoint

    // Bounding box
    void updateBoundingBox(); // if points vector has changed : update bounding box
    bool isIntoBoundingBox(const Ofx3DPointD& testPoint); // is testPoint into bounding box

    // Recopy geodesic form (evitate to extends spill form)
    void copyGeodesicForm(const GeodesicForm& copy);

    // Scale geodesic form (multiplier result after extends)
    void scaleGeodesicForm(const double scale);

    // test
    void testOnePointFunction();

private:
    // subdivise one face of the geodesic form
    void subdiviseOneFace(PyramidTriangle& f, const int divisor); // sub-divise one face
    // create and add point of one subdivised face
    void createPointsOneFace(const PyramidTriangle& f, const int divisor); // create all of needed point on one face
    // compute intersection between a point and a triangle
    bool getIntersection(const Ofx3DPointD& point, const Triangle& triangle); // intersection Moller
    bool getIntersection2(const Ofx3DPointD& point, const PyramidTriangle& pyramidTriangle, Ofx3DPointD& intersectionPoint,
                          double& s, double& t, const bool& inverse = false); // intersection Dan Sunday
    // transform one point to sphere
    void currentPointToSphere(Ofx3DPointD& point, const double& radius); // transform double pyramid to sphere
    // Extends one point
    bool extendsOneTrianglePoint(
        Ofx3DPointD& pointToMove, const double* testPointCenterVector,
        const double normTestPointCenterVector); // Extends one point of the intersected triangle (call 3 times - triangle)
    // update Bounding box
    void updateBoundingBox(const Ofx3DPointD& testPoint); // update bounding box (compare with one point)
                                                          // //test overlay
};
}
}
}

#endif /* GEODESICFORM_HPP */
