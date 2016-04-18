/**
 * \brief This file provides a set of reimplementations of CImg functions by means of the generic image library (gil).
 *        Functions for edge detection
 */
#ifndef _EDGE_DETECT_HPP_
#define _EDGE_DETECT_HPP_

#include <cmath>
#include <vector>

#include <boost/gil/gil_all.hpp>
#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle
{
namespace imageUtils
{

using namespace tuttle::plugin;
namespace bgil = boost::gil;

/**
 * @brief Apply precise forward/backward finite differences to get a gradient image
 *
 * @param[in]       src         Source view
 * @param[out]      final_dst   Destination view
 * @return outline image (destination)
 * @warning Destination MUST BE different from source.
 */
template <typename S_VIEW, typename D_VIEW>
D_VIEW& simple_structure_tensor(const S_VIEW& src, D_VIEW& final_dst, float threshold)
{
    if(final_dst.width() < 3)
    {
        std::cerr << "Error: destination width must be >= 3" << std::endl;
        return final_dst;
    }

    // Concept checking
    bgil::gil_function_requires<bgil::ColorSpacesCompatibleConcept<typename bgil::color_space_type<S_VIEW>::type,
                                                                   typename bgil::color_space_type<D_VIEW>::type> >();

    typedef typename S_VIEW::x_iterator sIterator;
    typedef typename D_VIEW::x_iterator dIterator;
    typedef typename S_VIEW::locator locator;
    typedef typename bgil::channel_type<S_VIEW>::type pix_t;
    typedef typename bgil::channel_type<D_VIEW>::type dpix_t;

    int w = src.width() < final_dst.width() ? src.width() : final_dst.width();
    int h = src.height() < final_dst.height() ? src.height() : final_dst.height();
    int nc = final_dst.num_channels() < src.num_channels() ? final_dst.num_channels() : src.num_channels();
    float r, g, b;
    float Icc, Inc, Icn, Ipc, Icp;
    float ixf, ixb, iyf, iyb;
    int ny, nym1, nx, nxm1;
    int x, y, c, xf;
    const int mc = 5;
    const int mc2 = mc / 2;
    float q, chaos, mean;

    for(y = 0; y < h; y++)
    {
        dIterator dst_it = final_dst.row_begin(y);

        nym1 = y - 1 >= 0 ? -1 : 0;
        ny = y + 1 < h ? 1 : 0;
        for(x = 0; x < w; x++)
        {
            nxm1 = x - 1 >= 0 ? -1 : 0;
            nx = x + 1 < w ? 1 : 0;
            locator loc = src.xy_at(x, y);
            mean = chaos = r = g = b = 0.0f;
            for(c = 0; c < nc; c++)
            {
                // Get neighbors pixels
                Icc = loc(0, 0)[c];
                Inc = loc(nx, 0)[c];
                Ipc = loc(nxm1, 0)[c];
                Icn = loc(0, ny)[c];
                Icp = loc(0, nym1)[c];
                ixf = Inc - Icc;
                ixb = Icc - Ipc;
                iyf = Icn - Icc;
                iyb = Icc - Icp;

                // Compute rgb tensors value
                r += 0.5f * (ixf * ixf + ixb * ixb);
                g += 0.25f * (ixf * iyf + ixf * iyb + ixb * iyf + ixb * iyb);
                b += 0.5f * (iyf * iyf + iyb * iyb);
            }
            for(c = 0; c < 3; c++)
            {
                if(x > mc2 && x < w - mc2)
                    for(xf = 0; xf < mc; xf++)
                    {
                        mean += loc(xf - mc2, 0)[c];
                        chaos += powf(loc(xf - mc2, 0)[c], 2.0f);
                    }
                if(y > mc2 && y < h - mc2)
                    for(xf = 0; xf < mc; xf++)
                    {
                        mean += loc(0, xf - mc2)[c];
                        chaos += powf(loc(0, xf - mc2)[c], 2.0f);
                    }
            }
            mean = powf(mean / (mc * 2.0f * 3), 2.0f);
            chaos = fabs((chaos / (mc * 2.0f * 3)) - mean);
            q = threshold * chaos;
            if(q > 0.0f)
            {
                // Threshold by q
                (*dst_it)[0] = (dpix_t)(floorf(r / q) * q);
                (*dst_it)[1] = (dpix_t)(g);
                (*dst_it)[2] = (dpix_t)(floorf(b / q) * q);
            }
            else
            {
                (*dst_it)[0] = (dpix_t)(r);
                (*dst_it)[1] = (dpix_t)(g);
                (*dst_it)[2] = (dpix_t)(b);
            }
            dst_it++;
        }
    }
    return final_dst;
}

template <typename S_VIEW, typename D_VIEW>
void harris(const S_VIEW& src, D_VIEW& dst, float threshold)
{
    const int mc = 5;
    const int mc2 = mc / 2;
    float xMatrix[mc] = {-0.5f, -0.5f, 0, 0.5f, 0.5f};

    int x, y, c, xf;
    const int w = dst.width(), h = dst.height();
    const int nc = dst.num_channels() < src.num_channels() ? dst.num_channels() : src.num_channels();
    // Concept checking
    bgil::gil_function_requires<bgil::ColorSpacesCompatibleConcept<typename bgil::color_space_type<S_VIEW>::type,
                                                                   typename bgil::color_space_type<D_VIEW>::type> >();

    typedef typename D_VIEW::x_iterator dIterator;
    typedef typename S_VIEW::locator locator;

    typedef typename bgil::channel_type<D_VIEW>::type pix_t;
    float chaos, mean, q, px, py, Ix[3], Iy[3];

    for(y = mc2; y < h - mc2; y++)
    {
        dIterator dst_it = dst.row_begin(y);
        dst_it += mc2;
        for(x = mc2; x < w - mc2; x++)
        {
            chaos = mean = 0.0f;
            memset(Ix, 0, 3 * sizeof(float));
            memset(Iy, 0, 3 * sizeof(float));
            locator sLoc = src.xy_at(x, y);
            for(c = 0; c < 3; c++)
            {
                for(xf = 0; xf < mc; xf++)
                {
                    px = sLoc(xf - mc2, 0)[c];
                    Ix[c] += px * xMatrix[xf];
                    mean += px;
                    chaos += powf(sLoc(xf - mc2, 0)[c], 2.0f);
                }
                for(xf = 0; xf < mc; xf++)
                {
                    py = sLoc(0, xf - mc2)[c];
                    Iy[c] += py * xMatrix[xf];
                    mean += py;
                    chaos += powf(sLoc(0, xf - mc2)[c], 2.0f);
                }
            }
            // Compute variance
            mean /= 2.0f * mc * 3.0f;
            mean = powf(mean, 2.0f);
            // Chaos is the variance
            chaos /= (mc * 2.0f) * 3.0f;
            chaos -= mean;
            q = threshold * fabs(chaos);
            for(c = 0; c < 3; c++)
            {
                Ix[c] /= 3.0f;
                Iy[c] /= 3.0f;
            }
            if(q != 0.0f)
            {
                // Threshold by q
                (*dst_it)[0] = pix_t(floorf((Ix[0] * Ix[0]) / q) * q + floorf((Ix[1] * Ix[1]) / q) * q +
                                     floorf((Ix[2] * Ix[2]) / q) * q);
                (*dst_it)[1] = pix_t(Ix[0] * Iy[0] + Ix[1] * Iy[1] + Ix[2] * Iy[2]);
                (*dst_it)[2] = pix_t(floorf((Iy[0] * Iy[0]) / q) * q + floorf((Iy[1] * Iy[1]) / q) * q +
                                     floorf((Iy[2] * Iy[2]) / q) * q);
            }
            else
            {
                (*dst_it)[0] = pix_t(Ix[0] * Ix[0] + Ix[1] * Ix[1] + Ix[2] * Ix[2]);
                (*dst_it)[1] = pix_t(Ix[0] * Iy[0] + Ix[1] * Iy[1] + Ix[2] * Iy[2]);
                (*dst_it)[2] = pix_t(Iy[0] * Iy[0] + Iy[1] * Iy[1] + Iy[2] * Iy[2]);
            }
            dst_it++;
        }
    }
    for(y = 0; y < mc2; y++)
    {
        dIterator dst_it = dst.row_begin(y);
        for(x = 0; x < mc2; x++)
        {
            for(c = 0; c < nc; c++)
            {
                (*dst_it)[c] = pix_t(0);
            }
            dst_it++;
        }
        dst_it = dst.row_begin(y);
        for(x = w - mc2; x < w; x++)
        {
            for(c = 0; c < nc; c++)
            {
                (*dst_it)[c] = pix_t(0);
            }
            dst_it++;
        }
    }
    for(y = h - mc2; y < h; y++)
    {
        dIterator dst_it = dst.row_begin(y);
        for(x = 0; x < mc2; x++)
        {
            for(c = 0; c < nc; c++)
            {
                (*dst_it)[c] = pix_t(0);
            }
            dst_it++;
        }
        dst_it = dst.row_begin(y);
        for(x = w - mc2; x < w; x++)
        {
            for(c = 0; c < nc; c++)
            {
                (*dst_it)[c] = pix_t(0);
            }
            dst_it++;
        }
    }
}

/**
 * @brief Compute the eigenvalues and eigenvectors of a symmetric matrix.
 *
 * @param[in]       tensorXY    tensor (view iterator) at a certain position
 * @param[out]      val         eigenvalue
 * @param[out]      val         eigenvector
 * @warning vec MUST BE a linear float vector of dimension 4.
 */
template <typename ViewIt>
void symmetric_eigen(ViewIt& tensorXY, std::vector<double>& val, std::vector<double>& vec)
{
    const double a = (*tensorXY)[0], b = (*tensorXY)[1], c = (*tensorXY)[1], d = (*tensorXY)[2], e = a + d;
    double f = (e * e - 4.0 * (a * d - b * c));
    if(f > 0)
        f = std::sqrt(f);
    else
        f = 0.0;

    const double l1 = 0.5 * (e - f), l2 = 0.5 * (e + f);
    const double theta1 = std::atan2(l2 - a, b), theta2 = std::atan2(l1 - a, b);
    val[0] = l2;
    val[1] = l1;
    vec[0] = std::cos(theta1);
    vec[1] = std::sin(theta1);
    vec[2] = std::cos(theta2);
    vec[3] = std::sin(theta2);
}
}
}

#endif
