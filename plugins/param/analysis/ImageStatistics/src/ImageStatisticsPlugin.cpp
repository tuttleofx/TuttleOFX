#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsProcess.hpp"
#include "ImageStatisticsDefinitions.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

ImageStatisticsPlugin::ImageStatisticsPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramCoordinateSystem = fetchChoiceParam( kParamCoordinateSystem );
	_paramRectCenter       = fetchDouble2DParam( kParamRectCenter );
	_paramRectSize         = fetchDouble2DParam( kParamRectSize );
	_paramChooseOutput     = fetchChoiceParam( kParamChooseOutput );

	_paramOutputAverage       = fetchRGBAParam( kParamOutputAverage );
	_paramOutputChannelMin    = fetchRGBAParam( kParamOutputChannelMin );
	_paramOutputChannelMax    = fetchRGBAParam( kParamOutputChannelMax );
	_paramOutputLuminosityMin = fetchRGBAParam( kParamOutputLuminosityMin );
	_paramOutputLuminosityMax = fetchRGBAParam( kParamOutputLuminosityMax );
	_paramOutputKurtosis      = fetchRGBAParam( kParamOutputKurtosis );
	_paramOutputSkewness      = fetchRGBAParam( kParamOutputSkewness );

	_paramOutputAverageHSL       = fetchDouble3DParam( kParamOutputAverageHSL );
	_paramOutputChannelMinHSL    = fetchDouble3DParam( kParamOutputChannelMinHSL );
	_paramOutputChannelMaxHSL    = fetchDouble3DParam( kParamOutputChannelMaxHSL );
	_paramOutputLuminosityMinHSL = fetchDouble3DParam( kParamOutputLuminosityMinHSL );
	_paramOutputLuminosityMaxHSL = fetchDouble3DParam( kParamOutputLuminosityMaxHSL );
	_paramOutputKurtosisHSL      = fetchDouble3DParam( kParamOutputKurtosisHSL );
	_paramOutputSkewnessHSL      = fetchDouble3DParam( kParamOutputSkewnessHSL );
}

ImageStatisticsProcessParams ImageStatisticsPlugin::getProcessParams( const OfxRectI& srcRod ) const
{
	ImageStatisticsProcessParams params;
	OfxPointD rectCenter = _paramRectCenter->getValue();
	OfxPointD rectSize   = _paramRectSize->getValue();

	rectSize.x = std::abs( rectSize.x );
	rectSize.y = std::abs( rectSize.y );
	if( _paramCoordinateSystem->getValue() == eParamCoordinateSystemNormalized )
	{
		OfxPointD projectSize = this->getProjectSize();
		rectCenter *= projectSize;
		rectSize   *= projectSize;
	}

	params._rect.x1      = boost::numeric_cast<int>( rectCenter.x - rectSize.x );
	params._rect.y1      = boost::numeric_cast<int>( rectCenter.y - rectSize.y );
	params._rect.x2      = boost::numeric_cast<int>( std::ceil( rectCenter.x + rectSize.x ) );
	params._rect.y2      = boost::numeric_cast<int>( std::ceil( rectCenter.y + rectSize.y ) );
	params._rect         = rectanglesIntersection( params._rect, srcRod );
	params._chooseOutput = static_cast<EParamChooseOutput>( _paramChooseOutput->getValue() );

	return params;
}

void ImageStatisticsPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	OfxRectI srcRealRoi = getProcessParams( _clipSrc->getPixelRod( args.time ) )._rect; // we need the selected rectangle
	OfxRectD srcRealRoiD;

	srcRealRoiD.x1 = srcRealRoi.x1;
	srcRealRoiD.y1 = srcRealRoi.y1;
	srcRealRoiD.x2 = srcRealRoi.x2;
	srcRealRoiD.y2 = srcRealRoi.y2;
//	TUTTLE_COUT_VAR( srcRealRoiD );
	
	rois.setRegionOfInterest( *_clipSrc, srcRealRoiD );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageStatisticsPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			switch( bitDepth )
			{
				case OFX::eBitDepthFloat:
				{
					doGilRender<ImageStatisticsProcess, false, boost::gil::rgba_layout_t, boost::gil::bits32f>( *this, args );
					return;
				}
				case OFX::eBitDepthUByte:
				case OFX::eBitDepthUShort:
				case OFX::eBitDepthNone:
				case OFX::eBitDepthCustom:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
				}
			}
			break;
		}
		case OFX::ePixelComponentRGB:
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

void ImageStatisticsPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamCoordinateSystem )
	{
		OfxPointD projectSize = this->getProjectSize();
		OfxPointD rectCenter  = _paramRectCenter->getValue();
		OfxPointD rectSize    = _paramRectSize->getValue();
		switch( _paramCoordinateSystem->getValue() )
		{
			case eParamCoordinateSystemCanonical:
			{
				rectCenter *= projectSize;
				rectSize   *= projectSize;
				break;
			}
			case eParamCoordinateSystemNormalized:
			{
				rectCenter /= projectSize;
				rectSize   /= projectSize;
				break;
			}
			default:
				BOOST_THROW_EXCEPTION( exception::Value() << exception::user( "Unrecognized coordinate system option." ) );
		}
		_paramRectCenter->setValue( rectCenter );
		_paramRectSize->setValue( rectSize );
	}
}

}
}
}
