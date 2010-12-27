#ifndef _TUTTLE_PLUGIN_CONVOLUTION_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONVOLUTION_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <boost/numeric/ublas/matrix.hpp>

#include <vector>

namespace tuttle {
namespace plugin {
namespace convolution {

struct ConvolutionProcessParams
{
	boost::gil::point2<unsigned int> _size;
	boost::numeric::ublas::matrix<double> _convMatrix;
};

/**
 * @brief Convolution plugin
 */
class ConvolutionPlugin : public ImageEffectGilPlugin
{
public:
	ConvolutionPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	ConvolutionProcessParams getProcessParams() const;

public:
	OFX::Int2DParam* _paramSize;
	std::vector<std::vector<OFX::DoubleParam*> > _paramCoef;
};

}
}
}

#endif
