#include "AddPlugin.hpp"
#include "AddProcess.hpp"
#include "AddDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace add {


AddPlugin::AddPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_addType   = fetchChoiceParam( kAddType );
	_master    = fetchDoubleParam( kMasterValue );
	_red       = fetchDoubleParam( kRedValue );
	_green     = fetchDoubleParam( kGreenValue );
	_blue      = fetchDoubleParam( kBlueValue );
	_alpha     = fetchDoubleParam( kAlphaValue );
	
	updateInterface();
}

AddProcessParams<AddPlugin::Scalar> AddPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	AddProcessParams<Scalar> params;
	EAddType type;

	type   = static_cast<EAddType>( _addType->getValue() );
	
	switch( type )
	{
		case eAddTypeGlobal:
			params.iRAdd             =
			    params.iGAdd         =
			        params.iBAdd     = 
						params.iAAdd = _master->getValue();
			break;
			
		case eAddTypeChannels:
			params.iRAdd = _red   -> getValue();
			params.iGAdd = _green -> getValue();
			params.iBAdd = _blue  -> getValue();
			params.iAAdd = _alpha -> getValue();
			break;
	}
	return params;
}

void AddPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kAddType )
	{
		updateInterface();
	}
}

//bool AddPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	AddProcessParams<Scalar> params = getProcessParams();
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
//void AddPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	AddProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool AddPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	AddProcessParams<Scalar> params = getProcessParams();
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
void AddPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<AddProcess>( *this, args );
}

void AddPlugin::updateInterface()
{
	bool bMaster, bRGBA;
	switch( getAddType() )
	{
		case eAddTypeGlobal:
			bMaster = false;
			bRGBA   = true;
			break;
		case eAddTypeChannels:
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
