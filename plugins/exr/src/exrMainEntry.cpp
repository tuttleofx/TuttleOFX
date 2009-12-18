#include <ofxsImageEffect.h>
#include "reader/EXRReaderPluginFactory.hpp"
#include "writer/EXRWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::exr::reader::EXRReaderPluginFactory p1( "fr.hd3d.tuttle.exrreader", 1, 0 );
	ids.push_back( &p1 );
	static tuttle::plugin::exr::writer::EXRWriterPluginFactory p2( "fr.hd3d.tuttle.exrwriter", 1, 0 );
	ids.push_back( &p2 );
}

}
}
