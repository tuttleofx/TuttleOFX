#include "SamplerPlugin.hpp"
#include <terry/sampler/sampler.hpp>

namespace tuttle {
namespace plugin {

SamplerPlugin::SamplerPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramFilter          = fetchChoiceParam   ( kParamFilter );
	_paramB	              = fetchDoubleParam   ( kParamFilterB );
	_paramC               = fetchDoubleParam   ( kParamFilterC );
	_paramFilterSize      = fetchIntParam      ( kParamFilterSize );
	_paramFilterSigma     = fetchDoubleParam   ( kParamFilterSigma );
	_paramFilterSharpen   = fetchDoubleParam   ( kParamFilterSharpen );
	_paramOutOfImage      = fetchChoiceParam   ( kParamFilterOutOfImage );
}

SamplerPlugin::~SamplerPlugin()
{}

void SamplerPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamFilter )
	{
		if( _paramFilter->getValue() == terry::sampler::eParamFilterBC )
		{
			_paramB->setIsSecretAndDisabled( false );
			_paramC->setIsSecretAndDisabled( false );
		}
		else
		{
			_paramB->setIsSecretAndDisabled( true );
			_paramC->setIsSecretAndDisabled( true );
		}
		if( ( _paramFilter->getValue() == terry::sampler::eParamFilterLanczos ) || ( _paramFilter->getValue() == terry::sampler::eParamFilterGaussian ) )
		{
			_paramFilterSize->setIsSecretAndDisabled( false );
		}
		else
		{
			_paramFilterSize->setIsSecretAndDisabled( true );
		}
		if( _paramFilter->getValue() == terry::sampler::eParamFilterLanczos )
		{
			_paramFilterSharpen->setIsSecretAndDisabled( false );
		}
		else
		{
			_paramFilterSharpen->setIsSecretAndDisabled( true );
		}
		if( _paramFilter->getValue() == terry::sampler::eParamFilterGaussian )
		{
			_paramFilterSigma->setIsSecretAndDisabled( false );
		}
		else
		{
			_paramFilterSigma->setIsSecretAndDisabled( true );
		}
	}
}

void SamplerPlugin::fillProcessParams( SamplerProcessParams& samplerprocessParam ) const
{
	samplerprocessParam._filter            = (terry::sampler::EParamFilter)           _paramFilter     -> getValue();
	samplerprocessParam._filterSize        = (double) _paramFilterSize    -> getValue();
	samplerprocessParam._filterSigma       = (double) _paramFilterSigma   -> getValue();
	samplerprocessParam._filterSharpen     = (double) _paramFilterSharpen -> getValue();
	samplerprocessParam._paramB            = (double) _paramB             -> getValue();
	samplerprocessParam._paramC            = (double) _paramC             -> getValue();
	samplerprocessParam._outOfImageProcess = (terry::sampler::EParamFilterOutOfImage) _paramOutOfImage  -> getValue();
}

}
}
