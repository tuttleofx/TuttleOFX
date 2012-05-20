#ifndef _TUTTLE_PLUGIN_DPXWRITER_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_DPXWRITER_ALGORITHM_HPP_

#include <boost/gil/gil_all.hpp>

// SSSE3
#include <tmmintrin.h>

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <vector>

using namespace boost::gil;

typedef std::vector<char, tuttle::plugin::OfxAllocator<char> > DataVector;

void convertGrayToRGB()
{

}

void convertRGBToRGBA()
{

}

void convertRGBToABGR()
{

}

void convertRGBAToABGR( DataVector& dataVec, size_t width, size_t height, int pixelSize )
{
	float* dataPtrIt = (float*)&dataVec.front();
	__m128i* dataCharIt;
	__m128i data, mask;
	
	float* dataFloatIt;
	
	// define flipping data
	char charMask [16] = {0x03, 0x02, 0x01, 0x00, // pixel 1
						  0x07, 0x06, 0x05, 0x04, // pixel 2
						  0x0B, 0x0A, 0x09, 0x08, // pixel 3
						  0x0F, 0x0E, 0x0D, 0x0C  // pixel 4
						 };
	char shortMask [16] = {0x06, 0x07, 0x04, 0x05, 0x02, 0x03, 0x00, 0x01, // pixel 1
						   0x0E, 0x0F, 0x0C, 0x0D, 0x0A, 0x0B, 0x08, 0x09  // pixel 2
						  };
	
	size_t size = width * height;
	
	switch( pixelSize )
	{
		case 4 :
			dataCharIt = (__m128i*) dataPtrIt;
			// loading mask to flip with char data
			mask = _mm_loadu_si128( (__m128i*)charMask );
			
			for(size_t i=0; i< size; i+=4)
			{
				data = _mm_loadu_si128( dataCharIt );
				_mm_store_si128( dataCharIt , _mm_shuffle_epi8( data, mask ) );
				dataCharIt += 1;
			}
			break;
		case 8:
			dataCharIt = (__m128i*) dataPtrIt;
			// loading mask to flip with char data
			mask = _mm_loadu_si128( (__m128i*)shortMask );
			
			for(size_t y = 0; y < height; y ++ )
			{
				for(size_t x = 0; x < width; x += 4)
				{
					data = _mm_loadu_si128( dataCharIt );
					_mm_store_si128( dataCharIt , _mm_shuffle_epi8( data, mask ) );
					dataCharIt += 1;
				}
			}
			break;
		case 16 :
			dataFloatIt = dataPtrIt;
			
			for(char i=0; i<4; i++)
				TUTTLE_COUT( *(dataFloatIt+i) );
			TUTTLE_COUT( "" );
			
			__m128 floatData;
			// only interate on 1 dimension: 128 bits is one pixel
			for(size_t i=0; i< size; i++)
			{
				// load 4 float into floatData ( R, G, B, A )
				floatData = _mm_loadu_ps(dataPtrIt);
				// store reversed ( A, B, G, R )
				_mm_storer_ps( dataPtrIt , floatData );
				// increase pointer from 4 float
				dataPtrIt += 4;
			}
			
			for(char i=0; i<4; i++)
				TUTTLE_COUT( *(dataFloatIt+i) );
			break;
	}
	

}

void selectChannelInRGB()
{

}

void selectChannelInRGBA( void* dataSrcPtr )
{
// boost::gil::rgb32f_view_t vw = boost::gil::interleaved_view (  size.x,  size.y, Iterator pixels, std::ptrdiff_t rowsize_in_bytes);
}

#endif
