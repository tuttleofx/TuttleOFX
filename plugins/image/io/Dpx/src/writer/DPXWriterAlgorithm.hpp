#ifndef _TUTTLE_PLUGIN_DPXWRITER_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_DPXWRITER_ALGORITHM_HPP_

#include <boost/gil/gil_all.hpp>

#ifndef __SSSE3__
#include <emmintrin.h>
#else
// SSSE3
#include <tmmintrin.h>
#endif

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <vector>

using namespace boost::gil;

typedef std::vector<char, tuttle::plugin::OfxAllocator<char> > DataVector;

void convertGrayToRGB(DataVector& dataVec, size_t width, size_t height, int pixelSize)
{
    size_t size = width * height;
    switch(pixelSize)
    {
        case 1:
        {
            char* ptrS = (char*)&dataVec.front();
            char* ptrD = (char*)&dataVec.front();
            // go to end of image
            ptrS += size - 1;
            ptrD += (3 * size) - 1;
            // process from back to begin
            for(size_t i = 0; i < size; i++)
            {
                *ptrD = *ptrS;
                ptrD--; // red
                *ptrD = *ptrS;
                ptrD--; // green
                *ptrD = *ptrS;
                ptrD--;
                ptrS--; // blue
            }
            break;
        }
        case 2:
        {
            short* ptrS = (short*)&dataVec.front();
            short* ptrD = (short*)&dataVec.front();
            // go to end of image
            ptrS += size - 1;
            ptrD += (3 * size) - 1;
            // process from back to begin
            for(size_t i = 0; i < size; i++)
            {
                *ptrD = *ptrS;
                ptrD--; // red
                *ptrD = *ptrS;
                ptrD--; // green
                *ptrD = *ptrS;
                ptrD--;
                ptrS--; // blue
            }
            break;
        }
        case 4:
        {
            float* ptrS = (float*)&dataVec.front();
            float* ptrD = (float*)&dataVec.front();
            // go to end of image
            ptrS += size - 1;
            ptrD += (3 * size) - 1;
            // process from back to begin
            for(size_t i = 0; i < size; i++)
            {
                *ptrD = *ptrS;
                ptrD--; // red
                *ptrD = *ptrS;
                ptrD--; // green
                *ptrD = *ptrS;
                ptrD--;
                ptrS--; // blue
            }
            break;
        }
    }
}

void convertRGBToRGBA()
{
}

void convertRGBToABGR()
{
}

void convertRGBAToRGB(DataVector& dataVec, size_t width, size_t height, int pixelSize)
{
    size_t size = width * height;

    switch(pixelSize)
    {
        case 4:
        {
            char* ptrS = (char*)&dataVec.front();
            char* ptrD = ptrS;
            for(size_t i = 0; i < size; i++)
            {
                *ptrD = *ptrS;
                ptrS++;
                ptrD++; // red
                *ptrD = *ptrS;
                ptrS++;
                ptrD++; // green
                *ptrD = *ptrS;
                ptrS += 2; /*skip alpha*/
                ptrD++;    // blue
            }
            break;
        }
        case 8:
        {
            short* ptrS = (short*)&dataVec.front();
            short* ptrD = ptrS;
            for(size_t i = 0; i < size; i++)
            {
                *ptrD = *ptrS;
                ptrS++;
                ptrD++; // red
                *ptrD = *ptrS;
                ptrS++;
                ptrD++; // green
                *ptrD = *ptrS;
                ptrS += 2; /*skip alpha*/
                ptrD++;    // blue
            }
            break;
        }
        case 16:
        {
            float* ptrS = (float*)&dataVec.front();
            float* ptrD = ptrS;
            for(size_t i = 0; i < size; i++)
            {
                *ptrD = *ptrS;
                ptrS++;
                ptrD++; // red
                *ptrD = *ptrS;
                ptrS++;
                ptrD++; // green
                *ptrD = *ptrS;
                ptrS += 2; /*skip alpha*/
                ptrD++;    // blue
            }
        }
    }
}

void convertRGBAToABGR(DataVector& dataVec, size_t width, size_t height, int pixelSize)
{
    float* dataPtrIt = (float*)&dataVec.front();
    __m128i* dataCharIt = NULL;
    __m128i data, mask;

    char* charPtr = NULL;
    // define flipping data
    char charMask[16] = {
        0x03, 0x02, 0x01,
        0x00, // pixel 1
        0x07, 0x06, 0x05,
        0x04, // pixel 2
        0x0B, 0x0A, 0x09,
        0x08, // pixel 3
        0x0F, 0x0E, 0x0D,
        0x0C // pixel 4
    };
    char shortMask[16] = {
        0x06, 0x07, 0x04, 0x05, 0x02,
        0x03, 0x00, 0x01, // pixel 1
        0x0E, 0x0F, 0x0C, 0x0D, 0x0A,
        0x0B, 0x08, 0x09 // pixel 2
    };

    size_t size = width * height;

    size_t i = 0;

    switch(pixelSize)
    {
        case 4:
        {
            // loading mask to flip with char data
            mask = _mm_loadu_si128((__m128i*)charMask);
#ifndef __SSSE3__
            __m128i maskA = _mm_set_epi32(0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff);
            __m128i maskB = _mm_sll_epi32(maskA, _mm_set_epi32(8, 8, 8, 8));
            __m128i maskC = _mm_sll_epi32(maskB, _mm_set_epi32(8, 8, 8, 8));
            __m128i maskD = _mm_sll_epi32(maskC, _mm_set_epi32(8, 8, 8, 8));
#endif
            for(; i < size - 3; i += 4)
            {
                dataCharIt = (__m128i*)dataPtrIt;
                data = _mm_loadu_si128(dataCharIt);
#ifdef __SSSE3__
                data = _mm_shuffle_epi8(data, mask);
#else

                __m128i result = _mm_srl_epi32(_mm_and_si128(data, maskD), _mm_set_epi32(24, 24, 24, 24));
                result = _mm_or_si128(_mm_srl_epi32(_mm_and_si128(data, maskC), _mm_set_epi32(8, 8, 8, 8)), result);
                result = _mm_or_si128(_mm_sll_epi32(_mm_and_si128(data, maskB), _mm_set_epi32(8, 8, 8, 8)), result);
                result = _mm_or_si128(_mm_sll_epi32(_mm_and_si128(data, maskA), _mm_set_epi32(24, 24, 24, 24)), result);

                data = result;
#endif
                _mm_store_si128(dataCharIt, data);
                dataPtrIt += 4;
            }
            charPtr = (char*)dataPtrIt;
            for(size_t lastPixel = 0; lastPixel < size - i; i++)
            {
                char pixel[4];
                for(size_t c = 0; c < 4; c++)
                {
                    pixel[c] = *charPtr;
                    charPtr++;
                }
                charPtr -= 4;
                for(int c = 3; c > -1; c--)
                {
                    *charPtr = pixel[c];
                    charPtr++;
                }
            }
            break;
        }
        case 8:
        {
            // loading mask to flip with short data
            mask = _mm_loadu_si128((__m128i*)shortMask);
#ifndef __SSSE3__
            __m128i maskA = _mm_set_epi32(0x00000000, 0x0000ffff, 0x00000000, 0x0000ffff);
            __m128i maskB = _mm_sll_epi32(maskA, _mm_set_epi32(16, 16, 16, 16));
            __m128i maskC = _mm_set_epi32(0x0000ffff, 0x00000000, 0x0000ffff, 0x00000000);
            __m128i maskD = _mm_sll_epi32(maskC, _mm_set_epi32(16, 16, 16, 16));
#endif
            for(; i < size - 1; i += 2)
            {
                dataCharIt = (__m128i*)dataPtrIt;
                data = _mm_loadu_si128(dataCharIt);
#ifdef __SSSE3__
                data = _mm_shuffle_epi8(data, mask);
#else
                __m128i result = _mm_srl_epi32(_mm_and_si128(data, maskD), _mm_set_epi32(48, 48, 48, 48));
                result = _mm_or_si128(_mm_srl_epi32(_mm_and_si128(data, maskC), _mm_set_epi32(16, 16, 16, 16)), result);
                result = _mm_or_si128(_mm_sll_epi32(_mm_and_si128(data, maskB), _mm_set_epi32(16, 16, 16, 16)), result);
                result = _mm_or_si128(_mm_sll_epi32(_mm_and_si128(data, maskA), _mm_set_epi32(48, 48, 48, 48)), result);

                data = result;
#endif
                _mm_store_si128(dataCharIt, data);
                dataPtrIt += 4;
            }
            if(i != size)
            {
                short* shortPtr = (short*)dataPtrIt;
                short pixel[4];
                for(size_t c = 0; c < 4; c++)
                {
                    pixel[c] = *shortPtr;
                    shortPtr++;
                }
                shortPtr -= 4;
                for(int c = 3; c > -1; c--)
                {
                    *shortPtr = pixel[c];
                    shortPtr++;
                }
            }
            break;
        }
        case 16:
        {
            float* dataFloatIt;
            dataFloatIt = dataPtrIt;

            /*for(char c=0; c<4; c++)
                    TUTTLE_LOG_INFO( *(dataFloatIt+c) );
            TUTTLE_LOG_INFO( "" );*/

            __m128 floatData;
            // only interate on 1 dimension: 128 bits is one pixel
            for(; i < size; i++)
            {
                // load 4 float into floatData ( R, G, B, A )
                floatData = _mm_loadu_ps(dataPtrIt);
                // store reversed ( A, B, G, R )
                _mm_storer_ps(dataPtrIt, floatData);
                // increase pointer from 4 float
                dataPtrIt += 4;
            }

            /*for(char i=0; i<4; i++)
                    TUTTLE_LOG_INFO( *(dataFloatIt+i) );*/
            break;
        }
    }
}

void selectChannelInRGB()
{
}

void selectChannelInRGBA(void* dataSrcPtr)
{
    // boost::gil::rgb32f_view_t vw = boost::gil::interleaved_view (  size.x,  size.y, Iterator pixels, std::ptrdiff_t
    // rowsize_in_bytes);
}

#endif
