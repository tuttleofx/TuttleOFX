/**
 * @brief This file provides a set of reimplementations of CImg functions by means of the generic image library (gil).
 *        Functions for image blurring
 */
#ifndef _BLUR_FILTERS_HPP_
#define _BLUR_FILTERS_HPP_

#include <tuttle/plugin/exceptions.hpp>

#include <cmath>
#include "boost/scoped_array.hpp"
#include "boost/gil/gil_all.hpp"

namespace tuttle
{
namespace imageUtils
{
using namespace tuttle::plugin;
namespace bgil = boost::gil;

/**
 * @brief The Canny-Deriche filter is a recursive algorithm allowing
 *        to compute blurred derivatives of order 0,1 or 2 of an image.
 *
 * @param[in, out]  src     Source view
 * @param[out]      dst     Destination view
 * @param[in]       sigma   Standard variation of the gaussian distribution
 * @param[in]       order   Derivative order (0 = kind of blur,
 *                                            1 = kind of blurred edge detection,
 *                                            2 = kind of very blurred edge detection)
 * @param[in]       axe     Direction of the blur
 * @param[in]       cond    Miscallenous
 * @return Return the result of the Deriche filter
 * @see blur
 */
template <typename S_VIEW, typename D_VIEW>
void deriche(const S_VIEW& src, D_VIEW& dst, const float sigma, const int order = 0, const char axe = 'x',
             const bool cond = true)
{
    int w = src.width();
    int h = src.height();

    // Concept checking
    bgil::gil_function_requires<bgil::ColorSpacesCompatibleConcept<typename bgil::color_space_type<S_VIEW>::type,
                                                                   typename bgil::color_space_type<D_VIEW>::type> >();

    if(sigma >= 0.1f || order)
    {
        const float nsigma = sigma < 0.1f ? 0.1f : sigma, alpha = 1.695f / nsigma, ema = expf(-alpha),
                    ema2 = expf(-2.0f * alpha), b1 = -2.0f * ema, b2 = ema2;
        float a0 = 0.0f, a1 = 0.0f, a2 = 0.0f, a3 = 0.0f, coefp = 0.0f, coefn = 0.0f;
        switch(order)
        {
            case 0:
            {
                const float k = (1.0f - ema) * (1.0f - ema) / (1.0f + 2.0f * alpha * ema - ema2);
                a0 = k;
                a1 = k * (alpha - 1.0f) * ema;
                a2 = k * (alpha + 1.0f) * ema;
                a3 = -k * ema2;
                break;
            }
            case 1:
            {
                const float k = (1.0f - ema) * (1.0f - ema) / ema;
                a0 = k * ema;
                a1 = a3 = 0;
                a2 = -a0;
                break;
            }
            case 2:
            {
                const float ea = expf(-alpha), k = -(ema2 - 1) / (2 * alpha * ema),
                            kn = (-2.0f * (-1.0f + 3.0f * ea - 3.0f * ea * ea + ea * ea * ea) /
                                  (3.0f * ea + 1.0f + 3.0f * ea * ea + ea * ea * ea));
                a0 = kn;
                a1 = -kn * (1.0f + k * alpha) * ema;
                a2 = kn * (1.0f - k * alpha) * ema;
                a3 = -kn * ema2;
                break;
            }
            default:
            {
                std::cerr << "Given filter order (order = " << order << ") must be 0, 1 or 2." << std::endl;
                return;
            }
        }
        typedef typename bgil::channel_type<S_VIEW>::type sPix_t;
        typedef typename bgil::channel_type<D_VIEW>::type dPix_t;
        const int nc = src.num_channels() > dst.num_channels() ? dst.num_channels() : src.num_channels();
        coefp = (a0 + a1) / (1.0f + b1 + b2);
        coefn = (a2 + a3) / (1.0f + b1 + b2);
        switch(axe)
        {
            case 'x':
            {
                typedef typename S_VIEW::x_iterator sIterator;
                typedef typename D_VIEW::x_iterator dIterator;
                const int N = w;
                boost::scoped_array<sPix_t> Y_scptr(new sPix_t[N]);
                sPix_t* Y = Y_scptr.get();
                // Recursive based algorithm
                for(int v = 0; v < nc; ++v)
                {
                    for(int y = 0; y < h; ++y)
                    {
                        sIterator iterSrcX = src.row_begin(y);
                        dIterator iterDstX = dst.row_end(y);
                        sPix_t *ptrY = Y, yb = 0, yp = 0;
                        sPix_t xp = (sPix_t)0;
                        if(cond)
                        {
                            xp = (*iterSrcX)[v];
                            yb = yp = (sPix_t)(coefp * xp);
                        }
                        for(int m = 0; m < N; ++m)
                        {
                            const sPix_t xc = (*iterSrcX)[v];
                            iterSrcX++;
                            const sPix_t yc = *(ptrY++) = (sPix_t)(a0 * xc + a1 * xp - b1 * yp - b2 * yb);
                            xp = xc;
                            yb = yp;
                            yp = yc;
                        }
                        sPix_t xn = (sPix_t)0, xa = (sPix_t)0;
                        sPix_t yn = 0, ya = 0;
                        if(cond)
                        {
                            xn = xa = (*(iterSrcX - 1))[v];
                            yn = ya = (sPix_t)coefn * xn;
                        }
                        for(int n = N - 1; n >= 0; --n)
                        {
                            const sPix_t xc = (*(--iterSrcX))[v];
                            const sPix_t yc = (sPix_t)(a2 * xn + a3 * xa - b1 * yn - b2 * ya);
                            xa = xn;
                            xn = xc;
                            ya = yn;
                            yn = yc;
                            (*--iterDstX)[v] = (dPix_t)(*(--ptrY) + yc);
                        }
                    }
                }
                break;
            }
            case 'y':
            {
                typedef typename S_VIEW::y_iterator sIterator;
                typedef typename D_VIEW::y_iterator dIterator;
                const int N = h;
                boost::scoped_array<sPix_t> Y_scptr(new sPix_t[N]);
                sPix_t* Y = Y_scptr.get();
                // Recursive based algorithm
                for(int v = 0; v < nc; ++v)
                {
                    for(int x = 0; x < w; ++x)
                    {
                        sIterator ptrX = src.col_begin(x);
                        dIterator dPtrX = dst.col_end(x);
                        sPix_t *ptrY = Y, yb = 0, yp = 0;
                        sPix_t xp = (sPix_t)0;
                        if(cond)
                        {
                            xp = (*ptrX)[v];
                            yb = yp = (sPix_t)(coefp * xp);
                        }
                        for(int m = 0; m < N; ++m)
                        {
                            const sPix_t xc = (*ptrX)[v];
                            ptrX++;
                            const sPix_t yc = *(ptrY++) = (sPix_t)(a0 * xc + a1 * xp - b1 * yp - b2 * yb);
                            xp = xc;
                            yb = yp;
                            yp = yc;
                        }
                        sPix_t xn = (sPix_t)0, xa = (sPix_t)0;
                        sPix_t yn = 0, ya = 0;
                        if(cond)
                        {
                            xn = xa = (*(ptrX - 1))[v];
                            yn = ya = (sPix_t)coefn * xn;
                        }
                        for(int n = N - 1; n >= 0; --n)
                        {
                            const sPix_t xc = (*(--ptrX))[v];
                            const sPix_t yc = (sPix_t)(a2 * xn + a3 * xa - b1 * yn - b2 * ya);
                            xa = xn;
                            xn = xc;
                            ya = yn;
                            yn = yc;
                            (*--dPtrX)[v] = (dPix_t)(*(--ptrY) + yc);
                        }
                    }
                }
                break;
            }
            default:
            {
                std::cerr << "Invalid axe value: '" << axe << "'" << std::endl;
                return;
            }
        }
    }
    else
        bgil::copy_pixels(src, dst);
}

/**
 * @brief Use deriche filter amon x and y direction to apply blur.
 *
 * @param[in, out]  src     Source view
 * @param[out]      dst     Destination view
 * @param[in]       sigma   Standard variation of the gaussian distribution
 *
 */
template <typename S_VIEW, typename D_VIEW>
void dericheFilter(const S_VIEW& src, D_VIEW& dst, const float sigma, int order = 0, float threshold = 0.0f)
{
    if(order > 0)
    {
        if(src.num_channels() < 3)
        {
            std::cerr << "Error: destination must have at least 3 channels" << std::endl;
            return;
        }
        int w = src.width();
        int h = src.height();
        int xf, c;
        const int mc = 5;
        const int mc2 = mc / 2;
        float Ix, Iy, Ixm1, Iym1, chaos, mean, q;
        // Concept checking
        bgil::gil_function_requires<bgil::ColorSpacesCompatibleConcept<typename bgil::color_space_type<S_VIEW>::type,
                                                                       typename bgil::color_space_type<D_VIEW>::type> >();
        typedef typename S_VIEW::locator Loc;
        typedef typename S_VIEW::x_iterator sxIterator;
        typedef typename D_VIEW::x_iterator dxIterator;
        typedef typename bgil::channel_type<D_VIEW>::type pix_t;

        typename terry::image_from_view<S_VIEW>::type tmpX(src.dimensions());
        typename terry::image_from_view<S_VIEW>::type tmpY(src.dimensions());

        S_VIEW vTmpX(bgil::view(tmpX));
        S_VIEW vTmpY(bgil::view(tmpY));

        deriche(src, vTmpX, sigma, order, 'x');
        deriche(src, vTmpY, sigma, order, 'y');
        if(threshold == 0.0f)
        {
            for(int y = 0; y < h; ++y)
            {
                sxIterator svx_it = vTmpX.row_begin(y);
                sxIterator svy_it = vTmpY.row_begin(y);
                dxIterator dst_it = dst.row_begin(y);
                for(int x = 0; x < w; ++x)
                {
                    Ix = ((*svx_it)[0] + (*svx_it)[1] + (*svx_it)[2]) / 3.0f;
                    Iy = ((*svy_it)[0] + (*svy_it)[1] + (*svy_it)[2]) / 3.0f;
                    (*dst_it)[0] = (pix_t)(Ix * Ix);
                    (*dst_it)[1] = (pix_t)(Ix * Iy);
                    (*dst_it)[2] = (pix_t)(Iy * Iy);
                    ++svx_it;
                    ++svy_it;
                    ++dst_it;
                }
            }
        }
        else
        {
            for(int y = 0; y < h; ++y)
            {
                sxIterator src_it = src.row_begin(y);
                sxIterator svx_it = vTmpX.row_begin(y);
                sxIterator svy_it = vTmpY.row_begin(y);
                dxIterator dst_it = dst.row_begin(y);
                chaos = Iym1 = Ixm1 = 0.0f;
                for(int x = 0; x < w; ++x)
                {
                    chaos = mean = 0.0f;
                    Loc loc = src.xy_at(x, y);
                    for(c = 0; c < 3; ++c)
                    {
                        if(x > mc2 && x < w - mc2)
                            for(xf = 0; xf < mc; ++xf)
                            {
                                mean += loc(xf - mc2, 0)[c];
                                chaos += powf(loc(xf - mc2, 0)[c], 2.0f);
                            }
                        if(y > mc2 && y < h - mc2)
                            for(xf = 0; xf < mc; ++xf)
                            {
                                mean += loc(0, xf - mc2)[c];
                                chaos += powf(loc(0, xf - mc2)[c], 2.0f);
                            }
                    }
                    mean = powf(mean / (mc * 2.0f * 3), 2.0f);
                    chaos = fabs((chaos / (mc * 2.0f * 3)) - mean);

                    q = threshold * chaos;

                    Ix = ((*svx_it)[0] + (*svx_it)[1] + (*svx_it)[2]) / 3.0f;
                    Iy = ((*svy_it)[0] + (*svy_it)[1] + (*svy_it)[2]) / 3.0f;
                    if(q > 0.0f)
                    {
                        (*dst_it)[0] = (pix_t)(floorf((Ix * Ix) / q) * q);
                        (*dst_it)[1] = (pix_t)(Ix * Iy);
                        (*dst_it)[2] = (pix_t)(floorf((Iy * Iy) / q) * q);
                    }
                    else
                    {
                        (*dst_it)[0] = (pix_t)(Ix * Ix);
                        (*dst_it)[1] = (pix_t)(Ix * Iy);
                        (*dst_it)[2] = (pix_t)(Iy * Iy);
                    }
                    Iym1 = Iy;
                    Ixm1 = Ix;
                    ++svx_it;
                    ++svy_it;
                    ++dst_it;
                    ++src_it;
                }
            }
        }
    }
    else if(sigma > 0.2f)
    {
        typename terry::image_from_view<S_VIEW>::type itmp(src.dimensions());

        S_VIEW vTmp(view(itmp));

        deriche(src, vTmp, sigma, order, 'x');
        deriche(vTmp, dst, sigma, order, 'y');
    }
    else
    {
        bgil::copy_and_convert_pixels(src, dst);
    }
}
}
}

#endif
