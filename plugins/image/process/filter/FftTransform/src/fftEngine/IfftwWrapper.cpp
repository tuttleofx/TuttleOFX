/**
 * This plugin uses fftw3 which is GPL licensed, thus, this (only) is GPL licensed.
 */

#include "IfftwWrapper.hpp"

#include <cstdlib>
#include <vector>
#include <cmath>

namespace tuttle {
namespace plugin {
namespace fft {

IfftwWrapper::IfftwWrapper()
{}

IfftwWrapper::~IfftwWrapper()
{}

/**
 * @brief compute fft real part
 * @param in input image
 * @param out real part image
 * @param w image width
 * @param h image height
 */
bool IfftwWrapper::fftRe( const float* in, float* out, size_t w, size_t h )
{
	size_t wh = w * h;

	std::vector<Complex> outc( wh );
	bool ret = fft( in, &outc[0], w, h );
	if( !ret )
	{
		for( size_t i = 0; i < wh; ++i )
		{
			out[i] = outc[i].x;
		}
	}
	return ret;
}

/**
 * @brief compute fft imaginary part
 * @param in input image
 * @param out imaginary part image
 * @param w image width
 * @param h image height
 */
bool IfftwWrapper::fftIm( const float* in, float* out, size_t w, size_t h )
{
	size_t wh = w * h;

	std::vector<Complex> outc( wh );
	bool ret = fft( in, &outc[0], w, h );
	if( !ret )
	{
		for( size_t i = 0; i < wh; ++i )
		{
			out[i] = outc[i].y;
		}
	}
	return ret;
}

/**
 * @brief Returns human interpretable fft
 * @param in input image
 * @param out real part image
 * @param w image width
 * @param h image height
 */
bool IfftwWrapper::fftMag( const float* in, float* out, size_t w, size_t h )
{
	float magmax = 0.0f;
	size_t wh    = w * h;

	std::vector<Complex> outc( w * h );
	fft( in, &outc[0], w, h );
	for( size_t i = 0; i < wh; ++i )
	{
		// Get magnitude
		out[i] = sqrt( outc[i].x * outc[i].x + outc[i].y * outc[i].y );
		if( magmax < out[i] )
			magmax = out[i];
	}
	// Take 10% of the maximum, so the pixels amplitude is better shown
	magmax *= 0.1;
	for( size_t i = 0; i < wh; ++i )
	{
		// Normalize to dB
		out[i] = 10.0f * std::log( 10.0f ) * ( out[i] / magmax );
	}
	return true;
}

/**
 * @brief compute fft real part
 * @param in input image
 * @param out real part image
 * @param w image width
 * @param h image height
 */
bool IfftwWrapper::fftMod( const float* in, float* out, size_t w, size_t h )
{
	size_t wh = w * h;

	std::vector<Complex> outc( wh );
	bool ret = fft( in, &outc[0], w, h );
	if( !ret )
	{
		ret = complex2m( &outc[0], out, w, h );
	}
	return ret;
}

/**
 * @brief compute fft real part
 * @param in input image
 * @param out real part image
 * @param w image width
 * @param h image height
 */
bool IfftwWrapper::fftPhase( const float* in, float* out, size_t w, size_t h )
{
	size_t wh = w * h;

	std::vector<Complex> outc( wh );
	bool ret = fft( in, &outc[0], w, h );
	if( !ret )
	{
		ret = complex2p( &outc[0], out, w, h );
	}
	return ret;
}

bool IfftwWrapper::impfft( const float* inMod, const float* inPhase, float* out, size_t w, size_t h )
{
	size_t wh = w * h;

	std::vector<Complex> complex( wh );
	mp2complex( inMod, inPhase, &complex[0], w, h );
	return ifft( &complex[0], out, w, h );
}

bool IfftwWrapper::complex2mp( const Complex* in, float* modOut, float* phaseOut, size_t w, size_t h )
{
	size_t i;
	size_t wh = w * h;

	for( i = 0; i < wh; ++i )
	{
		modOut[i] = sqrt( in[i].x * in[i].x + in[i].y * in[i].y );
		if( in[i].x != 0.0f )
		{
			phaseOut[i] = atan2( in[i].y, in[i].x );
		}
	}
	return false;
}

bool IfftwWrapper::mp2complex( const float* modIn, const float* phaseIn, Complex* out, size_t w, size_t h )
{
	size_t i;
	size_t wh = w * h;

	for( i = 0; i < wh; ++i )
	{
		out[i].x = modIn[i] * cos( phaseIn[i] );
		out[i].y = modIn[i] * sin( phaseIn[i] );
	}
	return false;
}

bool IfftwWrapper::complex2m( const Complex* in, float* modOut, size_t w, size_t h )
{
	size_t i;
	size_t wh = w * h;

	for( i = 0; i < wh; ++i )
	{
		modOut[i] = sqrt( ( in[i].x * in[i].x ) + ( in[i].y * in[i].y ) );
	}
	return false;
}

bool IfftwWrapper::complex2p( const Complex* in, float* phaseOut, size_t w, size_t h )
{
	size_t i;
	size_t wh = w * h;

	for( i = 0; i < wh; ++i )
	{
		if( in[i].x != 0.0f )
		{
			phaseOut[i] = atan2( in[i].y, in[i].x );
		}
	}
	return false;
}

}
}
}


