namespace tuttle
{
namespace plugin
{
namespace seExpr
{

template <class View>
SeExprProcess<View>::SeExprProcess(SeExprPlugin& effect)
    : ImageGilProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
    this->setNoMultiThreading();
}

template <class View>
void SeExprProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);

    rod = _plugin._clipDst->getCanonicalRod(args.time, args.renderScale);

    _time = args.time;

    TUTTLE_LOG_INFO(_params._code);

    ImageSynthExpr expr(_params._code);
    expr.vars["u"] = ImageSynthExpr::Var(_params._paramTextureOffset.x);
    expr.vars["v"] = ImageSynthExpr::Var(_params._paramTextureOffset.y);
    expr.vars["w"] = ImageSynthExpr::Var(rod.x2 - rod.x1);
    expr.vars["h"] = ImageSynthExpr::Var(rod.y2 - rod.y1);
    expr.vars["frame"] = ImageSynthExpr::Var(_time);

    bool valid = expr.isValid();
    if(!valid)
    {
        TUTTLE_LOG_ERROR("Invalid expression");
        TUTTLE_LOG_ERROR(expr.parseError());
    }
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void SeExprProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    ImageSynthExpr expr(_params._code);
    expr.vars["u"] = ImageSynthExpr::Var(_params._paramTextureOffset.x);
    expr.vars["v"] = ImageSynthExpr::Var(_params._paramTextureOffset.y);
    expr.vars["w"] = ImageSynthExpr::Var(rod.x2 - rod.x1);
    expr.vars["h"] = ImageSynthExpr::Var(rod.y2 - rod.y1);
    expr.vars["frame"] = ImageSynthExpr::Var(_time);

    double one_over_width = 1.0 / procWindowSize.x;
    double one_over_height = 1.0 / procWindowSize.y;
    double& u = expr.vars["u"].val;
    double& v = expr.vars["v"].val;

    for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
    {
        typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
        for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++dst_it)
        {
            u = one_over_width * (x + .5 - _params._paramTextureOffset.x);
            v = one_over_height * (y + .5 - _params._paramTextureOffset.y);
            SeVec3d result = expr.evaluate();

            color_convert(rgba32f_pixel_t((float)result[0], (float)result[1], (float)result[2], 1.0), *dst_it);
        }
        if(this->progressForward(procWindowSize.x))
            return;
    }
}
}
}
}
