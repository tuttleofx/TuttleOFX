/**
 * @brief This file provides a set of reimplementations of CImg functions by means of the generic image library (gil).
 *        Functions for interpolation
 */
#ifndef _TUTTLE_ANISOTROPICDIFFUSION_INTERPOLATION_HPP_
#define _TUTTLE_ANISOTROPICDIFFUSION_INTERPOLATION_HPP_

#include <boost/gil/gil_all.hpp>

#include <cmath>

namespace tuttle
{
namespace imageUtils
{

/**
 * @brief Return right pixel from unsafe coordinates
 *
 * @param[in]       W   input image (linear buffer, cache optimisation)
 * @param[in]       x   abscisse
 * @param[in]       y   ordinate
 * @param[in]       c   channel index
 * @param[in]       w   image width
 * @param[in]       h   image height
 * @param[in]       nc  channels number
 * @return          right pixel
 */
inline const float& pix2d(const float* W, const int x, const int y, const int c, const int w, const int h, const int nc)
{
    return W[((y < 0 ? 0 : (y >= h ? h - 1 : y)) * w + (x < 0 ? 0 : (x >= w ? w - 1 : x))) * nc + c];
}

/**
 * @brief Return right pixel from unsafe coordinates
 *
 * @param[in]       myView   input view
 * @param[in]       x   abscisse
 * @param[in]       y   ordinate
 * @param[in]       c   channel index
 * @param[in]       w   image width
 * @param[in]       h   image height
 * @param[in]       nc  channels number
 * @return          right pixel
 */
template <typename DstView>
const float pix2d(DstView& myView, const int x, const int y, const int c, const int w, const int h)
{
    return myView((x < 0 ? 0 : (x >= w ? w - 1 : x)), (y < 0 ? 0 : (y >= h ? h - 1 : y)))[c];
}

/**
 * @brief Return interpolated pixel
 *
 * @param[in]       W   input image (linear buffer, cache optimisation)
 * @param[in]       fx  float abscisse
 * @param[in]       fy  float ordinate
 * @param[in]       c   channel index
 * @param[in]       w   image width
 * @param[in]       h   image height
 * @param[in]       nc  channels number
 * @return          right pixel
 */
inline float linear_pix2d(const float* W, const float fx, const float fy, const int c, const int w, const int h,
                          const int nc)
{
    const int x = (int)fx - (fx >= 0 ? 0 : 1), nx = x + 1, y = (int)fy - (fy >= 0 ? 0 : 1), ny = y + 1;
    const float dx = fx - x, dy = fy - y;
    const float Icc = pix2d(W, x, y, c, w, h, nc), Inc = pix2d(W, nx, y, c, w, h, nc), Icn = pix2d(W, x, ny, c, w, h, nc),
                Inn = pix2d(W, nx, ny, c, w, h, nc);
    return Icc + dx * (Inc - Icc + dy * (Icc + Inn - Icn - Inc)) + dy * (Icn - Icc);
}

/**
 * @brief Return interpolated pixel
 *
 * @param[in]       myView   input view
 * @param[in]       fx  float abscisse
 * @param[in]       fy  float ordinate
 * @param[in]       c   channel index
 * @param[in]       w   image width
 * @param[in]       h   image height
 * @param[in]       nc  channels number
 * @return          right pixel
 */
template <typename DstView>
float linear_pix2d(DstView& myView, const float fx, const float fy, const int c, const int w, const int h)
{
    const int x = (int)fx - (fx >= 0 ? 0 : 1), nx = x + 1, y = (int)fy - (fy >= 0 ? 0 : 1), ny = y + 1;
    const float dx = fx - x, dy = fy - y;
    const float Icc = pix2d(myView, x, y, c, w, h), Inc = pix2d(myView, nx, y, c, w, h), Icn = pix2d(myView, x, ny, c, w, h),
                Inn = pix2d(myView, nx, ny, c, w, h);
    return Icc + dx * (Inc - Icc + dy * (Icc + Inn - Icn - Inc)) + dy * (Icn - Icc);
}
}
}

#endif
