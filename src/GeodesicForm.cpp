#include "GeodesicForm.hpp"

#include <tuttle/plugin/opengl/gl.h>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {
	
/**
 * GeodesicForm constructor
 */
GeodesicForm::GeodesicForm() {
	//reserve memory
	_points.reserve(128);		//reserve necessay points
	_faces.reserve(8);			//there are 8 big face in the Geodesic form (two pyramids)	
	//initialize intersection attribute
	_hasIntersection = false;	//there is not intersection
	_idFaceIntersection = 9;	//there is not intersection face
}

/*
 * Create a GeodesicForm centered in a point
 */
void GeodesicForm::createGeodesicForm(Ofx3DPointD center)
{
	double kRadiusFromCenter = 0.01f;				// radius of GeodesicForm
	//double kRadiusFromCenter = .5f;				// radius of GeodesicForm
	double altitude = sqrt(3)/2*kRadiusFromCenter;  // altitude of an equilateral triangle is sqrt(3)/2*a
	
	//compute top and bottom point of GeodesicForm only Y value
	double topPointY = altitude;		//top
	double bottomPointY = -altitude;	//bottom
	//construct top and bottom points
	Ofx3DPointD top,bottom;
	//top
	top.x = center.x;			//X axis
	top.y = center.y+topPointY;	//Y axis
	top.z = center.z;			//Z axis
	//bottom
	bottom.x = center.x;				//X axis
	bottom.y = center.y+bottomPointY;	//Y axis
	bottom.z = center.z;				//Z axis
	
	// square base
	//			z
	//		   /|\  base2
	//		  / | \
	//		 /	|  \
	//		/	|   \
	//base1/	-----\--------- x
	//     \ center /  base3
	//		\	   /
	//		 \	  /
	//		  \  /
	//		   \/base4
	//
	//define square base points
	Ofx3DPointD base1, base2, base3, base4;
	//compute square diagonal
	double squareDiag = kRadiusFromCenter*sqrt(2); // sqrt(2)*a
	//base1
	base1.x = center.x - (squareDiag/2.0);	//X axis (use square diagonal to translate on x axis)
	base1.y = center.y;						//Y axis
	base1.z = center.z;						//Z axis
	//base2
	base2.x	= center.x;						//X axis
	base2.y = center.y;						//Y axis
	base2.z = center.z + (squareDiag/2.0);	//Z axis
	//base3
	base3.x = center.x + (squareDiag/2.0);	//X axis
	base3.y = center.y;						//Y axis
	base3.z = center.z;						//Z axis
	//base4
	base4.x = center.x;						//X axis
	base4.y = center.y;						//Y axis
	base4.z = center.z - (squareDiag/2.0);	//Z axis
	
	//Add points to GeodesicForm vector
	_points.push_back(top);			//add top (first)
	_points.push_back(bottom);		//add bottom
	_points.push_back(base1);		//base 1
	_points.push_back(base2);		//base 2
	_points.push_back(base3);		//base 3
	_points.push_back(base4);		//base 4
	
	
	//construct top and bottom triangles
	//			   base1
	//				/\
	//			   /  \				z
	//			  /	   \		    | 
	//			 /______\			|
	//		base2        base3		------x
	
	//Define main faces (PyramidTriangles) of GeodesicForm
	PyramidTriangle Ptop1, Ptop2, Ptop3, Ptop4;				//top faces
	PyramidTriangle Pbottom1, Pbottom2, Pbottom3, Pbottom4;	//bottom faces
	//start with top faces
	Ptop1.point1 = &(_points[0]); //top
	Ptop1.point2 = &(_points[2]); //base1
	Ptop1.point3 = &(_points[3]); //base2
	//Ptop2
	Ptop2.point1 = &(_points[0]); //top
	Ptop2.point2 = &(_points[3]); //base2
	Ptop2.point3 = &(_points[4]); //base3
	//Ptop3
	Ptop3.point1 = &(_points[0]); //top
	Ptop3.point2 = &(_points[4]); //base3
	Ptop3.point3 = &(_points[5]); //base4
	//Ptop4
	Ptop4.point1 = &(_points[0]); //top
	Ptop4.point2 = &(_points[5]); //base4
	Ptop4.point3 = &(_points[2]); //base1
	
	//Bottom
	Pbottom1.point1 = &(_points[1]); //bottom
	Pbottom1.point2 = &(_points[2]); //base1
	Pbottom1.point3 = &(_points[3]); //base2
	//Pbottom2
	Pbottom2.point1 = &(_points[1]); //bottom
	Pbottom2.point2 = &(_points[3]); //base2
	Pbottom2.point3 = &(_points[4]); //base3
	//Pbottom3
	Pbottom3.point1 = &(_points[1]); //bottom
	Pbottom3.point2 = &(_points[4]); //base3
	Pbottom3.point3 = &(_points[5]); //base4
	//Pbottom4
	Pbottom4.point1 = &(_points[1]); //bottom
	Pbottom4.point2 = &(_points[5]); //base4
	Pbottom4.point3 = &(_points[2]); //base2
	
	//Add faces to the vector
	_faces.push_back(Ptop1);	//add top1 to faces vector
	_faces.push_back(Ptop2);	//add top2 to faces vector
	_faces.push_back(Ptop3);	//add top3 to faces vector
	_faces.push_back(Ptop4);	//add top4 to faces vector
	_faces.push_back(Pbottom1);	//add bottom1 to faces vector
	_faces.push_back(Pbottom2);	//add bottom2 to faces vector
	_faces.push_back(Pbottom3);	//add bottom3 to faces vector
	_faces.push_back(Pbottom4);	//add bottom4 to faces vector
}

/*
 *Draw the geodesic form on screen 
 */
void GeodesicForm::draw()
{
	if(_points.empty() || _faces.empty()) //GeodesicForm has not been created
		return;
	
	//draw the geodesic form on screen (start with faces)
	for(unsigned int i=0; i< _faces.size(); ++i)
	{
		/*glBegin(GL_TRIANGLES);
		glColor3f(0.9,0.9,0.1);	//color yellow
		Ofx3DPointD p1 = *(_faces[i].point1);	//point 1 of current face
		glVertex3d(p1.x,p1.y,p1.z);	//draw point
		Ofx3DPointD p2 = *(_faces[i].point2);	//point 2 of current face
		glVertex3f(p2.x,p2.y,p2.z); //draw point
		Ofx3DPointD p3 = *(_faces[i].point3);	//point 3 of current face
		glVertex3f(p3.x,p3.y,p3.z); //draw point
		glEnd();*/		
		//draw triangles of the face
		if(!_faces[i].triangles.empty())// if there are triangles (subdivision) in the current face
		{
			glBegin(GL_TRIANGLES);
			for(unsigned int j=0; j<_faces[i].triangles.size(); ++j) //for each triangle of the current face
			{
				glColor3f(0.9,0.9,0.1);	//color yellow
				Ofx3DPointD tr1 = *(_faces[i].triangles[j].point1); //get point1 of current triangle
				glVertex3d(tr1.x,tr1.y,tr1.z); //draw point
				Ofx3DPointD tr2 = *(_faces[i].triangles[j].point2); //get point2 of current triangle
				glVertex3d(tr2.x,tr2.y,tr2.z); //draw point
				Ofx3DPointD tr3 = *(_faces[i].triangles[j].point3); //get point3 of current triangle
				glVertex3d(tr3.x,tr3.y,tr3.z); //draw point
			}
			glEnd();
			glColor3f(0.9,0.1,0.1);//color red
			glBegin(GL_LINES);
			for(unsigned int j=0; j<_faces[i].triangles.size(); ++j) //for each triangle of the current face
			{
				//segment 1
				Ofx3DPointD tr1 = *(_faces[i].triangles[j].point1); //get point1 of current triangle
				glVertex3d(tr1.x,tr1.y,tr1.z); //draw point
				Ofx3DPointD tr2 = *(_faces[i].triangles[j].point2); //get point2 of current triangle
				glVertex3d(tr2.x,tr2.y,tr2.z); //draw point
				//segment 2
				glVertex3d(tr2.x,tr2.y,tr2.z); //draw point
				Ofx3DPointD tr3 = *(_faces[i].triangles[j].point3); //get point3 of current triangle
				glVertex3d(tr3.x,tr3.y,tr3.z); //draw point
				//segment 3
				glVertex3d(tr3.x,tr3.y,tr3.z); //draw point
				glVertex3d(tr1.x,tr1.y,tr1.z); //draw point
			}
			glEnd();
			
		}
	}
	//draw all of the points of the point vector
	glBegin(GL_POINTS);
	glColor3f(0.1,0.1,0.9); //color blue
	//draw points
	for(unsigned int i=0; i<_points.size(); ++i)				// for each points in point vector
		glVertex3d(_points[i].x, _points[i].y, _points[i].z);	//draw current point
	glEnd();
	
	//draw intersection test
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f); //color white
	glVertex3d(_center.x+0.1,_center.y+0.2,_center.z+0.1); glVertex3d(_center.x,_center.y, _center.z); //intersection line
	glEnd();
	
	//draw intersection triangle
	if(_hasIntersection)
	{
		//draw intersection triangle
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f,0,0); // color red
		glVertex3d(_intersection.point1->x, _intersection.point1->y, _intersection.point1->z); //point 1
		glVertex3d(_intersection.point2->x, _intersection.point2->y, _intersection.point2->z); //point 2
		glVertex3d(_intersection.point3->x, _intersection.point3->y, _intersection.point3->z); //point 3
		glEnd();
		//draw intersection point
		glBegin(GL_POINTS);
		glColor3f(0.0f,1.0f,1.0f); //color cyan
		glVertex3d(_intersectionPoint.x, _intersectionPoint.y, _intersectionPoint.z); //intersection point
		glEnd();
	}
}

void GeodesicForm::testOnePointFunction()
{
	//initialize test point
	Ofx3DPointD testPoint;
	//set testPoint values
	testPoint.x = _center.x+0.1; //x value
	testPoint.y = _center.y+0.2; //y value
	testPoint.z = _center.z+0.1; //z value
	//test intersection
	testIntersection(testPoint);
	if(_hasIntersection) //if there is an intersection
		extendOnePoint(testPoint,_intersection); //extends test point
}

/*
 * Subdivise each face of the GeodesicForm
 */
void GeodesicForm::subdiviseFaces(Ofx3DPointD center, int divisor)
{
	_points.clear(); //reset points vector
	_faces.clear();  //reset faces vector

	//reserve necessary memory for points
	_points.reserve(divisor*divisor*8);		//reserve necessary points
	
	//keep center and number of divisions
	_center = center; //keep center
	_nbDivisions = divisor; //keep number of divisions on each face	
	//create face
	createGeodesicForm(center);	//place geodesic form at center point
	
	for(unsigned int i=0; i<_faces.size(); ++i)	//for each face
	{
		createPointsOneFace(_faces[i],divisor);	//create points of this face
		subdiviseOneFace(_faces[i],divisor);    //create triangles of the face
	}
	//transform geodesic form to sphere
	double kRadiusFromCenter = 0.01; // radius of GeodesicForm
	for(unsigned int i=0; i<_points.size(); ++i)
		currentPointToSphere(_points[i], kRadiusFromCenter); //translate current point
	
	
	///@todo:remove 
	//testOnePointFunction(); //test intersection and extends with only one point
}

/*
 * Subdivise one face on the GeodesicForm
 */
void GeodesicForm::subdiviseOneFace(PyramidTriangle& f, int divisor)
{
	//Define steps to construct triangles
	Ofx3DPointD Ystep, Xstep;	//one for X axis and another for Y axis
	//Y step
	Ystep.x = double((f.point1->x-f.point2->x)/(double)divisor);	//X axis
	Ystep.y = double((f.point1->y-f.point2->y)/(double)divisor);	//Y axis
	Ystep.z = double((f.point1->z-f.point2->z)/(double)divisor);	//Z axis
	//X step
	Xstep.x = double((f.point3->x-f.point2->x)/(double)divisor);	//X axis
	Xstep.y = double((f.point3->y-f.point2->y)/(double)divisor);	//Y axis
	Xstep.z = double((f.point3->z-f.point2->z)/(double)divisor);	//Z axis
	
	//Triangle construction
	Ofx3DPointD pX, pX2, pY;		   //needed points to building triangles
	pX2.x = pX.x = pY.x = f.point2->x; //initialize X axis
	pX2.y = pX.y = pY.y = f.point2->y; //initialize Y axis
	pX2.z = pX.z = pY.z = f.point2->z; //initialize Z axis
	
	double epsilon = (double)(1.0/100000.0); //epsilon (for test)
	
	//Create all needed points
	for(unsigned int y= divisor; y>0; --y) //Y axis
	{
		//place X cursor at the begin of the line
		pX.x = pY.x; //on X value
		pX.y = pY.y; //on Y value
		pX.z = pY.z; //on Z value
		//place X2 cursor at the begin of the line
		pX2.x = pY.x; //on X value
		pX2.y = pY.y; //on Y value
		pX2.z = pY.z; //on Z value
		
		//increments pX2
		pX2.x += Xstep.x; //increments X value
		pX2.y += Xstep.y; //increments Y value
		pX2.z += Xstep.z; //increments Z value
		
		for(unsigned int x=0; x<y; ++x)	//X axis
		{	
			Ofx3DPointD up, bottom; //up and bottom
			//compute up
			up.x = (double)(pX.x+Ystep.x);		//compute X axis
			up.y = (double)(pX.y+Ystep.y);		//compute Y axis
			up.z = (double)(pX.z+Ystep.z);		//compute Z axis
			
			//compute bottom
			bottom.x = (double)(pX2.x-Ystep.x); //compute X axis
			bottom.y = (double)(pX2.y-Ystep.y); //compute Y axis
			bottom.z = (double)(pX2.z-Ystep.z); //compute Z axis
			
			//find triangle point1, point2 and point3
			unsigned int pt1,pt2,pt3; //define indices
			//initialize indices
			pt1 = pt2 = pt3 = 0;
			//define booleans
			bool findPt1, findPt2, findPt3;			//define booleans
			findPt1 = findPt2 = findPt3 = false;	//set booleans to false
			
			//define point2
			while(!findPt2 && pt2<_points.size()) //find point2
			{
				if(_points[pt2].x == pX.x && _points[pt2].y == pX.y && _points[pt2].z == pX.z) //_points[i] == point2
					findPt2 = true;	//point has been find
				else	
					++pt2;	//increments indice point2
			}
			
			//define point3
			while(!findPt3 && pt3<_points.size()) //find point3
			{
				if(_points[pt3].x == pX2.x && _points[pt3].y == pX2.y && _points[pt3].z == pX2.z) //_points[i] == point3
					findPt3 = true;	//point has been find
				else	
					++pt3;	//increments indice point3
			}
			
			//define point1 (up for triangle)
			while(!findPt1 && pt1<_points.size()) //find point 1 (up)
			{
				if(fabs(up.x-_points[pt1].x)<epsilon && fabs(up.y-_points[pt1].y)<epsilon  && fabs(up.z-_points[pt1].z)<epsilon ) //_points[i] == point3
					findPt1 = true;	//point has been find
				else	
					++pt1;	//increments indice point1
			}
			
			//Create and add triangle
			if(findPt1)
			{
				//define triangle to add
				Triangle trUp; //define
				//fill up point
				trUp.point1 = &(_points[pt1]); // point1 value
				trUp.point2 = &(_points[pt2]); // point2 value
				trUp.point3 = &(_points[pt3]); // point3 value
				//add triangle into face vector
				f.triangles.push_back(trUp); //add triangle at the end of vector
				//re-initialize findPt1
				findPt1 = false; //for next treatment
			}
			
			//re-initialize pt1
			pt1 = 0;
			//define point1 (bottom for triangle)
			while(!findPt1 && pt1<_points.size()) //find point 1 (bottom)
			{
				if(fabs(bottom.x-_points[pt1].x)<epsilon && fabs(bottom.y-_points[pt1].y)<epsilon  && fabs(bottom.z-_points[pt1].z)<epsilon ) //_points[i] == point3
					findPt1 = true;	//point has been find
				else	
					++pt1;	//increments indice point1
			}		
			//Create and add triangle
			if(findPt1)
			{
				//define triangle to add
				Triangle trBottom; //define
				//fill up point
				trBottom.point1 = &(_points[pt1]); // point1 value
				trBottom.point2 = &(_points[pt2]); // point2 value
				trBottom.point3 = &(_points[pt3]); // point3 value
				//add triangle into face vector
				f.triangles.push_back(trBottom); //add triangle at the end of vector
			}

			//increments X axis point
			pX.x += Xstep.x; //increments X value
			pX.y += Xstep.y; //increments Y value
			pX.z += Xstep.z; //increments Z value
			//increments X axis point
			pX2.x += Xstep.x; //increments X value
			pX2.y += Xstep.y; //increments Y value
			pX2.z += Xstep.z; //increments Z value
			
			//increments bottom on X axis
			bottom.x += Xstep.x; //increments X value
			bottom.y += Xstep.y; //increments Y value
			bottom.z += Xstep.z; //increments Z value
		}
		//increments Y axis point
		pY.x += Ystep.x; //increments X value
		pY.y += Ystep.y; //increments Y value
		pY.z += Ystep.z; //increments Z value
	}
}

/*
 * Create and add point of one subdivised face
 */
void GeodesicForm::createPointsOneFace(const PyramidTriangle f, int divisor)
{
	//Stock points from face
	Ofx3DPointD facePoint1, facePoint2, facePoint3; //define
	//Create facePoint1
	facePoint1.x = f.point1->x; //x value
	facePoint1.y = f.point1->y; //y value
	facePoint1.z = f.point1->z; //z value
	//Create facePoint2
	facePoint2.x = f.point2->x; //x value
	facePoint2.y = f.point2->y; //y value
	facePoint2.z = f.point2->z; //z value
	//Create facePoint3
	facePoint3.x = f.point3->x; //x value
	facePoint3.y = f.point3->y; //y value
	facePoint3.z = f.point3->z; //z value
	
	//Define steps to construct triangles
	Ofx3DPointD Ystep, Xstep;	//one for X axis and another for Y axis
	//Y step
	Ystep.x = double((facePoint1.x-facePoint2.x)/(double)divisor);	//X axis
	Ystep.y = double((facePoint1.y-facePoint2.y)/(double)divisor);	//Y axis
	Ystep.z = double((facePoint1.z-facePoint2.z)/(double)divisor);	//Z axis
	//X step
	Xstep.x = double((facePoint3.x-facePoint2.x)/(double)divisor);	//X axis
	Xstep.y = double((facePoint3.y-facePoint2.y)/(double)divisor);	//Y axis
	Xstep.z = double((facePoint3.z-facePoint2.z)/(double)divisor);	//Z axis
	
	//Triangle construction
	Ofx3DPointD pX, pY; //needed points to building triangles
	pX.x = pY.x = facePoint2.x; //initialize X axis
	pX.y = pY.y = facePoint2.y; //initialize Y axis
	pX.z = pY.z = facePoint2.z; //initialize Z axis
	
	//Create all needed points
	for(unsigned int y= divisor; y>0; --y) //Y axis
	{
		//place X cursor at the begin of the line
		pX.x = pY.x; //on X value
		pX.y = pY.y; //on Y value
		pX.z = pY.z; //on Z value
		
		for(unsigned int x=0; x<=y; ++x)	//X axis
		{
			//Check current point is not one of the face
			if(pX.x == facePoint1.x && pX.y == facePoint1.y && pX.z == facePoint1.z )
			{ //current point is f.point1
			} 
			else if(pX.x == facePoint2.x && pX.y == facePoint2.y && pX.z == facePoint2.z)
			{ //current point is f.point2
			}
			else if(pX.x == facePoint3.x && pX.y == facePoint3.y && pX.z == facePoint3.z)
			{ //current point is f.point3
			}
			else
			{
				//current point is not already in the vector
				_points.push_back(pX); //add current point in vector
			}
			//increments X axis point
			pX.x += Xstep.x; //increments X value
			pX.y += Xstep.y; //increments Y value
			pX.z += Xstep.z; //increments Z value
		}
		//increments Y axis point
		pY.x += Ystep.x; //increments X value
		pY.y += Ystep.y; //increments Y value
		pY.z += Ystep.z; //increments Z value
	}
}

/*
 * scalar product between 2 vectors
 */
double DOT(double* v1, double* v2)
{
	 return (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]);
}

/*
 * compute intersection between a point and a triangle
 * based on Möller-Trumbore ray/triangle intersection detection algorithm
 * return if there is an intersection (or not) and the intersection point
 */
bool GeodesicForm::getIntersection(const Ofx3DPointD point, const Triangle triangle)
{
	bool isThereIntersection = false; // returned value
	static const double epsilon = 0.0000001; //epsilon value (used to test)
	
	//compute direction vector
	double directionVector[3]; //initialize direction vector
	directionVector[0] =  _center.x - point.x; //compute X value
	directionVector[1] =  _center.y - point.y; //compute Y value
	directionVector[2] =  _center.z - point.z; //compute Z value

	//compute direction vectors of the triangle
	double directionVectorTriangle1[3]; //initialize first direction vector of the triangle
	double directionVectorTriangle2[3]; //initialize second direction vector of the triangle
	//compute values of first direction vector of the triangle
	directionVectorTriangle1[0] = triangle.point2->x - triangle.point1->x; //X value
	directionVectorTriangle1[1] = triangle.point2->y - triangle.point1->y; //Y value
	directionVectorTriangle1[2] = triangle.point2->z - triangle.point1->z; //Z value
	//compute values of second direction vector of the triangle
	directionVectorTriangle2[0] = triangle.point3->x - triangle.point1->x; //X value
	directionVectorTriangle2[1] = triangle.point3->y - triangle.point1->y; //Y value
	directionVectorTriangle2[2] = triangle.point3->z - triangle.point1->z; //Z value
		
	//Multiply directionVectorTriangle2 by directionVector
	double p[3]; //initialization
	//vector product
	p[0] = directionVector[1]*directionVectorTriangle2[2]-directionVector[2]*directionVectorTriangle2[1];	//X axis
	p[1] = directionVector[2]*directionVectorTriangle2[0]-directionVector[0]*directionVectorTriangle2[2];	//Y axis 
	p[2] = directionVector[0]*directionVectorTriangle2[1]-directionVector[1]*directionVectorTriangle2[0];	//Z axis
		
	//First check if there is an intersection
	double firstCheck = DOT(directionVectorTriangle1, p); //initialize
	
	if(fabs(firstCheck) <epsilon) //if firstCheck == 0
	{
		//there is not intersection point data is wrong
		return isThereIntersection; //return false
	}
	else
	{
		//get invert of first check
		double invertFirstCheck = 1.0/firstCheck;
		
		//try intersection between triangle and ray
		double s[3]; //initialize
		s[0] = point.x - triangle.point1->x; //X value
		s[1] = point.y - triangle.point1->y; //Y value
		s[2] = point.z - triangle.point1->z; //Z value
		
		//first test
		double u = DOT(s,p);
		u *= invertFirstCheck;
		if(u<0 || u>1.0)
		{
			//there is not an intersection point
			return isThereIntersection; //return false
		}
		
		//second try intersection between triangle and ray
		double q[3]; //initialize
		q[0] = s[1]*directionVectorTriangle1[2]-s[2]*directionVectorTriangle1[1];
		q[1] = s[2]*directionVectorTriangle1[0]-s[0]*directionVectorTriangle1[2];
		q[2] = s[0]*directionVectorTriangle1[1]-s[1]*directionVectorTriangle1[0];
				
		//second test
		double v = DOT(directionVector,q);
		v *= invertFirstCheck;
		if(v<0 || u+v>1.0)
		{
			//there is not an intersection point
			return isThereIntersection; //return false
		}
		
		//last test 
		double t=DOT(directionVectorTriangle2, q);
		t *= invertFirstCheck;
		
		//there is an intersection point
		_intersectionPoint.x = (1-u-v)*triangle.point1->x + u*triangle.point2->x + v*triangle.point3->x; //X value
		_intersectionPoint.y = (1-u-v)*triangle.point1->y + u*triangle.point2->y + v*triangle.point3->y; //Y value
		_intersectionPoint.z = (1-u-v)*triangle.point1->z + u*triangle.point2->z + v*triangle.point3->z; //Z value
		
		//change bool returned
		isThereIntersection = true; //return true		
	}
	return isThereIntersection; //return if there is an intersection
}

/*
 * Test intersection between a ray (testPoint-center) and the geodesic form
 */
void GeodesicForm::testIntersection(Ofx3DPointD testPoint)
{
	_hasIntersection = false;
	bool intersectionFound = false;
	int i=0; //initialize indice faces
	
	//research optimization
	Ofx3DPointD pointChangeReference; //testPoint is geodesicForm reference
	pointChangeReference.x = testPoint.x - _center.x; //X value in changed reference
	pointChangeReference.y = testPoint.y - _center.y; //Y value in changed reference
	pointChangeReference.z = testPoint.z - _center.z; //Z value in changed reference
	//select appropriated faces
	if(pointChangeReference.x<0 && pointChangeReference.z>=0)	   {i=0;} //current point (X<0 && Z>0) => faces 0 and 4
	else if(pointChangeReference.x>=0 && pointChangeReference.z>=0){i=1;} //current point (X>0 && Z>0) => faces 1 and 5
	else if(pointChangeReference.x>=0 && pointChangeReference.z<0) {i=2;} //current point (X>0 && Z<0) => faces 2 and 6
	else if(pointChangeReference.x<0 && pointChangeReference.z<0)  {i=3;} //current point (X<0 && Z<0) => faces 3 and 7
	
	while(i<8 && !intersectionFound)
	{
		std::size_t j=0; // initialize indice triangle
		while(j<_faces[i].triangles.size() && !intersectionFound)
		{
			//test intersection
			if(getIntersection(testPoint,_faces[i].triangles[j]))
			{
				//there is an intersection point
				intersectionFound = true;						//An intersection has been found
				_idFaceIntersection = i;							//Keep intersection face
				_intersection = _faces[i].triangles[j];			//Keep intersection triangle 
				_hasIntersection = true;						//An intersection has been found
			}
			++j; //increments triangle indice
		}
		i+=4; //increments faces indice
	}
}

/*
 * Is point in the geodesicForm
 */
bool GeodesicForm::isPointIntoGeodesicForm(Ofx3DPointD testPoint)
{
	double tolerance = 0.01; //tolerance value
	bool isPointInto = false; //initialize returned value
	//compute first vector (between testPoint and GeodesicForm center)
	double testPointVector[3]; //initialize vector
	//compute first vector
	testPointVector[0] = testPoint.x - _center.x; //X value
	testPointVector[1] = testPoint.y - _center.y; //Y value
	testPointVector[2] = testPoint.z - _center.z; //Z value
	//compute first vector norm
	double normFirstVector = testPointVector[0]*testPointVector[0]; //initialize to x*x
	normFirstVector += testPointVector[1]*testPointVector[1];		//add y*y
	normFirstVector += testPointVector[2]*testPointVector[2];		//add z*z
	normFirstVector = sqrt(normFirstVector); //compute norm
	
	//compute second vector (between intersection point and GeodesicForm center)
	double intersectionPointVector[3];
	//get intersection point
	testIntersection(testPoint); //compute intersection
	if(_hasIntersection) //if there is an intersection
	{
		//compute second vector
		intersectionPointVector[0] = _intersectionPoint.x - _center.x; //X value
		intersectionPointVector[1] = _intersectionPoint.y - _center.y; //Y value
		intersectionPointVector[2] = _intersectionPoint.z - _center.z; //Z value
		//compute second vector norm
		double normSecondVector = intersectionPointVector[0]*intersectionPointVector[0]; //initialize to x*x
		normSecondVector += intersectionPointVector[1]*intersectionPointVector[1]; //add y*y
		normSecondVector += intersectionPointVector[2]*intersectionPointVector[2]; //add z*z
		normSecondVector = sqrt(normSecondVector); //compute norm
		
		if((normFirstVector-tolerance) <= normSecondVector) //norm of test point is smaller than that with intersection point
			isPointInto = true; //current point is into the GeodesicForm
	}
	return isPointInto; //return value
}

/*
 * Extend the geodesic form with only one point
 * Assumes that testPoint is not into the geodesicForm
 */
void GeodesicForm::extendOnePoint(Ofx3DPointD testPoint, Triangle intersectionTriangle)
{
	//test intersection point 
	testIntersection(testPoint); //test intersection
	if(!_hasIntersection) //there is not intersection between ray and geodesic form
		return; //end of function
	
	//compute angle between (testPoint-center) and (triangle points -center)
	double testPointCenterVector[3]; //initialize
	testPointCenterVector[0] = testPoint.x - _center.x; //X value
	testPointCenterVector[1] = testPoint.y - _center.y; //Y value
	testPointCenterVector[2] = testPoint.z - _center.z; //Z value
	//compute norm
	double normTestPointCenterVector = 0; //initialize
	normTestPointCenterVector += testPointCenterVector[0]*testPointCenterVector[0]; //add x*x
	normTestPointCenterVector += testPointCenterVector[1]*testPointCenterVector[1]; //add y*y
	normTestPointCenterVector += testPointCenterVector[2]*testPointCenterVector[2]; //add z*z
	normTestPointCenterVector = sqrt(normTestPointCenterVector); //compute norm
	//normalize testPointCenterVector
	testPointCenterVector[0] /= normTestPointCenterVector; //normalize X value
	testPointCenterVector[1] /= normTestPointCenterVector; //normalize Y value
	testPointCenterVector[2] /= normTestPointCenterVector; //normalize Z value
	
	//compute vector (center-triangle.point1)
	double vectorCenterPoint1[3]; //initialize
	vectorCenterPoint1[0] = _intersection.point1->x - _center.x; //X value
	vectorCenterPoint1[1] = _intersection.point1->y - _center.y; //Y value
	vectorCenterPoint1[2] = _intersection.point1->z - _center.z; //Z value
	//compute norm of vector (center-triangle.point1)
	double normVectorCenterPoint1 = 0; //initialize
	normVectorCenterPoint1 += vectorCenterPoint1[0]*vectorCenterPoint1[0]; //add x*x
	normVectorCenterPoint1 += vectorCenterPoint1[1]*vectorCenterPoint1[1]; //add y*y
	normVectorCenterPoint1 += vectorCenterPoint1[2]*vectorCenterPoint1[2]; //add z*z
	normVectorCenterPoint1 = sqrt(normVectorCenterPoint1); //compute norm
	//normalize center-triangle.point1 vector
	vectorCenterPoint1[0] /= normVectorCenterPoint1; //normalize X value
	vectorCenterPoint1[1] /= normVectorCenterPoint1; //normalize Y value
	vectorCenterPoint1[2] /= normVectorCenterPoint1; //normalize Z value
	
	//compute vector (center-triangle.point2)
	double vectorCenterPoint2[3]; //initialize
	vectorCenterPoint2[0] = _intersection.point2->x - _center.x; //X value
	vectorCenterPoint2[1] = _intersection.point2->y - _center.y; //Y value
	vectorCenterPoint2[2] = _intersection.point2->z - _center.z; //Z value
	//compute norm of vector (center-triangle.point2)
	double normVectorCenterPoint2 = 0; //initialize
	normVectorCenterPoint2 += vectorCenterPoint2[0]*vectorCenterPoint2[0]; //add x*x
	normVectorCenterPoint2 += vectorCenterPoint2[1]*vectorCenterPoint2[1]; //add y*y
	normVectorCenterPoint2 += vectorCenterPoint2[2]*vectorCenterPoint2[2]; //add z*z
	normVectorCenterPoint2 = sqrt(normVectorCenterPoint2); //compute norm
	//normalize center-triangle.point2 vector
	vectorCenterPoint2[0] /= normVectorCenterPoint2; //normalize X value
	vectorCenterPoint2[1] /= normVectorCenterPoint2; //normalize Y value
	vectorCenterPoint2[2] /= normVectorCenterPoint2; //normalize Z value
	
	//compute vector (center-triangle.point3)
	double vectorCenterPoint3[3]; //initialize
	vectorCenterPoint3[0] = _intersection.point3->x - _center.x; //X value
	vectorCenterPoint3[1] = _intersection.point3->y - _center.y; //Y value
	vectorCenterPoint3[2] = _intersection.point3->z - _center.z; //Z value
	//compute norm of vector (center-triangle.point3)
	double normVectorCenterPoint3 = 0; //initialize
	normVectorCenterPoint3 += vectorCenterPoint3[0]*vectorCenterPoint3[0]; //add x*x
	normVectorCenterPoint3 += vectorCenterPoint3[1]*vectorCenterPoint3[1]; //add y*y
	normVectorCenterPoint3 += vectorCenterPoint3[2]*vectorCenterPoint3[2]; //add z*z
	normVectorCenterPoint3 = sqrt(normVectorCenterPoint3); //compute norm
	//normalize center-triangle.point3 vector
	vectorCenterPoint3[0] /= normVectorCenterPoint3; //normalize X value
	vectorCenterPoint3[1] /= normVectorCenterPoint3; //normalize Y value
	vectorCenterPoint3[2] /= normVectorCenterPoint3; //normalize Z value
	
	//compute translation triangle.point1
	double cosAngleIntersectionPoint1 = DOT(testPointCenterVector, vectorCenterPoint1); //compute cos (testPoint-triangle.point1)
	//compute new norm triangle.point1
	double hypothenusCenterPoint1 = normTestPointCenterVector/cosAngleIntersectionPoint1; //compute new norm
	if(normVectorCenterPoint1 > hypothenusCenterPoint1) //compare current norm with existing norm
	{
		//nothing to do point1 is enough far of center
	}
	else
	{
		//compute translation
		vectorCenterPoint1[0] *= hypothenusCenterPoint1; //get coordinate X
		vectorCenterPoint1[1] *= hypothenusCenterPoint1; //get coordinate Y
		vectorCenterPoint1[2] *= hypothenusCenterPoint1; //get coordinate Z
		//translate triangle.point1
		_intersection.point1->x += vectorCenterPoint1[0]; //translate point1 X
		_intersection.point1->y += vectorCenterPoint1[1]; //translate point1 Y
		_intersection.point1->z += vectorCenterPoint1[2]; //translate point2 Z
	}
	
	//compute translation triangle.point2
	double cosAngleIntersectionPoint2 = DOT(testPointCenterVector, vectorCenterPoint2); //compute cos (testPoint-triangle.point2)
	//compute new norm triangle.point2
	double hypothenusCenterPoint2 = normTestPointCenterVector/cosAngleIntersectionPoint2; //compute new norm
	if(normVectorCenterPoint2 > hypothenusCenterPoint2) //compare current norm with existing norm
	{
		//nothing to do point2 is enough far of center
	}
	else
	{
		//compute translation
		vectorCenterPoint2[0] *= hypothenusCenterPoint2; //get coordinate X
		vectorCenterPoint2[1] *= hypothenusCenterPoint2; //get coordinate Y
		vectorCenterPoint2[2] *= hypothenusCenterPoint2; //get coordinate Z
		//translate triangle.point2
		_intersection.point2->x += vectorCenterPoint2[0]; //translate point2 X
		_intersection.point2->y += vectorCenterPoint2[1]; //translate point2 Y
		_intersection.point2->z += vectorCenterPoint2[2]; //translate point2 Z
	}
	
	//compute translation triangle.point3
	double cosAngleIntersectionPoint3 = DOT(testPointCenterVector, vectorCenterPoint3); //compute cos (testPoint-triangle.point3)
	//compute new norm triangle.point3
	double hypothenusCenterPoint3 = normTestPointCenterVector/cosAngleIntersectionPoint3; //compute new norm
	if(normVectorCenterPoint3 > hypothenusCenterPoint3) //compare current norm with existing norm
	{
		//nothing to do point3 is enough far of center
	}
	else
	{
		//compute translation
		vectorCenterPoint3[0] *= hypothenusCenterPoint3; //get coordinate X
		vectorCenterPoint3[1] *= hypothenusCenterPoint3; //get coordinate Y
		vectorCenterPoint3[2] *= hypothenusCenterPoint3; //get coordinate Z
		//translate triangle.point3
		_intersection.point3->x += vectorCenterPoint3[0]; //translate point3 X
		_intersection.point3->y += vectorCenterPoint3[1]; //translate point3 Y
		_intersection.point3->z += vectorCenterPoint3[2]; //translate point3 Z
	}
}

/**
 * Transform one point of current pyramid to sphere
 * @param point current point to transform
 */
void GeodesicForm::currentPointToSphere(Ofx3DPointD& point, const double radius)
{
	//compute vector between point-center
	double vectorCenterPoint[3]; //initialize
	vectorCenterPoint[0] = point.x - _center.x; //X value
	vectorCenterPoint[1] = point.y - _center.y; //Y value
	vectorCenterPoint[2] = point.z - _center.z; //Z value
	//compute norm
	double normVectorCenterPoint = 0; //initialize
	normVectorCenterPoint += vectorCenterPoint[0]*vectorCenterPoint[0]; //add x*x
	normVectorCenterPoint += vectorCenterPoint[1]*vectorCenterPoint[1]; //add y*y
	normVectorCenterPoint += vectorCenterPoint[2]*vectorCenterPoint[2]; //add z*z
	normVectorCenterPoint = sqrt(normVectorCenterPoint); //compute norm
	//normalize vector
	vectorCenterPoint[0] /= normVectorCenterPoint; //normalize X value
	vectorCenterPoint[1] /= normVectorCenterPoint; //normalize Y value
	vectorCenterPoint[2] /= normVectorCenterPoint; //normalize Z value
	//translate point (create sphere)
	point.x = vectorCenterPoint[0]*radius + _center.x; //translate point X
	point.y = vectorCenterPoint[1]*radius + _center.y; //translate point Y
	point.z = vectorCenterPoint[2]*radius + _center.z; //translate point Z
}

}
}
}