/**
 * This plugin uses fftw3 which is GPL licensed, thus, this (only) is GPL licensed.
 */
#ifdef with_cuda

#include "FftwWrapperGPU.hpp"
#include "IfftwWrapper.hpp"

#include <cufft.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <cutil_inline.h>
#include <cutil_gl_inline.h>
#include <cuda_gl_interop.h>

namespace tuttle {
namespace plugin {
namespace fft {

FftwWrapperGPU::FftwWrapperGPU()
{
	cuInit( 0 );
	cuCtxCreate( &_ictx, 0, cutGetMaxGflopsDeviceId() );
}

FftwWrapperGPU::~FftwWrapperGPU()
{
	cuCtxDestroy( _ictx );
}

/**
 * @brief
 * @param in input image
 * @param out complex number image
 * @param w image width
 * @param h image height
 */
bool FftwWrapperGPU::fft( const float* in, Complex* out, size_t w, size_t h )
{
	cufftHandle plan;
	CUdeviceptr cidata;
	CUdeviceptr codata;

	cuMemAlloc( &cidata, sizeof( cufftReal ) * w * h );
	cuMemAlloc( &codata, sizeof( cufftComplex ) * w * h );
	cuMemcpyHtoD( cidata, in, sizeof( cufftReal ) * w * h );
	/* Create a 2D FFT plan. */
	cufftPlan2d( &plan, w, h, CUFFT_R2C );
	/* Use the CUFFT plan to transform the signal out of place. */
	cufftExecR2C( plan, (cufftReal*)cidata, (cufftComplex*)codata );
	cuMemcpyDtoH( out, codata, sizeof( cufftComplex ) * w * h );
	/* Destroy the CUFFT plan. */
	cufftDestroy( plan );
	cuMemFree( cidata );
	cuMemFree( codata );
	return false;
}

bool FftwWrapperGPU::ifft( Complex* in, float* out, size_t w, size_t h )
{
	cufftHandle plan;
	CUdeviceptr idata;
	CUdeviceptr odata;

	cuMemAlloc( &idata, sizeof( cufftComplex ) * w * h );
	cuMemAlloc( &odata, sizeof( cufftReal ) * w * h );

	cuMemcpyHtoD( idata, in, sizeof( cufftComplex ) * w * h );

	// Create a 2D FFT plan.
	cufftPlan2d( &plan, w, h, CUFFT_C2R );
	// Use the CUFFT plan to transform the signal out of place.
	cufftExecC2R( plan, (cufftComplex*)idata, (cufftReal*)odata );

	cuMemcpyDtoH( out, odata, sizeof( cufftReal ) * w * h );

	// Destroy the CUFFT plan.
	cufftDestroy( plan );
	cuMemFree( idata );
	cuMemFree( odata );
	return false;
}

}
}
}

#endif

