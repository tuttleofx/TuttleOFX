#include "MultiplyPlugin.hpp"
#include "MultiplyProcess.hpp"
#include "MultiplyDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace multiply {


MultiplyPlugin::MultiplyPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_multiplyType   = fetchChoiceParam( kMultiplyType );
	_master         = fetchDoubleParam( kMasterValue );
	_red            = fetchDoubleParam( kRedValue );
	_green          = fetchDoubleParam( kGreenValue );
	_blue           = fetchDoubleParam( kBlueValue );
	_alpha          = fetchDoubleParam( kAlphaValue );
	
	updateInterface();
}

MultiplyProcessParams<MultiplyPlugin::Scalar> MultiplyPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	MultiplyProcessParams<Scalar> params;
	EMultiplyType type;

	type   = static_cast<EMultiplyType>( _multiplyType->getValue() );
	
	switch( type )
	{
		case eMultiplyTypeGlobal:
			params.iRMultiply             =
			    params.iGMultiply         =
			        params.iBMultiply     = 
						params.iAMultiply = _master->getValue();
			break;
			
		case eMultiplyTypeChannels:
			params.iRMultiply = _red   -> getValue();
			params.iGMultiply = _green -> getValue();
			params.iBMultiply = _blue  -> getValue();
			params.iAMultiply = _alpha -> getValue();
			break;
	}
	return params;
}

void MultiplyPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kMultiplyType )
	{
		updateInterface();
	}
}

//bool MultiplyPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	MultiplyProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}
//
//void MultiplyPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	MultiplyProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool MultiplyPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	MultiplyProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void MultiplyPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<MultiplyProcess>( *this, args );
}

void MultiplyPlugin::updateInterface()
{
	bool bMaster, bRGBA;
	switch( getMultiplyType() )
	{
		case eMultiplyTypeGlobal:
			bMaster = false;
			bRGBA   = true;
			break;
		case eMultiplyTypeChannels:
			bMaster = true;
			bRGBA   = false;
			break;
		default: // Error
			bMaster = true;
			bRGBA   = true;
			break;
	}
	_master -> setIsSecret( bMaster );
	_red    -> setIsSecret( bRGBA );
	_green  -> setIsSecret( bRGBA );
	_blue   -> setIsSecret( bRGBA );
	_alpha  -> setIsSecret( bRGBA );
}


}
}
}
