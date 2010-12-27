#ifndef _VOLET_PROCESS_HPP_
#define _VOLET_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

/**
 * @brief Base class
 *
 */
template<class View>
class CropProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

protected:
	CropPlugin&          _plugin;        ///< Rendering plugin
	OfxRectI _srcBounds;    ///< Current source bounds
	OfxRectI _srcROD;       ///< Current source bounds
	OfxRectD _clipROD;      ///< Source clip region of definition
	OfxPointD _renderScale;  ///< Render scale
	double _par;          ///< Pixel aspect ratio
	OFX::IntParam*        _upBand;        ///< Up band size
	OFX::IntParam*        _downBand;      ///< Down band size
	OFX::IntParam*        _leftBand;      ///< Left band size
	OFX::IntParam*        _rightBand;     ///< Right band size
	OFX::BooleanParam*    _anamorphic;    ///< Anormorphic (stretch)

public:
	CropProcess<View>( CropPlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

private:
	OfxRectD getCrop() const;
};

}
}
}

#include "CropProcess.tcc"

#endif

