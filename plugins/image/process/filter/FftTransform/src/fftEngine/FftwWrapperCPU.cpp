/**
 * This plugin uses fftw3 which is GPL licensed, thus, this (only) is GPL licensed.
 */

#include "FftwWrapperCPU.hpp"
#include <fftw3.h>

namespace tuttle {
namespace plugin {
namespace fft {

FftwWrapperCPU::FftwWrapperCPU()
{}

FftwWrapperCPU::~FftwWrapperCPU()
{}

/**
 * @brief
 * @param in input image
 * @param out complex number image
 * @param w image width
 * @param h image height
 */
bool FftwWrapperCPU::fft( const float* in, Complex* out, size_t w, size_t h )
{
	fftw_complex* spatial_repr;
	fftw_complex* frequency_repr;
	size_t i;
	size_t j;
	fftw_plan plan;
	size_t x, y;

	spatial_repr   = (fftw_complex*)fftw_malloc( sizeof( fftw_complex ) * w * h );
	frequency_repr = (fftw_complex*)fftw_malloc( sizeof( fftw_complex ) * w * h );

	if( !out )
	{
		return true;
	}
	else
	{

		for( i = 0; i < w * h; ++i )
		{
			spatial_repr[i][0] = in[i];
			spatial_repr[i][1] =  0.0f;
		}

		// Compute execution plan
		plan = fftw_plan_dft_2d( h, w, spatial_repr, frequency_repr, FFTW_FORWARD, FFTW_ESTIMATE );

		// Compute FFT
		fftw_execute( plan );

		for( j = 0; j < h; ++j )
		{
			for( i = 0; i < w; ++i )
			{
				// Change centering
				x = i;
				y = j;
				if( i < w / 2 && j < h / 2 )
				{
					x = i + w / 2; y = j + h / 2;
				}
				if( i >= w / 2 && j < h / 2 )
				{
					x = i - w / 2; y = j + h / 2;
				}
				if( i < w / 2 && j >= h / 2 )
				{
					x = i + w / 2; y = j - h / 2;
				}
				if( i >= w / 2 && j >= h / 2 )
				{
					x = i - w / 2; y = j - h / 2;
				}
				out[y * w + x].x = frequency_repr[j * w + i][0];
				out[y * w + x].y = frequency_repr[j * w + i][1];
			}
		}

		fftw_destroy_plan( plan );
		fftw_free( spatial_repr );
		fftw_free( frequency_repr );
		return false;
	}
}

bool FftwWrapperCPU::ifft( Complex* in, float* out, size_t w, size_t h )
{
	fftw_complex* spatial_repr;
	fftw_complex* frequency_repr;
	unsigned int i;
	unsigned int j;
	int x, y;
	fftw_plan plan;
	size_t wh = w * h;

	if( !out )
	{
		return true;
	}
	else
	{
		spatial_repr   = (fftw_complex*)fftw_malloc( sizeof( fftw_complex ) * wh );
		frequency_repr = (fftw_complex*)fftw_malloc( sizeof( fftw_complex ) * wh );
		for( j = 0; j < h; j++ )
		{
			for( i = 0; i < w; i++ )
			{
				// Recenter
				x = i;
				y = j;
				if( i < w / 2 && j < h / 2 )
				{
					x = i + w / 2; y = j + h / 2;
				}
				if( i >= w / 2 && j < h / 2 )
				{
					x = i - w / 2; y = j + h / 2;
				}
				if( i < w / 2 && j >= h / 2 )
				{
					x = i + w / 2; y = j - h / 2;
				}
				if( i >= w / 2 && j >= h / 2 )
				{
					x = i - w / 2; y = j - h / 2;
				}
				frequency_repr[j * w + i][0] = in[y * w + x].x;
				frequency_repr[j * w + i][1] = in[y * w + x].y;
			}
		}

		plan = fftw_plan_dft_2d( h, w, frequency_repr, spatial_repr, FFTW_BACKWARD, FFTW_ESTIMATE );

		fftw_execute( plan );

		for( i = 0; i < wh; ++i )
		{
			out[i] = spatial_repr[i][0];
		}

		fftw_destroy_plan( plan );
		fftw_free( spatial_repr );
		fftw_free( frequency_repr );
		return false;
	}
}

}
}
}


