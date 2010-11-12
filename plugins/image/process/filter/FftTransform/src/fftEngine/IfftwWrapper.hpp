/**
 * This plugin uses fftw3 which is GPL licensed, thus, this (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_IFFTWWRAPPER_HPP_
#define _TUTTLE_PLUGIN_IFFTWWRAPPER_HPP_

#include <cstdlib>

namespace tuttle {
namespace plugin {
namespace filter {
namespace fft {

	struct Complex
	{
		float x;
		float y;
	};

	class IfftwWrapper
	{
	public:
		IfftwWrapper();
		virtual ~IfftwWrapper();

		virtual bool fftIm( const float* in, float* out, size_t w, size_t h );
		virtual bool fftRe( const float* in, float* out, size_t w, size_t h );
		virtual bool fftMag( const float* in, float* out, size_t w, size_t h );
		virtual bool fft( const float* in, Complex* out, size_t w, size_t h ) = 0;
		virtual bool ifft( Complex* in, float* out, size_t w, size_t h )      = 0;
		virtual bool impfft( const float* inMod, const float* inPhase, float* out, size_t w, size_t h );
		virtual bool fftMod( const float* in, float* out, size_t w, size_t h );
		virtual bool fftPhase( const float* in, float* out, size_t w, size_t h );
		virtual bool complex2mp( const Complex* in, float* modOut, float* phaseOut, size_t w, size_t h );
		virtual bool mp2complex( const float* modIn, const float* phaseIn, Complex* out, size_t w, size_t h );
		virtual bool complex2m( const Complex* in, float* modOut, size_t w, size_t h );
		virtual bool complex2p( const Complex* in, float* phaseOut, size_t w, size_t h );
	};

}
}
}
}

#endif

