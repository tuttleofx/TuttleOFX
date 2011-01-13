#include "Jpeg2000WriterPlugin.hpp"
#include "Jpeg2000WriterProcess.hpp"
#include "Jpeg2000WriterDefinitions.hpp"

#include <openjpeg/J2KWriter.hpp>

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace writer {

using namespace tuttle::io;
using namespace boost::gil;

Jpeg2000WriterPlugin::Jpeg2000WriterPlugin( OfxImageEffectHandle handle )
: WriterPlugin( handle )
{
	_paramCineProfil = fetchChoiceParam( kParamCinemaProfil );
	_paramLossless = fetchBooleanParam( kParamLossless );
}

Jpeg2000ProcessParams Jpeg2000WriterPlugin::getProcessParams(const OfxTime time)
{
	Jpeg2000ProcessParams params;
	params._filepath = getAbsoluteFilenameAt(time);
	switch( static_cast<EParamBitDepth>(this->_paramBitDepth->getValue()) )
	{
		case eParamBitDepth8:
			params._bitDepth = 8;
			break;
		case eParamBitDepth12:
			params._bitDepth = 12;
			break;
		case eParamBitDepth16:
			params._bitDepth = 16;
			break;
		case eParamBitDepth32:
			params._bitDepth = 32;
			break;
		default:
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite(kOfxStatErrValue, "Incorrect bit depth.") );
			break;
	}
	params._cineProfil = _paramCineProfil->getValue();
	params._lossless = _paramLossless->getValue();
	params._flip = _paramFlip->getValue();

	return params;
}

void Jpeg2000WriterPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	WriterPlugin::changedParam( args, paramName );
	if( paramName == kParamCinemaProfil && args.reason == OFX::eChangeUserEdit )
	{
		if (_paramCineProfil->getValue() != 0)
		{
			_paramLossless->setEnabled(false);
			// DCI needs 12b
			_paramBitDepth->setValue(1);
			_paramBitDepth->setEnabled(false);
		}
		else
		{
			_paramLossless->setEnabled(true);
			_paramBitDepth->setEnabled(true);
		}
	}
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Jpeg2000WriterPlugin::render( const OFX::RenderArguments &args )
{
	WriterPlugin::render( args );
	
	doGilRender<Jpeg2000WriterProcess>( *this, args );
}

}
}
}
}
