/**
 * This plugin uses fftw3 which is GPL licensed, thus, this (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_FFTWWRAPPERCPU_HPP_
#define _TUTTLE_PLUGIN_FFTWWRAPPERCPU_HPP_

#include "IfftwWrapper.hpp"

namespace tuttle {
namespace plugin {
namespace fft {

	class FftwWrapperCPU : public IfftwWrapper
	{
	public:
		FftwWrapperCPU();
		virtual ~FftwWrapperCPU();
		bool fft( const float* in, Complex* out, size_t w, size_t h );
		bool ifft( Complex* in, float* out, size_t w, size_t h );

	private:
	};

}
}
}

#endif

