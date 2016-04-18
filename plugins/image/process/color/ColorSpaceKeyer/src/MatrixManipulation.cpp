#include "MatrixManipulation.hpp"
#include "ofxCore.h"

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

/*
 * Matrix product with a 3D vector and a 4*4 matrix
 */
void productVectorMatrix(const Vect4& v, const Matrix4& m, Vect4& result)
{
    result[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * 1;  // compute X value
    result[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * 1;  // compute Y value
    result[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * 1; // compute Z value
    result[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * 1; // compute w
}

/*
 * Does the multiplication A=A*B : all the matrices are described column-major
 */
void multMatrixBtoMatrixA(Matrix4& A, const Matrix4& B)
{
    unsigned int i = 0; // row index
    unsigned int j = 0; // column index
    Matrix4 temp;

    for(unsigned int iValue = 0; iValue < 16; ++iValue)
    {
        temp[iValue] = 0;
        i = iValue % 4; // cause column-major
        j = iValue / 4; // cause column-major
        for(unsigned int k = 0; k < 4; ++k)
        {
            int indexik = k * 4 + i;
            int indexkj = j * 4 + k;
            temp[iValue] += A[indexik] * B[indexkj];
        }
    }
    // recopy temporary matrix into result
    A = temp;
}

/*
 *	Sets the provided matrix to identity
 */
void setToIdentity(Matrix4& matrix)
{
    Matrix4 Id = {{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    // recopy matrix
    matrix = Id;
}

/*
 * Sets the provided matrix to a rotate matrix of angle "angle", around axis "axis"
 */
void setToRotate(Matrix4& matrix, const double& angle, const Axis axis)
{
    const double c = cos(angle);
    const double s = sin(angle);
    const double x = axis[0];
    const double y = axis[1];
    const double z = axis[2];

    if((x == 1.0) && (y == 0.0) && (z == 0.0)) // current axis is X
    {
        Matrix4 R = {{1.0, 0.0, 0.0, 0.0, 0.0, c, s, 0.0, 0.0, -s, c, 0.0, 0.0, 0.0, 0.0, 1.0}};
        // recopy matrix
        matrix = R;
    }
    else
    {
        if((x == 0.0) && (y == 1.0) && (z == 0.0)) // current axis is Y
        {
            Matrix4 R = {{c, 0.0, -s, 0.0, 0.0, 1.0, 0.0, 0.0, s, 0.0, c, 0.0, 0.0, 0.0, 0.0, 1.0}};
            // recopy matrix
            matrix = R;
        }
        else
        {

            if((x == 0.0) && (y == 0.0) && (z == 1.0)) // current axis is Z
            {
                Matrix4 R = {{c, s, 0.0, 0.0, -s, c, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
                // recopy matrix
                matrix = R;
            }
            else // Rotation on non standard axis
            {
                Matrix4 R = {{(1.0 - c) * (x * x - 1.0) + 1.0, (1.0 - c) * x * y + (z * s), (1.0 - c) * x * z - (y * s), 0.0,
                              (1.0 - c) * x * y - (z * s), (1.0 - c) * (y * y - 1.0) + 1.0, (1.0 - c) * y * z + (x * s), 0.0,
                              (1.0 - c) * x * z + (y * s), (1.0 - c) * y * z - (x * s), (1.0 - c) * (z * z - 1.0) + 1.0, 0.0,
                              0.0, 0.0, 0.0, 1.0}};
                // recopy matrix
                matrix = R;
            }
        }
    }
}

/*
 *Sets the provided matrix to a translate matrix on vector t
 */
void setToTranslate(Matrix4& matrix, const Vect4& t)
{
    Matrix4 T = {{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, t[0], t[1], t[2], 1.0}};
    // recopy matrix
    matrix = T;
}

/*
 * Create rotation matrix with rotation center (translate + rotates + inverse translate)
 */
Matrix4 constructRotationMatrix(const Ofx3DPointD& rotationCenter, const double& angleX, const double& angleY,
                                const double& angleZ)
{
    // define axes
    Axis axisX = {{1, 0, 0}}; // define axe X
    Axis axisY = {{0, 1, 0}}; // define axe Y
    Axis axisZ = {{0, 0, 1}}; // define axe Z

    // Define and create rotation + translation matrix
    Matrix4 rotationTranslationMatrix;        // initialize
    setToIdentity(rotationTranslationMatrix); // set matrix center to identity

    // Define  and construct rotation matrices
    Matrix4 rotationMatrixX;                            // initialize rotation matrix X
    setToRotate(rotationMatrixX, angleX / 10.0, axisX); // construct rotation matrix X
    Matrix4 rotationMatrixY;                            // initialize rotation matrix Y
    setToRotate(rotationMatrixY, angleY / 10.0, axisY); // construct rotation matrix Y
    Matrix4 rotationMatrixZ;                            // initialize rotation matrix Z
    setToRotate(rotationMatrixZ, angleZ / 10.0, axisZ); // construct rotation matrix Z

    // Define and construct translation matrix
    Matrix4 translationMatrix; // initialize translation matrix
    // Construct translation vector
    Vect4 translationVector = {{rotationCenter.x, rotationCenter.y, rotationCenter.z, 1.0}};
    setToTranslate(translationMatrix, translationVector); // construct translation matrix

    // Define and construct inverse translation matrix
    Matrix4 translationInverseMatrix; // initialize inverse translation matrix
    // Construct inverse translation vector
    Vect4 inverseTranslationVector = {{-rotationCenter.x, -rotationCenter.y, -rotationCenter.z, 1.0}};
    setToTranslate(translationInverseMatrix, inverseTranslationVector); // construct inverse translation matrix

    // Construct final reference center matrix (inverse order of application)
    multMatrixBtoMatrixA(rotationTranslationMatrix, translationMatrix);        // translation matrix
    multMatrixBtoMatrixA(rotationTranslationMatrix, rotationMatrixZ);          // rotation Z axis
    multMatrixBtoMatrixA(rotationTranslationMatrix, rotationMatrixY);          // rotation Y axis
    multMatrixBtoMatrixA(rotationTranslationMatrix, rotationMatrixX);          // rotation X axis
    multMatrixBtoMatrixA(rotationTranslationMatrix, translationInverseMatrix); // inverse translation matrix
    // return result
    return rotationTranslationMatrix;
}
}
}
}
