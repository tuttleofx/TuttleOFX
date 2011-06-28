#include "ColorTransfertPlugin.hpp"
#include "ColorTransfertProcess.hpp"
#include "ColorTransfertDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_assign.hpp>

namespace tuttle {
namespace plugin {
namespace colorTransfert {


ColorTransfertPlugin::ColorTransfertPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_clipSrcRef = this->fetchClip( kClipSrcRef );
	_clipDstRef = this->fetchClip( kClipDstRef );

	_paramAverageCoef = this->fetchDoubleParam( kParamAverageCoef );
	_paramDynamicCoef = this->fetchDoubleParam( kParamDynamicCoef );

//	_paramRegionA = this->fetchDouble2DParam( kParamRegionA );
//	_paramRegionB = this->fetchDouble2DParam( kParamRegionB );
//	_paramSameRegion = this->fetchBooleanParam( kParamSameRegion );
//	_paramInputRegionA = this->fetchDouble2DParam( kParamInputRegionA );
//	_paramInputRegionB = this->fetchDouble2DParam( kParamInputRegionB );
//	const OFX::InstanceChangedArgs args( this->timeLineGetTime() );
//	changedParam( args, kParamSameRegion );
}

ColorTransfertProcessParams<ColorTransfertPlugin::Scalar> ColorTransfertPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ColorTransfertProcessParams<Scalar> params;
	params._averageCoef = _paramAverageCoef->getValue();
	params._dynamicCoef = _paramDynamicCoef->getValue();
	return params;
}

void ColorTransfertPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
//	if( paramName == kParamSameRegion )
//	{
//		const bool status = _paramSameRegion->getValue();
//		_paramInputRegionA->setIsSecretAndDisabled( status );
//		_paramInputRegionB->setIsSecretAndDisabled( status );
//	}
}

void ColorTransfertPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	ColorTransfertProcessParams<Scalar> params = getProcessParams();
	
	if( _clipSrcRef->isConnected() )
	{
		OfxRectD srcRod = args.regionOfInterest;
		rois.setRegionOfInterest( *this->_clipSrc, srcRod );
	}
	else
	{
		OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
		rois.setRegionOfInterest( *this->_clipSrc, srcRod );
	}
	
	OfxRectD srcRefRod = _clipSrcRef->getCanonicalRod( args.time );
	rois.setRegionOfInterest( *_clipSrcRef, srcRefRod );
	
	OfxRectD dstRefRod = _clipDstRef->getCanonicalRod( args.time );
	rois.setRegionOfInterest( *_clipDstRef, dstRefRod );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorTransfertPlugin::render( const OFX::RenderArguments &args )
{
//	doGilRender<ColorTransfertProcess>( *this, args );

	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<ColorTransfertProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			doGilRender<ColorTransfertProcess, false, boost::gil::rgb_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}


}
}
}
