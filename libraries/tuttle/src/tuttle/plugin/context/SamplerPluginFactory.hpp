#ifndef _TUTTLE_PLUGIN_SAMPLERPLUGINFACTORY_HPP_
#define	_TUTTLE_PLUGIN_SAMPLERPLUGINFACTORY_HPP_

#include "SamplerDefinition.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {

void addFilterParameters( OFX::ImageEffectDescriptor& desc )
{
	OFX::ChoiceParamDescriptor* filter = desc.defineChoiceParam( kParamFilter );
	filter->setLabel		( kParamFilterLabel );

	filter->appendOption	( kParamFilterNearest   );
	filter->appendOption	( kParamFilterBilinear  );
	filter->appendOption	( kParamFilterBC        );
	filter->appendOption	( kParamFilterBicubic   );
	filter->appendOption	( kParamFilterCatrom    );
	filter->appendOption	( kParamFilterKeys      );
	filter->appendOption	( kParamFilterSimon     );
	filter->appendOption	( kParamFilterRifman    );
	filter->appendOption	( kParamFilterMitchell  );
	filter->appendOption	( kParamFilterParzen    );
	filter->appendOption	( kParamFilterLanczos   );
	filter->appendOption	( kParamFilterLanczos3  );
	filter->appendOption	( kParamFilterLanczos4  );
	filter->appendOption	( kParamFilterLanczos6  );
	filter->appendOption	( kParamFilterLanczos12 );
	filter->appendOption	( kParamFilterGaussian  );

	filter->setDefault	( terry::sampler::eParamFilterBicubic   );
	filter->setHint(
			"Interpolation methods\n"
			"\n"
			/// @todo: documentation...
			"Nearest: \n"
			"Bilinear: \n"
			"Bicubic: Cubic filter(0.0, 0.0)\n"
			"Catmul: Cubic filter(0.0, 0.5)\n"
			"Mitchell: Cubic filter(1/3, 1/3)\n"
			"Parsen: Cubic filter(1.0, 0.0)\n"
			"Keys: \n"
			"Simon: \n"
			"Rifman: \n"
		);

	OFX::DoubleParamDescriptor* B = desc.defineDoubleParam( kParamFilterB );
	B->setLabel			( "B value" );
	B->setDefault			( 1.0 );
	B->setRange			( 0.0, 1.0 );
	B->setDisplayRange		( 0.0, 1.0 );
	B->setHint			( "Adjust the B valuye of the cubic filter." );

	OFX::DoubleParamDescriptor* C = desc.defineDoubleParam( kParamFilterC );
	C->setLabel			( "C value" );
	C->setDefault			( 0.0 );
	C->setRange			( 0.0, 1.0 );
	C->setDisplayRange		( 0.0, 1.0 );
	C->setHint			( "Adjust the C valuye of the cubic filter." );

	OFX::IntParamDescriptor* filterSize = desc.defineIntParam( kParamFilterSize );
	filterSize->setLabel		( "Filter size" );
	filterSize->setDefault		( 3.0 );
	filterSize->setRange		( 1.0, 30.0 );
	filterSize->setDisplayRange	( 1.0, 30.0 );
	filterSize->setHint		( "Set the filter size." );

	OFX::DoubleParamDescriptor* filterSigma = desc.defineDoubleParam( kParamFilterSigma );
	filterSigma->setLabel		( "Sigma" );
	filterSigma->setDefault		( 1.0 );
	filterSigma->setRange		( 0.0001, 30.0 );
	filterSigma->setDisplayRange	( 0.0001, 4.0 );
	filterSigma->setHint		( "Set the gaussian sigma coefficient." );

	OFX::ChoiceParamDescriptor* outOfImage = desc.defineChoiceParam( kParamFilterOutOfImage );
	outOfImage->setLabel            ( "Out of Image" );
	outOfImage->appendOption        ( kParamFilterOutBlack );
	outOfImage->appendOption        ( kParamFilterOutTransparency );
#ifndef TUTTLE_PRODUCTION
	outOfImage->appendOption        ( kParamFilterOutCopy );
	outOfImage->appendOption        ( kParamFilterOutMirror );
#endif
	outOfImage->setHint             ( "Select the filtering method out of the image." );
	outOfImage->setDefault          ( terry::sampler::eParamFilterOutCopy );
}


}
}
#endif
