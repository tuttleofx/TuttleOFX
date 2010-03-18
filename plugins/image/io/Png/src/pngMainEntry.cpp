#include <ofxsImageEffect.h>
#include "reader/PNGReaderPluginFactory.hpp"
#include "writer/PNGWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::png::reader::PNGReaderPluginFactory p1( "fr.tuttle.pngreader", 1, 0 );
	ids.push_back( &p1 );
	static tuttle::plugin::png::writer::PNGWriterPluginFactory p2( "fr.tuttle.pngwriter", 1, 0 );
	ids.push_back( &p2 );
}

}
}
