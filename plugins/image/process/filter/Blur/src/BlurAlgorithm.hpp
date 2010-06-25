#ifndef _TUTTLE_PLUGIN_BLURALGORITHM_HPP_
#define _TUTTLE_PLUGIN_BLURALGORITHM_HPP_

#include <tuttle/common/image/gilGlobals.hpp>
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
namespace blur {

static const double kConvolutionEpsilon = 0.05; ///< arbitrary value...


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
	return yscale * std::exp( -bu::pow<2>(x-xcenter) / (2.0*bu::pow<2>(amplitude) ) );
}

/**
 * @brief create the gaussian kernel
 * * found the kernel size
 * * fill kernel values
 */
template<typename Scalar>
boost::gil::kernel_1d<Scalar> buildGaussian1DKernel( const Scalar size )
{
	using namespace boost::lambda;
	std::vector<Scalar> rightKernel;
	rightKernel.reserve(10);
	int x = 1;
	Scalar v;
	Scalar sum = 0.0;
	while( (v = gaussianValueAt<Scalar>( x, size )) > kConvolutionEpsilon )
	{
		rightKernel.push_back( v );
		sum += v;
		++x;
	}
	if( rightKernel.size() == 0 || sum == 0 )
		return boost::gil::kernel_1d<Scalar>();
	sum *= 2.0;

	std::vector<Scalar> kernel( rightKernel.size()*2+1 );
	const Scalar kernelCenter = gaussianValueAt<Scalar>( 0, size );
	sum += kernelCenter;

	std::for_each(rightKernel.begin(), rightKernel.end(), _1 /= sum );

	kernel[rightKernel.size()] = kernelCenter / sum; // kernel center to 0
	std::copy( rightKernel.rbegin(), rightKernel.rend(), kernel.begin() );
	std::copy( rightKernel.begin(), rightKernel.end(), kernel.begin()+rightKernel.size()+1 );

//	COUT_VAR( rightKernel.size() );
//	COUT_VAR( kernel.size() );
//	std::cout << "[";
//	std::for_each(rightKernel.begin(), rightKernel.end(), std::cout << _1 << ',');
//	std::cout << "]" << std::endl;
//	std::cout << "[";
//	std::for_each(kernel.begin(), kernel.end(), std::cout << _1 << ',');
//	std::cout << "]" << std::endl;
	return boost::gil::kernel_1d<Scalar>( &(kernel[0]), kernel.size(), rightKernel.size() );
}


}
}
}

#endif
