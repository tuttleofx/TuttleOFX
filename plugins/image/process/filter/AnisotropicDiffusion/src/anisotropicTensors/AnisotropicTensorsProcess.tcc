
namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

/**
 * @brief Constructor of the TensorsBase class
 *
 * @param[in, out]  instance     Plugin instance
 */
template <class View>
AnisotropicTensorsProcess<View>::AnisotropicTensorsProcess(TensorsPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
    _stAlgo = instance.fetchChoiceParam(kParamEdgeDetectAlgo);
    _algorithm = instance.fetchBooleanParam(kParamDisplayStructureTensors);
    _alpha = instance.fetchDoubleParam(kParamAlpha);
    _sigma = instance.fetchDoubleParam(kParamSigma);
    _sharpness = instance.fetchDoubleParam(kParamSharpness);
    _anisotropy = instance.fetchDoubleParam(kParamAnisotropy);
    _geom_fact = instance.fetchDoubleParam(kParamGeometryFactor);
    _threshold = instance.fetchDoubleParam(kParamThresholdingQuantization);
}

/**
 * @brief Function for the effect processing.
 * @param[in]   args     Rendering parameters
 */
template <class View>
void AnisotropicTensorsProcess<View>::setup(const OFX::RenderArguments& args)
{
    // fetch main upscaled input image
    this->_src.reset(_plugin._clipSrc->fetchImage(args.time));
    if(!this->_src.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady());
    _upScaledSrcBounds = this->_src->getBounds();

    // fetch output image
    this->_dst.reset(_plugin._clipDst->fetchImage(args.time));
    if(!this->_dst.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady());
    _dBounds = this->_dst->getBounds();

    // Make sure bit depths are same
    OFX::EBitDepth srcBitDepth = this->_src->getPixelDepth();
    OFX::EPixelComponent srcComponents = this->_src->getPixelComponents();
    OFX::EBitDepth dstBitDepth = this->_dst->getPixelDepth();
    OFX::EPixelComponent dstComponents = this->_dst->getPixelComponents();

    // See if they have the same depths and bytes and all
    if(srcBitDepth != dstBitDepth || srcComponents != dstComponents)
    {
        BOOST_THROW_EXCEPTION(exception::BitDepthMismatch());
    }

    this->_srcView =
        interleaved_view(_upScaledSrcBounds.x2 - _upScaledSrcBounds.x1, _upScaledSrcBounds.y2 - _upScaledSrcBounds.y1,
                         (Pixel*)this->_src->getPixelData(), this->_src->getRowDistanceBytes());

    this->_dstView = interleaved_view(_dBounds.x2 - _dBounds.x1, _dBounds.y2 - _dBounds.y1,
                                      (Pixel*)this->_dst->getPixelData(), this->_dst->getRowDistanceBytes());
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void AnisotropicTensorsProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    int margin = _plugin.getMargin();
    imageUtils::tensor_t<View> params;
    params.stAlgo = _stAlgo->getValue();
    params.algorithm = _algorithm->getValue();
    params.alpha = _alpha->getValue();
    params.sigma = _sigma->getValue();
    params.sharpness = _sharpness->getValue();
    params.anisotropy = _anisotropy->getValue();
    params.geom_fact = _geom_fact->getValue();
    params.threshold = _threshold->getValue();

    // dest width and height
    params.dw = procWindowRoW.x2 - procWindowRoW.x1;
    params.dh = procWindowRoW.y2 - procWindowRoW.y1;

    // Set upscaled source
    OfxRectI srcRect;
    srcRect.x1 = procWindowRoW.x1 - margin;
    srcRect.y1 = procWindowRoW.y1 - margin;
    srcRect.x2 = procWindowRoW.x2 + margin + 1;
    srcRect.y2 = procWindowRoW.y2 + margin + 1;

    srcRect = rectanglesIntersection(srcRect, _upScaledSrcBounds);

    // dest starting offset among x and y
    params.dox = procWindowRoW.x1 - srcRect.x1;
    params.doy = procWindowRoW.y1 - srcRect.y1;
    params.dw = params.dox + procWindowRoW.x2 - procWindowRoW.x1;
    params.dh = params.doy + procWindowRoW.y2 - procWindowRoW.y1;

    // We want to work in the source roi space
    View src = subimage_view(this->_srcView, srcRect.x1 - _upScaledSrcBounds.x1, srcRect.y1 - _upScaledSrcBounds.y1,
                             srcRect.x2 - srcRect.x1, srcRect.y2 - srcRect.y1);
    // We want to work in the source roi space
    View dst = subimage_view(this->_dstView, procWindowRoW.x1 - this->_renderArgs.renderWindow.x1,
                             procWindowRoW.y1 - this->_renderArgs.renderWindow.y1, procWindowRoW.x2 - procWindowRoW.x1,
                             procWindowRoW.y2 - procWindowRoW.y1);

    imageUtils::ImageTensors<View> dv(dst);
    dv.process(src, imageUtils::ImageTensors<View>::eAnisotGradient, this, &params);
}
}
}
}
}
