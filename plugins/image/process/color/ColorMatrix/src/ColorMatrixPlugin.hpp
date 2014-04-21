#ifndef _TUTTLE_PLUGIN_COLORMATRIX_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORMATRIX_PLUGIN_HPP_

#include "ColorMatrixDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <terry/globals.hpp>
#include <terry/numeric/matrix.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorMatrix {

struct ColorMatrixProcessParams
{
	typedef boost::numeric::ublas::bounded_matrix<float, 4, 4 > Matrix44;
	Matrix44 matrixMix;
};

/**
 * @brief ColorMatrix plugin
 */
class ColorMatrixPlugin : public ImageEffectGilPlugin
{
public:
    ColorMatrixPlugin( OfxImageEffectHandle handle );

public:
	ColorMatrixProcessParams getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
	OFX::RGBAParam* _redMix;
	OFX::RGBAParam* _greenMix;
	OFX::RGBAParam* _blueMix;
	OFX::RGBAParam* _alphaMix;
};

}
}
}

#endif
