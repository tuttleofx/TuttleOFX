#include "ThreadEnv.hpp"

#include "Graph.hpp"
#include "ComputeOptions.hpp"

namespace tuttle {
namespace host {

void ThreadEnv::runProcessFunc( ThreadEnv* threadEnv, Graph& graph, const std::list<std::string>& nodes )
{
	threadEnv->setResult( graph.compute( threadEnv->_imageCache, nodes, threadEnv->_options ) );
	threadEnv->setIsRunning(false);
	threadEnv->getSignalEnd()();
}

void ThreadEnv::compute( Graph& graph, const NodeListArg& nodes )
{
	if( _asynchronous )
	{
		setIsRunning(true);
		_thread = boost::thread( runProcessFunc, this, graph, nodes.getNodes() );
	}
	else
	{
		setResult( graph.compute( _imageCache, nodes, _options ) );
	}
}

}
}
