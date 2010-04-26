#include <ofxsImageEffect.h>
#include "reader/Jpeg2000ReaderPluginFactory.hpp"
#include "writer/Jpeg2000WriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::jpeg2000::reader::Jpeg2000ReaderPluginFactory p1( "fr.duranduboi.duplo.jpeg2000reader", 1, 0 );
	ids.push_back( &p1 );
	static tuttle::plugin::jpeg2000::writer::Jpeg2000WriterPluginFactory p2( "fr.duranduboi.duplo.jpeg2000writer", 1, 0 );
	ids.push_back( &p2 );
}

}
}
