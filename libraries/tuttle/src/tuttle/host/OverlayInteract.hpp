#ifndef _TUTTLE_HOST_OVERLAYINTERACT_HPP_
#define _TUTTLE_HOST_OVERLAYINTERACT_HPP_

#include "ImageEffectNode.hpp"

#include <tuttle/host/ofx/OfxhInteract.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/graph/ProcessGraph.hpp>

namespace tuttle
{
namespace host
{

/**
 * An overlay interact for image effects, derived from one of these to
 * be an overlay interact
 */
class OverlayInteract : public ofx::interact::OfxhInteract
{
protected:
    /// our image effect instance
    ImageEffectNode& _instance;
    int _bitDepthPerComponent;
    bool _hasAlpha;
    OfxTime _time;
    graph::ProcessGraph _procGraph;

public:
    OverlayInteract(Graph& userGraph, ImageEffectNode& effect, int bitDepthPerComponent = 8, bool hasAlpha = false);

    void setupGraph()
    {
        _procGraph.setup();
        _procGraph.setupAtTime(_time);
    }

    void draw(const OfxPointD& renderScale);
};
}
}

#endif
