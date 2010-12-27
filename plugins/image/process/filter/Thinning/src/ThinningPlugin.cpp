#include "ThinningPlugin.hpp"
#include "ThinningProcess.hpp"
#include "ThinningDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace thinning {

ThinningPlugin::ThinningPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramBorder = fetchChoiceParam( kParamBorder );
}

ThinningProcessParams<ThinningPlugin::Scalar> ThinningPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ThinningProcessParams<Scalar> params;
	params._border = static_cast<EParamBorder>( _paramBorder->getValue() );
	return params;
}

bool ThinningPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ThinningProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	switch( params._border )
	{
		case eParamBorderPadded:
			rod = rectangleReduce( srcRod, 2 );
			return true;
		default:
			break;
	}
	return false;
}

void ThinningPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	ThinningProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	
	OfxRectD srcRoi = rectangleGrow( srcRod, 2 );

	rois.setRegionOfInterest( *_clipSrc, srcRoi );
}


/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ThinningPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<ThinningProcess>( *this, args );
}

}
}
}
