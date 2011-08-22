#ifndef GEODESICFORM_HPP
#define	GEODESICFORM_HPP

#include "ColorSpaceKeyerDefinitions.hpp"
#include <vector>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {
	
//Basic triangle structure
struct Triangle
{
	Ofx3DPointD* point1;	//first position of the triangle
	Ofx3DPointD* point2;	//second position of the triangle
	Ofx3DPointD* point3;	//third position of the triangle
};

//Face triangle structure there are 6 big triangles (two pyramids)
struct PyramidTriangle
{
	Ofx3DPointD* point1;	//first position of the face
	Ofx3DPointD* point2;	//second position of the face
	Ofx3DPointD* point3;	//third position of the face
	
	std::vector<Triangle> triangles;	//contains all of the triangle of the face
};

class GeodesicForm {
public:
	//Geodesic form parameters
	std::size_t _nbDivisions;					//number of divisions of each faces
	Ofx3DPointD _center;						//center of the geodesic form
	std::vector<Ofx3DPointD> _points;			//contains all of necessary points
	std::vector<PyramidTriangle> _faces;		//contains triangles
	//Intersection parameters
	std::size_t _idFaceIntersection;			//face of the current intersection
	Triangle _intersection;						//triangle of the current intersection
	Ofx3DPointD _intersectionPoint;				//point of intersection between ray and geodesic form
	bool _hasIntersection;						//is there an intersection
	
public:
	//Constructor of geodesic form
	GeodesicForm();
	
	//Create a geodesic form centered in a point
	void createGeodesicForm(Ofx3DPointD center);
	//subdivise each face of GeodesicForm
	void subdiviseFaces(Ofx3DPointD center, int divisor);
	
	//Draw the geodesic form on screen
	void draw();
	
	//Intersection test
	void testIntersection(Ofx3DPointD testPoint);	//intersection test between a ray and the geodesic form
	bool isPointIntoGeodesicForm(Ofx3DPointD testPoint);//return if testPoint is contained into GeodesicForm
	
	//Extend geodesicForm
	void extendOnePoint(Ofx3DPointD testPoint, Triangle intersectionTriangle); //extend the geodesic form to contain testPoint
	
private:
	//subdivise one face of the geodesic form
	void subdiviseOneFace(PyramidTriangle& f,int divisor);
	//create and add point of one subdivised face
	void createPointsOneFace(const PyramidTriangle f, int divisor);
	
	//compute intersection between a point and a triangle
	bool getIntersection(const Ofx3DPointD point, const Triangle triangle);
	
	//transform one point to sphere 
	void currentPointToSphere(Ofx3DPointD& point, const double radius);
	
	//test
	void testOnePointFunction();

};

}
}
}

#endif	/* GEODESICFORM_HPP */

