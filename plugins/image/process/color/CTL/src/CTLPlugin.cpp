#include "CTLPlugin.hpp"
#include "CTLProcess.hpp"
#include "CTLDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

#include <fstream>

namespace tuttle {
namespace plugin {
namespace ctl {


CTLPlugin::CTLPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramInput = fetchChoiceParam( kParamChooseInput );
	_paramCode = fetchStringParam( kParamCTLCode );
	_paramFile = fetchStringParam( kParamCTLFile );

	changedParam( _instanceChangedArgs, kParamChooseInput );
}

CTLProcessParams<CTLPlugin::Scalar> CTLPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	CTLProcessParams<Scalar> params;
	EParamChooseInput input = static_cast<EParamChooseInput>( _paramInput->getValue() );
	switch( input )
	{
		case eParamChooseInputCode:
		{
			params._code = _paramCode->getValue();
			break;
		}
		case eParamChooseInputFile:
		{
			std::ifstream f( _paramFile->getValue().c_str(), std::ios::in );
			std::getline( f, params._code, '\0' );
			break;
		}
	}
	return params;
}

void CTLPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kParamChooseInput )
	{
		EParamChooseInput input = static_cast<EParamChooseInput>( _paramInput->getValue() );
		switch( input )
		{
			case eParamChooseInputCode:
			{
				_paramCode->setIsSecretAndDisabled( false );
				_paramFile->setIsSecretAndDisabled( true );
				break;
			}
			case eParamChooseInputFile:
			{
				_paramCode->setIsSecretAndDisabled( true );
				_paramFile->setIsSecretAndDisabled( false );
				break;
			}
		}
	}
	else if( paramName == kParamCTLCode )
	{
		_paramInput->setValue( eParamChooseInputCode );
	}
	else if( paramName == kParamCTLFile )
	{
		_paramInput->setValue( eParamChooseInputFile );
	}
}

//bool CTLPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	CTLProcessParams<Scalar> params = getProcessParams();
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
//void CTLPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	CTLProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool CTLPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	CTLProcessParams<Scalar> params = getProcessParams();
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
void CTLPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<CTLProcess>( *this, args );
}


}
}
}
