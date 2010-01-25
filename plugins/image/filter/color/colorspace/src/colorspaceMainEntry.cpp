#include <ofxsImageEffect.h>
#include "lin2log/Lin2LogPluginFactory.hpp"
#include "log2lin/Log2LinPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::colorspace::lin2log::Lin2LogPluginFactory p1( "fr.hd3d.tuttle.lin2log", 1, 0 );
	ids.push_back( &p1 );
	static tuttle::plugin::colorspace::log2lin::Log2LinPluginFactory p2( "fr.hd3d.tuttle.log2lin", 1, 0 );
	ids.push_back( &p2 );
}

}
}
