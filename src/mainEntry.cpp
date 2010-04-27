#include <tuttle/plugin/Plugin.hpp>
#include "reader/Jpeg2000ReaderPluginFactory.hpp"
#include "writer/Jpeg2000WriterPluginFactory.hpp"

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	//mAppendPluginFactory( ids, tuttle::plugin::pushPixel::PushPixelPluginFactory, "fr.tuttle.duranduboi.pushpixel" );
	mAppendPluginFactoryVersions( ids,
								  tuttle::plugin::jpeg2000::reader::Jpeg2000ReaderPluginFactory,
								  "fr.tuttle.duranduboi.jpeg2000reader",
								  1, 0 );
	mAppendPluginFactoryVersions( ids,
								  tuttle::plugin::jpeg2000::writer::Jpeg2000WriterPluginFactory,
								  "fr.tuttle.duranduboi.jpeg2000writer",
								  1, 0 );
}

}
}
