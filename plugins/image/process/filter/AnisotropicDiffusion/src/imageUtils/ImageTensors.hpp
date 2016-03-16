/**
 * @brief This file provides a class for tensors image generation.
 */

#ifndef _TUTTLE_PLUGIN_IMAGE_TENSORS_HPP_
#define _TUTTLE_PLUGIN_IMAGE_TENSORS_HPP_

#include "filters/edgeDetect.hpp"
#include "filters/blurFilters.hpp"

#include <tuttle/plugin/IProgress.hpp>
#include <terry/math.hpp>

#include <boost/gil/gil_all.hpp>

#include <vector>
#include <string>
#include <float.h>

namespace tuttle
{
namespace imageUtils
{

/**
 * @brief Structure handling anisotropic gradient parameters.
 */
template <typename TensorView>
struct tensor_t
{

    tensor_t()
        : dox(0)
        , doy(0)
        , dw(0)
        , dh(0)
        , algorithm(false)
        , stAlgo(0)
        , alpha(0.0)
        , sigma(0.0)
        , sharpness(0.0)
        , anisotropy(0.0)
        , geom_fact(0.0)
        , threshold(0.0){};
    int dox, doy, dw, dh;
    bool algorithm;    ///< Generation algorithm
    int stAlgo;        ///< Structure tensors algorithm
    double alpha;      ///< Pre-blurring (noise scale)
    double sigma;      ///< Post-blurring
    double sharpness;  ///< Contour preservation
    double anisotropy; ///< Anisotropic filtering
    double geom_fact;  ///< Geometry factor
    double threshold;  ///< Thresholding quantization factor
};

/**
 * @brief Class used to render tensors
 */
template <class View>
class ImageTensors : public View
{
private:
    template <class D_ITER>
    void compute_vectors(D_ITER& iter_dst, const std::vector<double>& val, const std::vector<double>& vec,
                         const double power1, const double power2);

public:
    typedef enum
    {
        eAnisotGradient
    } E_TensorsAlgorithm;

    ImageTensors(const View& view)
        : View(view)
    {
    }

    ImageTensors(int& width, int& height, typename View::xy_locator loc)
        : View(width, height, loc)
    {
    }

    virtual ~ImageTensors() {}
    //@todo: add other constructors

    void process(const View& srcView, E_TensorsAlgorithm tensAlgo, tuttle::plugin::IProgress* progress, void* args);
    void anisotropic_gradient(const View& src, tuttle::plugin::IProgress* progress, tensor_t<View>* args);
};

/**
 * @brief Process rendering
 *
 * @param[in]       srcView   source view
 * @param[in]       progress  tensor algorithm
 * @param[in]       args      algorithm parameters
 *
 */
template <class View>
void ImageTensors<View>::process(const View& srcView, E_TensorsAlgorithm tensAlgo, tuttle::plugin::IProgress* progress,
                                 void* args)
{
    switch(tensAlgo)
    {
        case eAnisotGradient:
            anisotropic_gradient(srcView, progress, (tensor_t<View>*)args);
            break;
    }
    progress->progressEnd();
}

template <class View>
template <class D_ITER>
void ImageTensors<View>::compute_vectors(D_ITER& iter_dst, const std::vector<double>& val, const std::vector<double>& vec,
                                         const double power1, const double power2)
{
    typedef typename bgil::channel_type<D_ITER>::type pix_t;
    const double
        // The more is l1 high, the more smoothing will be performed along
        // the edges direction (because n1 << n2).
        // Otherwise, the smoothing will be performed
        // along all directions (because n1 =~ n2).
        l1 = val[1],
        l2 = val[0],
        // (ux,uy) is orthogonal to (vx, vy)
        vx = vec[0], vy = vec[1], ux = vec[2], uy = vec[3],
        // n1, n2 set the strengths of the desired smoothing factor along
        // respectives directions.
        n1 = std::pow(1.0 + l1 + l2, -power1), n2 = std::pow(1.0 + l1 + l2, -power2),
        // Gaussian kernel strength for horizontal smoothing
        c0 = n1 * ux * ux + n2 * vx * vx,
        // Gaussian kernel strength for diagonal smoothing
        c1 = n1 * ux * uy + n2 * vx * vy,
        // Gaussian kernel strength for vertical smoothing
        c2 = n1 * uy * uy + n2 * vy * vy;
    /*
        c0 = c0 < 0.0 ? 0.0 : (c0 > 1.0 ? 1.0 : c0);
        c1 = c1 < 0.0 ? 0.0 : (c1 > 1.0 ? 1.0 : c1);
        c2 = c2 < 0.0 ? 0.0 : (c2 > 1.0 ? 1.0 : c2);
     */
    // Compute final values
    (*iter_dst)[0] = pix_t(c0);
    (*iter_dst)[1] = pix_t(c1);
    (*iter_dst)[2] = pix_t(c2);
}

/**
 * @brief Anisotropic gradient
 *
 * @param[in]       srcView   source view
 * @param[in]       args      algorithm parameters
 *
 */
template <class View>
void ImageTensors<View>::anisotropic_gradient(const View& src, tuttle::plugin::IProgress* progress, tensor_t<View>* args)
{
    using namespace boost::gil;
    using namespace terry;
    float alpha = (float)args->alpha;
    float sigma = (float)args->sigma;
    float sharpness = (float)args->sharpness;
    float anisotropy = (float)args->anisotropy;
    float geom_factor = (float)args->geom_fact;
    float threshold = (float)args->threshold / 1000.0f;
    int stAlgo = args->stAlgo;
    bool tensorAlgo = args->algorithm;

    if(anisotropy < 0.0f || anisotropy > 1.0f)
        return;
    if(alpha < 0.0f)
        return;
    if(sigma < 0.0f)
        return;
    if(sharpness < 0.0f)
        return;

    // Float type to process computation with reals
    typedef typename image_from_view<View>::type image_t;
    typedef typename View::x_iterator iterator;
    typedef typename View::x_iterator tmp_iterator;
    typedef typename View::locator locator;

    const double nsharpness = std::max((double)sharpness, 1e-5), power1 = 0.5 * nsharpness,
                 power2 = power1 / (1e-7 + 1.0 - anisotropy);
    std::vector<double> val(2);
    std::vector<double> vec(4);
    View& dst = *(View*)this;

    progress->progressBegin(5 * 25, "Tensors generator algorithm in progress");
    image_t tmp1(src.dimensions());
    image_t tmp2(src.dimensions());
    View tmpv1(view(tmp1));
    View tmpv2(view(tmp2));
    // Apply pre-blur (attenuate noise effect in tensors map)
    dericheFilter(src, tmpv1, alpha);
    if(progress->progressForward(25))
        return;

    // Multiply or normalize tensors to amplify image geometry
    if(geom_factor > 0.0f)
        multiply(tmpv1, tmpv2, geom_factor);
    else if(geom_factor < 0.0f)
        normalize(tmpv1, tmpv2, 0.0f, -geom_factor);
    else
        copy_pixels(tmpv1, tmpv2);
    if(progress->progressForward(25))
        return;

    // Compute structure tensors
    switch(stAlgo)
    {
        case 0:
            // Using precise forward backward finite differences
            simple_structure_tensor(tmpv2, tmpv1, threshold);
            break;
        case 1:
            // Using harris edges detection
            harris(tmpv2, tmpv1, threshold);
            break;
        case 2:
            // Using canny-deriche edges detection
            dericheFilter(tmpv2, tmpv1, alpha, 1, threshold);
            break;
        default:
            return;
            break;
    }

    if(progress->progressForward(25))
        return;

    // Apply a post-blur to attenuate the texture
    // of the gradient image
    dericheFilter(tmpv1, tmpv2, sigma);
    if(progress->progressForward(25))
        return;

    // Compute diffusion tensors (gaussian kernel directions map).
    if(tensorAlgo == false)
    {
        const float step = 25 / (args->dh - args->doy);
        for(int y = args->doy; y < args->dh; ++y)
        {
            iterator src_it = tmpv2.row_begin(y);
            iterator dst_it = this->row_begin(y - args->doy);
            if(progress->progressForward((int)step))
                return;
            src_it += args->dox;
            for(int x = args->dox; x < args->dw; ++x)
            {
                // eigen value and vectors gives strength and direction of
                // the vectors for the gaussian kernel.
                symmetric_eigen(src_it, val, vec);
                // Compute vector directions:
                // r = along X, g = along diagonal, b = along y
                compute_vectors(dst_it, val, vec, power1, power2);
                dst_it++;
                src_it++;
            }
        }
    }
    else
    {
        copy_pixels(subimage_view(tmpv2, args->dox, args->doy, dst.width(), dst.height()), dst);
    }
}
}
}

#endif
