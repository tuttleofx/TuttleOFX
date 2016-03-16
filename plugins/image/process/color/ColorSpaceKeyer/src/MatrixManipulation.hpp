#ifndef MATRIXMANIPULATION_HPP
#define MATRIXMANIPULATION_HPP

#include <ofxCore.h>
#include <math.h>
#include <boost/array.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

// declare boost::array as Types
typedef boost::array<double, 16> Matrix4; // 4*4 matrix
typedef boost::array<double, 4> Vect4; // 4 vector
typedef boost::array<double, 3> Axis; // axis X = (1,0,0)    Y = (0,1,0)   Z = (0,0,1) or others

// Matrix product with a 3D vector and a 4*4 matrix
void productVectorMatrix(const Vect4& v, const Matrix4& m, Vect4& result);

// Does the multiplication A=A*B : all the matrices are 4*4 and described column-major
void multMatrixBtoMatrixA(Matrix4& A, const Matrix4& B);

// Sets the provided matrix to identity
void setToIdentity(Matrix4& matrix);

// Sets the provided matrix to a rotate matrix of angle "angle", around axis "axis"
void setToRotate(Matrix4& matrix, const double& angle, const Axis axis);

// Sets the provided matrix to a translate matrix on vector t
void setToTranslate(Matrix4& matrix, const Vect4& t);

// Create rotation matrix with rotation center (translate + rotate + inverse translate)
Matrix4 constructRotationMatrix(const Ofx3DPointD& rotationCenter, const double& angleX, const double& angleY,
                                const double& angleZ);
}
}
}
#endif
