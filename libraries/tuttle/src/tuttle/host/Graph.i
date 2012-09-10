%include <tuttle/host/global.i>
%include <tuttle/host/memory/MemoryCache.i>
%include <tuttle/host/InputBufferWrapper.i>
%include <tuttle/host/OutputBufferWrapper.i>
%include <tuttle/host/ComputeOptions.i>
%include <tuttle/host/NodeListArg.i>
%include <tuttle/host/INode.i>


%{
#include <tuttle/host/Graph.hpp>
%}

// rename the original "connect" function to reimplement it in python
%rename(private_connect) connect;

%include <tuttle/host/Graph.hpp>

%extend tuttle::host::Graph
{
	%pythoncode
	{
		def connect(self, *args):
			if not args:
				raise RuntimeException("Nothing in list of nodes to connect.")

			if isinstance(args[0], list) or isinstance(args[0], tuple):
				for a in args:
					self.connect(*a)
				return

			if len(args) < 2:
				raise RuntimeException("Needs multiple nodes to connect them together.")

			for nA, nB in zip(args[:-1], args[1:]):
				self.private_connect( nA, nB )
	}
}
