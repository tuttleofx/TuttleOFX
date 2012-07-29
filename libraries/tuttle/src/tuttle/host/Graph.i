%include <tuttle/host/global.i>
%include <tuttle/host/memory/MemoryCache.i>
%include <tuttle/host/ComputeOptions.i>

%include <std_vector.i>
%include <std_list.i>
%include <std_string.i>

%{
#include <tuttle/host/Graph.hpp>
%}

namespace std {
%template(StringVector) vector<string>;
%template(StringList) list<string>;
%template(NodePtrVector) vector<tuttle::host::Graph::Node*>;
%template(NodePtrList) list<tuttle::host::Graph::Node*>;
}

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
