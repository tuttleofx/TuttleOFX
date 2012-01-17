#ifndef _TERRY_MATH_MATRIX_HPP_
#define	_TERRY_MATH_MATRIX_HPP_

#ifndef BOOST_UBLAS_SHALLOW_ARRAY_ADAPTOR
	#define BOOST_UBLAS_SHALLOW_ARRAY_ADAPTOR
#endif

#include <boost/numeric/ublas/storage.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <memory>


namespace terry {
namespace math {

typedef boost::numeric::ublas::row_major DefaultStorageOrder;


/*
// C++-1x solution
template<typename BaseType, class Alloc = std::allocator<BaseType> >
typedef boost::numeric::ublas::matrix<BaseType, StorageOrder, boost::numeric::ublas::unbounded_array<BaseType, Alloc > > Matrix;

template<typename T>
void f( Matrix<T> & m );

Matrixd m;
f(m);
*/

template<typename T, typename SO = DefaultStorageOrder, class Alloc = std::allocator<T> >
struct Matrix
{
	typedef T BaseType;
	typedef boost::numeric::ublas::unbounded_array<BaseType, Alloc > StorageType;
	typedef boost::numeric::ublas::matrix<BaseType, SO, StorageType> Type;

	typedef boost::numeric::ublas::shallow_array_adaptor<BaseType>        ShallowStorageType;
	typedef boost::numeric::ublas::matrix<BaseType, SO, ShallowStorageType>   ShallowType;

private:
	Matrix(){}
};

template<class T, std::size_t M, std::size_t N, typename SO = DefaultStorageOrder>
struct BoundedMatrix
{
	typedef T BaseType;
	typedef boost::numeric::ublas::bounded_matrix<BaseType, M, N, SO> Type;

private:
	BoundedMatrix(){}
};

template<typename T, class Alloc = std::allocator<T> >
struct Vector
{
	typedef T BaseType;
	typedef boost::numeric::ublas::unbounded_array<BaseType, Alloc > StorageType;
	typedef boost::numeric::ublas::vector<BaseType, StorageType> Type;

	typedef boost::numeric::ublas::shallow_array_adaptor<BaseType>        ShallowStorageType;
	typedef boost::numeric::ublas::vector<BaseType, ShallowStorageType>   ShallowType;

private:
	Vector(){}
};

template<typename T, std::size_t S>
struct BoundedVector
{
	typedef T BaseType;
	typedef boost::numeric::ublas::bounded_vector<BaseType, S> Type;
	
private:
	BoundedVector(){}
};




#define TERRY_DEFINE_MATRIX_TYPES( TYPE, ORDER, ORDERSTR, POST ) \
	\
	typedef Matrix<TYPE,ORDER>::Type Matrix##ORDERSTR##POST; \
	typedef Matrix<TYPE,ORDER>::ShallowType MatrixView##ORDERSTR##POST; \
	typedef Matrix<TYPE,ORDER>::ShallowStorageType MatrixViewInit##ORDERSTR##POST; \
	\
	typedef BoundedMatrix<TYPE, 2, 2, ORDER>::Type BoundedMatrix2x2##ORDERSTR##POST; \
	typedef BoundedMatrix<TYPE, 2, 3, ORDER>::Type BoundedMatrix2x3##ORDERSTR##POST; \
	typedef BoundedMatrix<TYPE, 2, 4, ORDER>::Type BoundedMatrix2x4##ORDERSTR##POST; \
	\
	typedef BoundedMatrix<TYPE, 3, 2, ORDER>::Type BoundedMatrix3x2##ORDERSTR##POST; \
	typedef BoundedMatrix<TYPE, 3, 3, ORDER>::Type BoundedMatrix3x3##ORDERSTR##POST; \
	typedef BoundedMatrix<TYPE, 3, 4, ORDER>::Type BoundedMatrix3x4##ORDERSTR##POST; \
	\
	typedef BoundedMatrix<TYPE, 4, 2, ORDER>::Type BoundedMatrix4x2##ORDERSTR##POST; \
	typedef BoundedMatrix<TYPE, 4, 3, ORDER>::Type BoundedMatrix4x3##ORDERSTR##POST; \
	typedef BoundedMatrix<TYPE, 4, 4, ORDER>::Type BoundedMatrix4x4##ORDERSTR##POST; \
//

#define TERRY_DEFINE_TYPES( TYPE, POST ) \
	\
	TERRY_DEFINE_MATRIX_TYPES( TYPE, boost::numeric::ublas::row_major, , POST ) \
	TERRY_DEFINE_MATRIX_TYPES( TYPE, boost::numeric::ublas::column_major, C, POST ) \
	TERRY_DEFINE_MATRIX_TYPES( TYPE, boost::numeric::ublas::row_major, R, POST ) \
	\
	typedef Vector<TYPE>::Type Vector##POST; \
	typedef Vector<TYPE>::ShallowType VectorView##POST; \
	\
	typedef boost::numeric::ublas::shallow_array_adaptor<TYPE> ArrayViewInit##POST; \
	\
	typedef BoundedVector<TYPE, 2>::Type BoundedVector2##POST; \
	typedef BoundedVector<TYPE, 3>::Type BoundedVector3##POST; \
	typedef BoundedVector<TYPE, 4>::Type BoundedVector4##POST; \
//


TERRY_DEFINE_TYPES( double, d )
TERRY_DEFINE_TYPES( float, f )
TERRY_DEFINE_TYPES( int, i )
TERRY_DEFINE_TYPES( char, c )


}
}

#endif

