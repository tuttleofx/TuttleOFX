#include "OfxhPluginHandle.hpp"
#include "OfxhPlugin.hpp"
#include "OfxhPluginBinary.hpp"

#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace host {
namespace ofx {

OfxhPluginHandle::OfxhPluginHandle( OfxhPlugin& p, OfxhHost& host ) : _p( &p )
{
	_b = &_p->getBinary();
	_b->_binary.ref();
	_op                          = 0;
	OfxPlugin* ( *getPlug )(int) = ( OfxPlugin * ( * )( int ) )_b->_binary.findSymbol( "OfxGetPlugin" );
	_op                          = getPlug( _p->getIndex() );
	if( !_op )
	{
		BOOST_THROW_EXCEPTION( exception::Data()
		    << exception::user( "Loading plugin failed." )
		    << exception::dev() + "OfxGetPlugin call failed at index " + _p->getIndex() + "."
		    << exception::filename( _b->getBundlePath() ) );
	}
	_op->setHost( host.getHandle() );
}

OfxhPluginHandle::~OfxhPluginHandle()
{
	_b->_binary.unref();
}

}
}
}

