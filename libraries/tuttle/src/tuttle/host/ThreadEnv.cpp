#include "ThreadEnv.hpp"

#include "Graph.hpp"
#include "ComputeOptions.hpp"

namespace tuttle {
namespace host {

void ThreadEnv::runProcessFunc( ThreadEnv* threadEnv, Graph* graph, const std::list<std::string>& nodes, const ComputeOptions* const options )
{
	threadEnv->setResult( graph->compute( nodes, *options ) );
	threadEnv->getSignalEnd()();
}

void ThreadEnv::compute( Graph& graph, const NodeListArg& nodes )
{
	if( _asynchronous )
	{
		_thread = boost::thread( runProcessFunc, this, &graph, nodes.getNodes(), &_options );
	}
	else
	{
		graph.compute( nodes, _options );
	}
}

}
}
