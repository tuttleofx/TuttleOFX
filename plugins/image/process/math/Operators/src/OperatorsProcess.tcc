
#include "OperatorsPlugin.hpp"
#include "OperatorsAlgorithm.hpp"

#include <cmath>

namespace tuttle {
namespace plugin {
namespace operators {

template<class View>
OperatorsProcess<View>::OperatorsProcess( OperatorsPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template<class View>
void OperatorsProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void OperatorsProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput;
	
	OperatorsProcessParams<OperatorsPlugin::Scalar> params = _plugin.getProcessParams();
	procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	
	rgba32f_pixel_t valuesPix;
	valuesPix[ 0 ] = params.iROperators ;
	valuesPix[ 1 ] = params.iGOperators ;
	valuesPix[ 2 ] = params.iBOperators ;
	valuesPix[ 3 ] = params.iAOperators ;
	
	rgba8_pixel_t processChannel;
	processChannel[ 0 ] = params.bRProcess;
	processChannel[ 1 ] = params.bGProcess;
	processChannel[ 2 ] = params.bBProcess;
	processChannel[ 3 ] = params.bAProcess;
	
	switch(params.op)
	{
		case eOperatorsOperatorPlus     : processImagePlus      ( procWindowOutput, procWindowRoW, valuesPix, processChannel ); break;
		case eOperatorsOperatorMultiply : processImageMultiply  ( procWindowOutput, procWindowRoW, valuesPix, processChannel ); break;
		case eOperatorsOperatorPow      : processImagePow       ( procWindowOutput, procWindowRoW, valuesPix, processChannel ); break;
		case eOperatorsOperatorSqrt     : processImageSqrt      ( procWindowOutput, procWindowRoW, valuesPix, processChannel ); break;
		case eOperatorsOperatorLog      : processImageLog       ( procWindowOutput, procWindowRoW, valuesPix, processChannel ); break;
		case eOperatorsOperatorLn       : processImageLn        ( procWindowOutput, procWindowRoW, valuesPix, processChannel ); break;
	}

}

}
}
}
