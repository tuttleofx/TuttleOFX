#include "OperatorsPlugin.hpp"
#include "OperatorsProcess.hpp"
#include "OperatorsDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace operators {


OperatorsPlugin::OperatorsPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_operator        = fetchChoiceParam( kOperatorsOperator );
	_operatorsType   = fetchChoiceParam( kOperatorsType );

	_master          = fetchDoubleParam( kMasterValue );
	_red             = fetchDoubleParam( kRedValue );
	_green           = fetchDoubleParam( kGreenValue );
	_blue            = fetchDoubleParam( kBlueValue );
	_alpha           = fetchDoubleParam( kAlphaValue );

	_redSelect       = fetchBooleanParam( kRedChannel );
	_greenSelect     = fetchBooleanParam( kGreenChannel );
	_blueSelect      = fetchBooleanParam( kBlueChannel );
	_alphaSelect     = fetchBooleanParam( kAlphaChannel );

	updateInterface();
}

OperatorsProcessParams<OperatorsPlugin::Scalar> OperatorsPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	OperatorsProcessParams<Scalar> params;
	EOperatorsType type;

	params.op  = static_cast<EOperatorsOperators>( _operator->getValue() );

	type   = static_cast<EOperatorsType>( _operatorsType->getValue() );
	
	params.bRProcess = _redSelect   -> getValue();
	params.bGProcess = _greenSelect -> getValue();
	params.bBProcess = _blueSelect  -> getValue();
	params.bAProcess = _alphaSelect -> getValue();

	switch( type )
	{
		case eOperatorsTypeRgba:
			switch( params.op )
			{
				case eOperatorsOperatorPlus     :
				case eOperatorsOperatorMultiply :
				case eOperatorsOperatorPow      :
					params.iROperators             =
						params.iGOperators         =
							params.iBOperators     =
								params.iAOperators = _master->getValue();
					break;
				case eOperatorsOperatorSqrt     :
				case eOperatorsOperatorLog      :
				case eOperatorsOperatorLn       :
					params.iROperators             =
						params.iGOperators         =
							params.iBOperators     =
								params.iAOperators = 1.0; // means process channels
					break;
			}

			break;
		case eOperatorsTypeRgb:
			switch( params.op )
			{
				case eOperatorsOperatorPlus     :
				case eOperatorsOperatorMultiply :
				case eOperatorsOperatorPow      :
					params.iROperators             =
						params.iGOperators         =
							params.iBOperators     = _master->getValue();
								params.iAOperators = 1.0;
					break;
				case eOperatorsOperatorSqrt     :
				case eOperatorsOperatorLog      :
				case eOperatorsOperatorLn       :
					params.iROperators             =
						params.iGOperators         =
							params.iBOperators     = 1.0; // means process channels
					params.iAOperators = 0.0; // means not process the alpha channel
					break;
			}

			if( params.op == eOperatorsOperatorPlus ) params.iAOperators = 0.0;
			params.bAProcess = false;

		case eOperatorsTypeChannels:
			switch( params.op )
			{
				case eOperatorsOperatorPlus     :
				case eOperatorsOperatorMultiply :
				case eOperatorsOperatorPow      :
					params.iROperators = _red   -> getValue();
					params.iGOperators = _green -> getValue();
					params.iBOperators = _blue  -> getValue();
					params.iAOperators = _alpha -> getValue();
					break;
				case eOperatorsOperatorSqrt     :
				case eOperatorsOperatorLog      :
				case eOperatorsOperatorLn       :
					params.iROperators             =
						params.iGOperators         =
							params.iBOperators     = 1.0; // means process channels
					params.iAOperators = 0.0; // means not process the alpha channel
					break;
			}

			break;
	}
	return params;
}

void OperatorsPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kOperatorsType )
	{
		updateInterface();
	}
}

//bool OperatorsPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	OperatorsProcessParams<Scalar> params = getProcessParams();
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
//void OperatorsPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	OperatorsProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool OperatorsPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	OperatorsProcessParams<Scalar> params = getProcessParams();
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
void OperatorsPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<OperatorsProcess>( *this, args );
}

void OperatorsPlugin::updateInterface()
{
	bool bMaster, bRGBA;
	bool bAlpha;
	switch( getOperatorsType() )
	{
		case eOperatorsTypeRgba:
			bMaster = false;
			bRGBA   = true;
			bAlpha  = false;
			break;
		case eOperatorsTypeRgb:
			bMaster = false;
			bRGBA   = true;
			bAlpha  = true;
			break;
		case eOperatorsTypeChannels:
			bMaster = true;
			bRGBA   = false;
			bAlpha  = false;
			break;
		default: // Error
			bMaster = true;
			bRGBA   = true;
			bAlpha  = false;
			break;
	}
	_master      -> setIsSecretAndDisabled( bMaster );
	_red         -> setIsSecretAndDisabled( bRGBA );
	_green       -> setIsSecretAndDisabled( bRGBA );
	_blue        -> setIsSecretAndDisabled( bRGBA );
	_alpha       -> setIsSecretAndDisabled( bRGBA );

	_alphaSelect -> setIsSecretAndDisabled( bAlpha );
}


}
}
}
