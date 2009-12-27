#include <ofxsImageEffect.h>
#include "reader/DPXReaderPluginFactory.hpp"
#include "writer/DPXWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::dpx::reader::DPXReaderPluginFactory p1( "fr.hd3d.tuttle.dpxreader", 1, 0 );

	ids.push_back( &p1 );
	static tuttle::plugin::dpx::writer::DPXWriterPluginFactory p2( "fr.hd3d.tuttle.dpxwriter", 1, 0 );
	ids.push_back( &p2 );
}

}
}
