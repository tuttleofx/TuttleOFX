#include <boost/math/constants/constants.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace diffusion
{

static const float kEpsilon = 1e-5f;

template <class View>
AnisotropicDiffusionProcess<View>::AnisotropicDiffusionProcess(AnisotropicDiffusionPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
    _fast_approx = instance.fetchBooleanParam(kParamFastApproximation);
    _amplitude = instance.fetchRGBParam(kParamAmplitude);
}

template <class View>
void AnisotropicDiffusionProcess<View>::setup(const OFX::RenderArguments& args)
{
    // Fetch output image
    this->_dst.reset(_plugin._clipDst->fetchImage(args.time));
    if(!this->_dst.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady() << exception::user("Output"));
    _dBounds = this->_dst->getBounds();

    OFX::EBitDepth dstBitDepth = this->_dst->getPixelDepth();
    OFX::EPixelComponent dstComponents = this->_dst->getPixelComponents();

    // Fetch main input image
    this->_src.reset(_plugin._clipSrc->fetchImage(args.time));
    if(!this->_src.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady() << exception::user("Input"));
    _upScaledSrcBounds = this->_src->getBounds();

    if(!_plugin._clipSrcTensors->isConnected())
        BOOST_THROW_EXCEPTION(exception::ImageNotConnected() << exception::user("Tensor"));

    // Fetch input tensors image
    _srcTensor.reset(_plugin._clipSrcTensors->fetchImage(args.time));
    if(!_srcTensor.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady() << exception::user("Tensors"));

    if(_srcTensor->getRowDistanceBytes() < 0 || this->_dst->getRowDistanceBytes() < 0 ||
       this->_src->getRowDistanceBytes() != _srcTensor->getRowDistanceBytes())
    {
        BOOST_THROW_EXCEPTION(exception::InOutMismatch() << exception::user("Incompatible row bytes !"));
    }

    // Make sure bit depths are same
    OFX::EBitDepth srcBitDepth = this->_src->getPixelDepth();
    OFX::EPixelComponent srcComponents = this->_src->getPixelComponents();

    // See if they have the same bit depths
    if(srcBitDepth != dstBitDepth || srcComponents != dstComponents)
    {
        BOOST_THROW_EXCEPTION(exception::BitDepthMismatch());
    }

    // Build views
    _srcView = interleaved_view(_upScaledSrcBounds.x2 - _upScaledSrcBounds.x1, _upScaledSrcBounds.y2 - _upScaledSrcBounds.y1,
                                static_cast<Pixel*>(this->_src->getPixelData()), this->_src->getRowDistanceBytes());

    _srcTensorView =
        interleaved_view(_upScaledSrcBounds.x2 - _upScaledSrcBounds.x1, _upScaledSrcBounds.y2 - _upScaledSrcBounds.y1,
                         static_cast<Pixel*>(_srcTensor->getPixelData()), _srcTensor->getRowDistanceBytes());

    this->_dstView = interleaved_view(_dBounds.x2 - _dBounds.x1, _dBounds.y2 - _dBounds.y1,
                                      static_cast<Pixel*>(this->_dst->getPixelData()), this->_dst->getRowDistanceBytes());
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void AnisotropicDiffusionProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    int margin = _plugin.getMargin();
    OfxRGBColourD amplitude_rgb = _amplitude->getValue();
    region_t dregion;

    // dest width and height
    dregion.dw = procWindowRoW.x2 - procWindowRoW.x1;
    dregion.dh = procWindowRoW.y2 - procWindowRoW.y1;

    // Set upscaled source
    OfxRectI srcRect;
    srcRect.x1 = procWindowRoW.x1 - margin;
    srcRect.y1 = procWindowRoW.y1 - margin;
    srcRect.x2 = procWindowRoW.x2 + margin + 1;
    srcRect.y2 = procWindowRoW.y2 + margin + 1;

    srcRect = rectanglesIntersection(srcRect, _upScaledSrcBounds);

    // dest starting offset among x and y
    dregion.dox = procWindowRoW.x1 - srcRect.x1;
    dregion.doy = procWindowRoW.y1 - srcRect.y1;
    dregion.dw = dregion.dox + procWindowRoW.x2 - procWindowRoW.x1;
    dregion.dh = dregion.doy + procWindowRoW.y2 - procWindowRoW.y1;

    // We want to work in the source roi space
    View src = subimage_view(_srcView, srcRect.x1 - _upScaledSrcBounds.x1, srcRect.y1 - _upScaledSrcBounds.y1,
                             srcRect.x2 - srcRect.x1, srcRect.y2 - srcRect.y1);

    View srct = subimage_view(_srcTensorView, srcRect.x1 - _upScaledSrcBounds.x1, srcRect.y1 - _upScaledSrcBounds.y1,
                              srcRect.x2 - srcRect.x1, srcRect.y2 - srcRect.y1);

    // We want to work in the source roi space
    View dst =
        subimage_view(this->_dstView, procWindowRoW.x1 - this->_renderArgs.renderWindow.x1 - dregion.dox,
                      procWindowRoW.y1 - this->_renderArgs.renderWindow.y1 - dregion.doy,
                      procWindowRoW.x2 - procWindowRoW.x1 + dregion.dw, procWindowRoW.y2 - procWindowRoW.y1 + dregion.dh);

    blur_anisotropic(dst, src, srct, dregion, amplitude_rgb, _fast_approx->getValue());
}

/**
 * @brief Function called to apply an anisotropic blur
 *
 * @param[out]  dst     Destination image view
 * @param[in]   amplitude     Amplitude of the anisotropic blur
 * @param dl    spatial discretization.
 * @param da    angular discretization.
 * @param gauss_prec    precision of the gaussian function.
 * @param fast_approx   Tell to use the fast approximation or not.
 *
 * @return Result view of the blurring process
 */
template <class View>
void AnisotropicDiffusionProcess<View>::blur_anisotropic(View& dst, View& src, View& G, const region_t& dregion,
                                                         const OfxRGBColourD& amplitude, const bool fast_approx /*=true*/,
                                                         const float dl /* =0.8f */, const float da /* = 30.0f */,
                                                         const float gauss_prec /*=2.0f*/)
{
    using namespace boost::gil;
    using namespace terry;

    typedef typename View::x_iterator dIterator;
    typedef typename channel_type<View>::type pix_t;
    typedef typename channel_type<View>::type dpix_t;

    if(amplitude.r > 0.0f || amplitude.g > 0.0f || amplitude.b > 0.0f)
    {
        if(src.num_channels() < 3 || dst.num_channels() < 3)
        {
            TUTTLE_LOG_ERROR("blur_anisotropic() : Specified tensor field (" << (int)G.width() << ", " << (int)G.height()
                                                                             << ", " << (int)G.num_channels()
                                                                             << ") is not valid.");
            return;
        }

        if(src.dimensions() != G.dimensions())
        {
            TUTTLE_LOG_ERROR("blur_anisotropic() : Incompatible dimensions.");
            return;
        }

        // Create a float compatible view
        typedef pixel<bits32f, devicen_layout_t<num_channels<View>::value> > comp_pixel_t;
        typedef image<comp_pixel_t, false, std::allocator<unsigned char> > comp_image_t;
        typedef typename view_type_from_pixel<comp_pixel_t>::type comp_view_t;
        typedef typename comp_view_t::x_iterator tmp_iterator;

        comp_image_t itmp(src.dimensions());
        comp_view_t tmpv(view(itmp));

        const float amplitude_tab[] = {amplitude.r, amplitude.g, amplitude.b};
        const float sqrt2amplitude_r = std::sqrt(2.0f * amplitude.r);
        const float sqrt2amplitude_g = std::sqrt(2.0f * amplitude.g);
        const float sqrt2amplitude_b = std::sqrt(2.0f * amplitude.b);
        const unsigned int dx1 = src.width() - 1, dy1 = src.height() - 1, w = src.width(), h = src.height(),
                           nc = std::min((int)src.num_channels(), std::min(3, (int)dst.num_channels()));

        std::vector<float> sW(w * h * nc);
        float* pW = NULL;
        std::vector<float> tmp(nc);
        int N = 0;

        this->progressBegin((int)((360.0f - (360 % (int)da) / 2.0f) / da) * h, "PDE Denoiser algorithm in progress");
        fill_black(tmpv /*subimage_view(tmpv, dregion.dox, dregion.doy, dregion.dw, dregion.dh)*/);

        // 2D version of the algorithm
        for(float theta = (360 % (int)da) / 2.0f; theta < 360.0f; (theta += da), ++N)
        {
            const float thetar = theta * boost::math::constants::pi<float>() / 180.0f, vx = std::cos(thetar),
                        vy = std::sin(thetar);
            pW = &sW[0];
            for(unsigned int yg = 0; yg < h; ++yg)
            {
                dIterator iterG = G.row_begin(yg);
                for(unsigned int xg = 0; xg < w; ++xg)
                {
                    const float a = (*iterG)[0], b = (*iterG)[1], c = (*iterG)[2];
                    const float u = a * vx + b * vy, v = b * vx + c * vy, n = std::sqrt(kEpsilon + u * u + v * v);
                    float dln = 0.0f;
                    if(n > 0.0f)
                        dln = dl / n;

                    pW[0] = u * dln;
                    pW[1] = v * dln;
                    pW[2] = n;
                    pW += nc;
                    iterG++;
                }
            }
            pW = &sW[0];
            for(unsigned int y = 0; y < h; ++y)
            {
                tmp_iterator tmp_it = tmpv.row_begin(y);
                dIterator view_it = src.row_begin(y);
                for(unsigned int x = 0; x < w; ++x)
                {
                    memset(&tmp[0], 0, sizeof(float) * nc);
                    const float cu = pW[0], cv = pW[1], n = pW[2], fsigma_r = sqrt2amplitude_r * n,
                                length_r = gauss_prec * fsigma_r, fsigma2_r = 2.0f * fsigma_r * fsigma_r,
                                fsigma_g = sqrt2amplitude_g * n, length_g = gauss_prec * fsigma_g,
                                fsigma2_g = 2.0f * fsigma_g * fsigma_g, fsigma_b = sqrt2amplitude_b * n,
                                length_b = gauss_prec * fsigma_b, fsigma2_b = 2.0f * fsigma_b * fsigma_b;

                    const float lrgba[] = {length_r, length_g, length_b, length_r + length_g + length_b / 3.0f};

                    const float srgba[] = {fsigma2_r, fsigma2_g, fsigma2_b, fsigma2_r + fsigma2_g + fsigma2_b / 3.0f};

                    float S, pu, pv, X, Y;
                    pW += nc;
                    if(n == 0.0f)
                    {
                        for(unsigned int i = 0; i < nc; ++i)
                            (*tmp_it)[i] += (*view_it)[i];
                    }
                    else
                    {
                        // Nearest-neighbor interpolation for 2D images
                        if(fast_approx)
                        {
                            for(unsigned int i = 0; i < nc; ++i)
                            {
                                X = (float)x;
                                Y = (float)y;
                                S = 0.0f;
                                pu = cu;
                                pv = cv;
                                if(amplitude_tab[i] > 0)
                                    for(float l = 0.0f; l < lrgba[i] && X >= 0.0f && X <= dx1 && Y >= 0.0f && Y <= dy1;
                                        l += dl)
                                    {
                                        const int cx = (int)(X + 0.5f), cy = (int)(Y + 0.5f);
                                        float u = sW[(cy * w + cx) * nc + 0], v = sW[(cy * w + cx) * nc + 1];
                                        if((pu * u + pv * v) < 0.0f)
                                        {
                                            u = -u;
                                            v = -v;
                                        }
                                        tmp[i] += src(cx, cy)[i];
                                        ++S;
                                        X += (pu = u);
                                        Y += (pv = v);
                                    }
                                if(S > 0)
                                    (*tmp_it)[i] += (dpix_t)(tmp[i] / S);
                                else
                                    (*tmp_it)[i] += (*view_it)[i];
                            }
                        }
                        else
                        {
                            for(unsigned int i = 0; i < nc; ++i)
                            {
                                X = (float)x;
                                Y = (float)y;
                                S = 0.0f;
                                pu = cu;
                                pv = cv;
                                if(amplitude_tab[i] > 0.0f)
                                    for(float l = 0.0f; l < lrgba[i] && X >= 0.0f && X <= dx1 && Y >= 0.0f && Y <= dy1;
                                        l += dl)
                                    {
                                        const int cx = (int)(X + 0.5f), cy = (int)(Y + 0.5f);
                                        float u = sW[(cy * w + cx) * nc + 0], v = sW[(cy * w + cx) * nc + 1];
                                        if((pu * u + pv * v) < 0.0f)
                                        {
                                            u = -u;
                                            v = -v;
                                        }
                                        const float coef = std::exp(-l * l / srgba[i]);
                                        tmp[i] += coef * src(cx, cy)[i];
                                        S += coef;
                                        X += (pu = u);
                                        Y += (pv = v);
                                    }
                                if(S > 0)
                                    (*tmp_it)[i] += (dpix_t)(tmp[i] / S);
                                else
                                    (*tmp_it)[i] += (*view_it)[i];
                            }
                        }
                    }
                    ++view_it;
                    ++tmp_it;
                }
                if(this->progressForward(w))
                    return;
            }
        }

        for(unsigned int y = dregion.doy; y < dregion.dh; ++y)
        {
            tmp_iterator t_iter = tmpv.row_begin(y);
            dIterator d_iter = dst.row_begin(y);
            t_iter += dregion.dox;
            d_iter += dregion.dox;

            for(unsigned int x = dregion.dox; x < dregion.dw; ++x)
            {
                for(unsigned int c = 0; c < nc; ++c)
                {
                    (*d_iter)[c] = (dpix_t)((*t_iter)[c] / N);
                }
                ++d_iter;
                ++t_iter;
            }
        }
    }
    else
    {
        copy_pixels(subimage_view(src, dregion.dox, dregion.doy, dregion.dw, dregion.dh),
                    subimage_view(dst, dregion.dox, dregion.doy, dregion.dw, dregion.dh));
    }
}
}
}
}
}
