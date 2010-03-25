/**
 * This plugin uses fftw3 which is GPL licensed, thus, this (only) is GPL licensed.
 */

#ifndef _FFTWWRAPPERGPU_HPP
#define	_FFTWWRAPPERGPU_HPP

#include "IfftwWrapper.hpp"

#include <cstdlib>
#include <vector>
#include <cuda/cuda.h>

namespace tuttle {
namespace filter {
namespace fft {

inline CUdeviceptr voidToDev( void* v )
{
	unsigned long u = (unsigned long)v;

	return (CUdeviceptr)( u & 0xffffffff );
}

inline const CUdeviceptr voidToConstDev( const void* v )
{
	const unsigned long u = (const unsigned long)v;

	return (const CUdeviceptr)( u & 0xffffffff );
}

class FftwWrapperGPU : public IfftwWrapper
{
public:
	FftwWrapperGPU();
	virtual ~FftwWrapperGPU();
	bool fft(const float* in, Complex* out, size_t w, size_t h);
	bool ifft(Complex* in, float* out, size_t w, size_t h);
private:
	CUdevice _idev;
	CUcontext _ictx;
};

}
}
}

#endif	/* _FFTWWRAPPERGPU_HPP */

