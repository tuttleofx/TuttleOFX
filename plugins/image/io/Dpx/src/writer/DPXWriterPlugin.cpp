#include "DPXWriterPlugin.hpp"
#include "DPXWriterProcess.hpp"
#include "DPXWriterDefinitions.hpp"

#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/filesystem.hpp>

#include <vector>


namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

using namespace boost::gil;

DPXWriterPlugin::DPXWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_bitDepth       = fetchChoiceParam( kTuttlePluginBitDepth );
	_descriptor     = fetchChoiceParam( kTuttlePluginComponents );
	_transfer       = fetchChoiceParam( kParamTransfer );
	_colorimetric   = fetchChoiceParam( kParamColorimetric );
	_packed         = fetchChoiceParam( kParamPacked );
	_swapEndian     = fetchBooleanParam( kParamSwapEndian );
	_encoding       = fetchChoiceParam( kParamEncoding );
#ifndef TUTTLE_PRODUCTION
	_orientation    = fetchChoiceParam( kParamOrientation );
#endif
	_project        = fetchStringParam( kParamProject );
	_copyright      = fetchStringParam( kParamCopyright );
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

	::tuttle::plugin::OfxAllocator<char> data;

	ETuttlePluginBitDepth eOutBitDepth = static_cast<ETuttlePluginBitDepth>( this->_paramBitDepth->getValue() );
	OFX::EBitDepth        eOfxBitDepth = _clipSrc->getPixelDepth();
	OFX::EPixelComponent  components   = _clipSrc->getPixelComponents();
	OfxPointI             size         = _clipSrc->getPixelRodSize( args.time );

	::dpx::Descriptor     eDescriptor  = ::dpx::kUndefinedDescriptor;   ///< Components type
	::dpx::Characteristic eTransfer;
	::dpx::Characteristic eColorimetric;
	::dpx::Packing        ePacked;       ///< Bit streaming packing
	::dpx::Encoding       eEncoding;

	std::string filename = getAbsoluteFilenameAt( args.time );

#ifndef TUTTLE_PRODUCTION
	::dpx::Orientation orientation = static_cast< ::dpx::Orientation >( _orientation->getValue() );
#endif

	::dpx::Writer   writer;
	::dpx::DataSize dataSize;

	OutStream       stream;

	if( ! stream.Open( filename.c_str() ) )
	{
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user( "Dpx: Unable to open output file" ) );
	}

	writer.SetOutStream( &stream );

	writer.Start();

	writer.SetFileInfo( filename.c_str(), 0, "TuttleOFX DPX Writer", _project->getValue().c_str(), _copyright->getValue().c_str(), ~0, _swapEndian->getValue() );

	writer.SetImageInfo( size.x, size.y );

#ifndef TUTTLE_PRODUCTION
	writer.header.SetImageOrientation( orientation );
#endif

	int iBitDepth = 0;
	int pixelSize = 0;
	std::string inputComponentString = "unknown";

	switch ( eOfxBitDepth )
	{
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			BOOST_THROW_EXCEPTION( exception::BitDepthMismatch()
				<< exception::user( "Dpx: Unable to compute custom or non bit depth" ) );
			break;
		case OFX::eBitDepthUByte:  dataSize = ::dpx::kByte;  pixelSize = 1; break;
		case OFX::eBitDepthUShort: dataSize = ::dpx::kWord;  pixelSize = 2; break;
		case OFX::eBitDepthFloat:  dataSize = ::dpx::kFloat; pixelSize = 4; break;
	}
	switch( components )
	{
		case OFX::ePixelComponentAlpha: inputComponentString = "Gray/Alpha"; break; // pixelSize *= 1;
		case OFX::ePixelComponentRGB  : inputComponentString = "RGB";  pixelSize *= 3; break;
		case OFX::ePixelComponentRGBA : inputComponentString = "RGBA"; pixelSize = 4;  break;
		default: break;
	}
	switch ( eOutBitDepth )
	{
		case eTuttlePluginBitDepth8:  iBitDepth = 8;  break;
		case eTuttlePluginBitDepth10: iBitDepth = 10; break;
		case eTuttlePluginBitDepth12: iBitDepth = 12; break;
		case eTuttlePluginBitDepth16: iBitDepth = 16; break;
		case eTuttlePluginBitDepth32: iBitDepth = 32; break;
		case eTuttlePluginBitDepth64: iBitDepth = 64; break;
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
		case 24:
			switch( components )
			{
				case OFX::ePixelComponentAlpha: eDescriptor = ::dpx::kLuma; break;
				case OFX::ePixelComponentRGB:  eDescriptor = ::dpx::kRGB; break;
				case OFX::ePixelComponentRGBA: eDescriptor = ::dpx::kRGBA;break;
				default: BOOST_THROW_EXCEPTION( exception::ImageFormat()
						<< exception::user( "Dpx: Unable to write CbYCr channels (input is " + inputComponentString + ")." ) );
						break;
			}
			break;
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
			iBitDepth,
			eTransfer,
			eColorimetric,
			ePacked,
			eEncoding );

	if( ! writer.WriteHeader() )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX Header)" ) );
	}

	char* dataPtr = data.allocate( size.x * size.y * pixelSize );
	char* dataPtrIt = dataPtr;

	for( int y = size.y; y > 0 ; y-- )
	{
		void* dataSrcPtr = _clipSrc->fetchImage( args.time )->getPixelAddress( 0, y );
		memcpy( dataPtrIt, dataSrcPtr, size.x * pixelSize );

		dataPtrIt += size.x * pixelSize;
	}
;
	if( ! writer.WriteElement( 0, dataPtr, ::dpx::kByte ) )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX User Data)" ) );
	}

	if( ! writer.Finish() )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
			<< exception::user( "Dpx: Unable to write data (DPX finish)" ) );
	}

	data.deallocate( dataPtr, 0 );
}

void DPXWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
}

}
}
}
}
