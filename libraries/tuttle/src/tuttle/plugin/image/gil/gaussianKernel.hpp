#ifndef _TUTTLE_PLUGIN_GAUSSIANKERNEL_HPP_
#define _TUTTLE_PLUGIN_GAUSSIANKERNEL_HPP_

#include <tuttle/plugin/image/gil/globals.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include <boost/units/pow.hpp>
#include <boost/lambda/core.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>

#include <cmath>
#include <vector>
#include <algorithm>

namespace tuttle {
namespace plugin {

static const double kConvolutionEpsilon = 0.1; ///< arbitrary value...

/**
 * @brief gaussian function
 * @f[
 * G(x) = \frac{1}{\sqrt{2\pi \sigma^2}} e^{-\frac{x^2}{2 \sigma^2}}
 * @f]
 * or simplified...
 * @f[
 * G(x) = a e^{- { \frac{(x-b)^2 }{ 2 c^2} } }
 * @f]
 *
 * @return y <- gauss(x)
 */
template<typename Scalar>
Scalar gaussianValueAt( const Scalar x, const Scalar amplitude, const Scalar yscale = 1, const Scalar xcenter = 0 )
{
	namespace bu = boost::units;
	return yscale * std::exp( -bu::pow<2>( x - xcenter ) / ( 2.0 * bu::pow<2>( amplitude ) ) );
}

/**
 * @brief create the gaussian kernel
 * * found the kernel size
 * * fill kernel values
 */
template<typename Scalar>
boost::gil::kernel_1d<Scalar> buildGaussian1DKernel( const Scalar size, const bool normalize = true )
{
	using namespace boost;
	if( size == 0 )
	{
		return boost::gil::kernel_1d<Scalar>();
	}
	std::vector<Scalar> rightKernel;
	rightKernel.reserve( 10 );
	int x      = 1;
	Scalar v   = gaussianValueAt<Scalar>( x, size );
	Scalar sum = 0.0;
	do
	{
		rightKernel.push_back( v );
		sum += v;
		++x;
	}
	while( ( v = gaussianValueAt<Scalar>( x, size ) ) > kConvolutionEpsilon );

	std::vector<Scalar> kernel( rightKernel.size() * 2 + 1 );
	Scalar kernelCenter = gaussianValueAt<Scalar>( 0, size );

	sum *= 2.0;
	sum += kernelCenter;

	if( normalize )
	{
		std::for_each( rightKernel.begin(), rightKernel.end(), lambda::_1 /= sum );
		kernelCenter /= sum;
	}

	kernel[rightKernel.size()] = kernelCenter; // kernel center to 0
	std::copy( rightKernel.rbegin(), rightKernel.rend(), kernel.begin() );
	std::copy( rightKernel.begin(), rightKernel.end(), kernel.begin() + rightKernel.size() + 1 );

	//	COUT_VAR( rightKernel.size() );
	//	COUT_VAR( kernel.size() );
	//	std::cout << "[";
	//	std::for_each(rightKernel.begin(), rightKernel.end(), std::cout << lambda::_1 << ',');
	//	std::cout << "]" << std::endl;
	//	std::cout << "[";
	//	std::for_each(kernel.begin(), kernel.end(), std::cout << lambda::_1 << ',');
	//	std::cout << "]" << std::endl;
	return boost::gil::kernel_1d<Scalar>( &( kernel[0] ), kernel.size(), rightKernel.size() );
}

/**
 * @brief gaussian derivative function
 * @f[
 * G(x) = - x/a e^{- { \frac{(x-b)^2 }{ 2 c^2} } }
 * @f]
 *
 * @return y <- gauss(x)
 */
template<typename Scalar>
Scalar gaussianDerivativeValueAt( const Scalar x, const Scalar amplitude, const Scalar yscale = 1, const Scalar xcenter = 0 )
{
	namespace bu = boost::units;
	return ( x / yscale ) * std::exp( -bu::pow<2>( x - xcenter ) / ( 2.0 * bu::pow<2>( amplitude ) ) );
}

/**
 * @brief create the gaussian derivative kernel
 * * found the kernel size
 * * fill kernel values
 */
template<typename Scalar>
boost::gil::kernel_1d<Scalar> buildGaussianDerivative1DKernel( const Scalar size, const bool normalize = true )
{
	using namespace boost;
	if( size == 0 )
	{
		return boost::gil::kernel_1d<Scalar>();
	}
	std::vector<Scalar> rightKernel;
	rightKernel.reserve( 10 );
	int x = 1;
	Scalar v   = gaussianValueAt<Scalar>( x, size );
	Scalar sum = 0.0;
	do
	{
		rightKernel.push_back( v );
		sum += v;
		++x;
	}
	while( ( v = gaussianDerivativeValueAt<Scalar>( x, size ) ) > kConvolutionEpsilon );

	if( rightKernel.size() == 0 || sum == 0 )
		return boost::gil::kernel_1d<Scalar>();

	std::vector<Scalar> kernel( rightKernel.size() * 2 + 1 );
	Scalar kernelCenter = gaussianDerivativeValueAt<Scalar>( 0, size );
	sum += kernelCenter;

	if( normalize )
	{
		std::for_each( rightKernel.begin(), rightKernel.end(), lambda::_1 /= sum );
		kernelCenter /= sum;
	}

	kernel[rightKernel.size()] = kernelCenter; // kernel center to 0
	std::copy( rightKernel.rbegin(), rightKernel.rend(), kernel.begin() );
	std::copy( rightKernel.begin(), rightKernel.end(), kernel.begin() + rightKernel.size() + 1 );

	std::for_each( kernel.begin(), kernel.begin() + rightKernel.size(), lambda::_1 *= -1 );

	//	COUT_VAR( rightKernel.size() );
	//	COUT_VAR( kernel.size() );
	//	std::cout << "[";
	//	std::for_each(rightKernel.begin(), rightKernel.end(), std::cout << lambda::_1 << ',');
	//	std::cout << "]" << std::endl;
	//	std::cout << "[";
	//	std::for_each(kernel.begin(), kernel.end(), std::cout << lambda::_1 << ',');
	//	std::cout << "]" << std::endl;
	return boost::gil::kernel_1d<Scalar>( &( kernel[0] ), kernel.size(), rightKernel.size() );
}

}
}

#endif

