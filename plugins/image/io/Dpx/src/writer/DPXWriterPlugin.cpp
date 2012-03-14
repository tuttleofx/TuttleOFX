#include "DPXWriterPlugin.hpp"
#include "DPXWriterProcess.hpp"
#include "DPXWriterDefinitions.hpp"

#include <boost/filesystem.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

using namespace boost::gil;

DPXWriterPlugin::DPXWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_bitDepth       = fetchChoiceParam( kTuttlePluginBitDepth );
	_descriptor     = fetchChoiceParam( kParamDescriptor );
	_transfer       = fetchChoiceParam( kParamTransfer );
	_colorimetric   = fetchChoiceParam( kParamColorimetric );
	_packed         = fetchChoiceParam( kParamPacked );
	_encoding       = fetchChoiceParam( kParamEncoding );
}

DPXWriterProcessParams DPXWriterPlugin::getProcessParams( const OfxTime time )
{

	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DPXWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );

	ETuttlePluginBitDepth eOutBitDepth = static_cast<ETuttlePluginBitDepth>( this->_paramBitDepth->getValue() );
	OFX::EBitDepth        eOfxBitDepth = _clipSrc->getPixelDepth();

	::dpx::Descriptor     eDescriptor  = ::dpx::kUndefinedDescriptor;   ///< Components type
	::dpx::Characteristic eTransfer;
	::dpx::Characteristic eColorimetric;
	::dpx::Packing        ePacked;       ///< Bit streaming packing
	::dpx::Encoding       eEncoding;

	std::string filename = this->_paramFilepath->getValue();

	::dpx::Writer   writer;
	::dpx::DataSize dataSize;

	OutStream       stream;


	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	if( ! stream.Open( filename.c_str() ) )
	{
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user( "Dpx: Unable to open output file" ) );
	}

	writer.SetOutStream( &stream );

	writer.Start();

	writer.SetFileInfo( filename.c_str(), 0, "TuttleOFX DPX Writer", "project", "copyright", ~0, true );

	OfxPointI size = _clipSrc->getPixelRodSize( args.time );

	writer.SetImageInfo( size.x, size.y );

	int ibitDepth = 0;

	switch ( eOfxBitDepth )
	{
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			BOOST_THROW_EXCEPTION( exception::BitDepthMismatch()
				<< exception::user( "Dpx: Unable to compute custom or non bit depth" ) );
			break;
		case OFX::eBitDepthUByte:  dataSize = ::dpx::kByte; break;
		case OFX::eBitDepthUShort: dataSize = ::dpx::kWord; break;
		case OFX::eBitDepthFloat:  dataSize = ::dpx::kFloat; break;
	}

	switch ( eOutBitDepth )
	{
		case eTuttlePluginBitDepth8:  ibitDepth = 8;  break;
		case eTuttlePluginBitDepth10: ibitDepth = 10; break;
		case eTuttlePluginBitDepth12: ibitDepth = 12; break;
		case eTuttlePluginBitDepth16: ibitDepth = 16; break;
		case eTuttlePluginBitDepth32: ibitDepth = 32; break;
		case eTuttlePluginBitDepth64: ibitDepth = 64; break;
	}

	std::string inputComponentString = "unknown";

	switch( components )
	{
		case OFX::ePixelComponentAlpha: inputComponentString = "Gray/Alpha"; break;
		case OFX::ePixelComponentRGB  : inputComponentString = "RGB"; break;
		case OFX::ePixelComponentRGBA : inputComponentString = "RGBA"; break;
		default: break;
	}

	switch( _descriptor->getValue() )
	{
		case 0:
			switch( components )
			{
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write user defined" ) );
						break;
			}
			eDescriptor = ::dpx::kUserDefinedDescriptor; break;
		case 1:
			switch( components )
			{
				case OFX::ePixelComponentAlpha: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write Red channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kRed; break;
		case 2:
			switch( components )
			{
				case OFX::ePixelComponentAlpha: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write Green channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kGreen; break;
		case 3:
			switch( components )
			{
				case OFX::ePixelComponentAlpha: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write Blue channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kBlue; break;
		case 4:
			switch( components )
			{
				case OFX::ePixelComponentAlpha: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write Alpha channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kAlpha; break;
		case 5:
			switch( components )
			{
				case OFX::ePixelComponentAlpha: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write Luma channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kLuma; break;
		case 6:
			switch( components )
			{
				case OFX::ePixelComponentAlpha: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write ColorDifference channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kColorDifference; break;
		case 7:
			switch( components )
			{
				case OFX::ePixelComponentAlpha: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write Depth channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kDepth; break;
		case 8:
			switch( components )
			{
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kCompositeVideo; break;
		case 9:
			switch( components )
			{
				case OFX::ePixelComponentRGB: break;
						break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write RGB channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kRGB; break;
		case 10:
			switch( components )
			{
				case OFX::ePixelComponentRGBA: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write RGBA channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kRGBA; break;
		case 11:

			switch( components )
			{
				case OFX::ePixelComponentRGBA: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write ABGR channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kABGR; break;
		case 12:
			switch( components )
			{
				case OFX::ePixelComponentRGB: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write CbYCrY channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kCbYCrY; break;
		case 13:
			switch( components )
			{
				case OFX::ePixelComponentRGBA: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write CbYCrYA channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kCbYACrYA; break;
		case 14:
			switch( components )
			{
				case OFX::ePixelComponentRGB: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write CbYCr channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kCbYCr; break;
		case 15:
			switch( components )
			{
				case OFX::ePixelComponentRGBA: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write CbYCrA channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kCbYCrA; break;
		case 16:
			switch( components )
			{
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write 2 channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kUserDefined2Comp; break;
		case 17:
			switch( components )
			{
				case OFX::ePixelComponentRGB: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write 3 channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kUserDefined3Comp; break;
		case 18:
			switch( components )
			{
				case OFX::ePixelComponentRGBA: break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write 4 channel (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kUserDefined4Comp; break;
		case 19:
			switch( components )
			{
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write 5 channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kUserDefined5Comp; break;
		case 20:
			switch( components )
			{
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write 6 channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kUserDefined6Comp; break;
		case 21:
			switch( components )
			{
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write 7 channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kUserDefined7Comp; break;
		case 22:
			switch( components )
			{
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write 8 channels (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kUserDefined8Comp; break;
		case 23:
			switch( components )
			{
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write undefined descriptor (input is " + inputComponentString + ")." ) );
						break;
			}
			eDescriptor = ::dpx::kUndefinedDescriptor; break;
	}

	switch( _transfer->getValue() )
	{
		case 13 : eTransfer = ::dpx::kUndefinedCharacteristic; break;
		default : eTransfer = static_cast< ::dpx::Characteristic >( _transfer->getValue() ); break;
	}
	switch( _colorimetric->getValue() )
	{
		case 13 : eColorimetric = ::dpx::kUndefinedCharacteristic; break;
		default : eColorimetric = static_cast< ::dpx::Characteristic >( _colorimetric->getValue() ); break;
	}
	ePacked   = static_cast< ::dpx::Packing > ( _packed->getValue() );
	eEncoding = static_cast< ::dpx::Encoding >( _encoding->getValue() );

	writer.SetElement( 0,
			eDescriptor,
			ibitDepth,
			eTransfer,
			eColorimetric,
			ePacked,
			eEncoding );

	if( ! writer.WriteHeader() )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX Header)" ) );
	}

	if( ! writer.WriteElement( 0, _clipSrc->fetchImage( args.time )->getPixelData(), ::dpx::kByte ) )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX User Data)" ) );
	}

	if( ! writer.Finish() )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX finish)" ) );
	}
}

void DPXWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
}

}
}
}
}
