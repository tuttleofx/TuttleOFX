/**
 * @brief This file provides a set of reimplementations of CImg functions by means of the generic image library (gil).
 *        Functions for noise analysis
 */
#ifndef _NOISE_ANALYSIS_HPP_
#define _NOISE_ANALYSIS_HPP_

#include <cmath>
#include "boost/gil/gil_all.hpp"

namespace tuttle
{
namespace imageUtils
{

namespace bgil = boost::gil;

/**
 * @brief Compute somme pseudo-residuals
 * The pseudo residual r_i of the image Y_i are so thar E[r_i^2] = E[Y_i^2].
 * This is the 2D pseudo-implementation.
 */
template <class S_VIEW, class D_VIEW>
D_VIEW& pseudo_residual(S_VIEW& src, D_VIEW& dst)
{
    using namespace boost::gil;
    typedef typename S_VIEW::locator Loc;
    typedef typename D_VIEW::x_iterator dxIterator;
    typedef typename channel_type<D_VIEW>::type pix_t;

    int w = src.width();
    int h = src.height();
    int nc = src.num_channels();
    int x, y, c;
    double t2;

    gil_function_requires<
        ColorSpacesCompatibleConcept<typename color_space_type<S_VIEW>::type, typename color_space_type<D_VIEW>::type> >();

    for(y = 0; y < h; ++y)
    {
        dxIterator dst_it = dst.row_begin(y);

        for(x = 0; x < w; ++x)
        {
            Loc loc = src.xy_at(x, y);
            for(c = 0; c < nc; ++c)
            {
                t2 = 0;
                if(x == 0)
                    t2 += loc(1, 0)[c];
                else
                    t2 += loc(-1, 0)[c];
                if(x == w - 1)
                    t2 += loc(-1, 0)[c];
                else
                    t2 += loc(1, 0)[c];
                if(y == 0)
                    t2 += loc(0, 1)[c];
                else
                    t2 += loc(0, -1)[c];
                if(y == h - 1)
                    t2 += loc(0, -1)[c];
                else
                    t2 += loc(0, 1)[c];
                (*dst_it)[c] = (pix_t)(0.223606798 * (4.0 * (double)loc(0, 0)[c] - t2));
            }
            dst_it++;
        }
    }
    return dst;
}

/**
 * @brief Return the variance and the mean of the image
 * Least Mean of Square
 */
template <class S_VIEW, typename t>
double variance_mean(S_VIEW& src, t& mean)
{
    typedef typename S_VIEW::x_iterator sxIterator;
    int w = src.width();
    int h = src.height();
    int nc = src.num_channels();
    int x, y, c;
    double variance = 0, average = 0;
    const unsigned int siz = w * h * nc;
    double S = 0, S2 = 0;

    for(y = 0; y < h; ++y)
    {
        sxIterator src_it = src.row_begin(y);
        for(x = 0; x < w; ++x)
        {
            for(c = 0; c < nc; c++)
            {
                const double val = (*src_it)[c];
                S += val;
                S2 += val * val;
            }
            ++src_it;
        }
    }
    variance = (S2 - S * S / siz) / siz;
    average = S;
    mean = (t)(average / siz);
    return variance;
}

/**
 * @brief Robustly estimatate the variance of a the noise using the pseudo-residuals.
 * @see variance_estimation()
 */
template <class S_VIEW>
double noise_variance(S_VIEW& src)
{
    double mean;
    typename image_from_view<S_VIEW>::type tmp(src.dimensions());
    S_VIEW v_tmp(view(tmp));
    return variance_mean(pseudo_residual(src, v_tmp), mean);
}
}
}

#endif
