%include <tuttle/host/global.i>
%include <std_vector.i>
%include <std_string.i>

%include <tuttle/host/INode.i>
%include <tuttle/host/ImageEffectNode.i>

%include <tuttle/host/attribute/allParams.i>

%include <factory.i>

%{
#include <tuttle/host/Node.hpp>
%}

namespace std {
%template(NodeInitVector) vector<tuttle::host::NodeInit>;
%template(NodeInitList) list<tuttle::host::NodeInit>;
}

%rename(private_createNode) createNode;
%rename(PrivateNodeInit) NodeInit;

%newobject tuttle::host::createNode;
%factory(tuttle::host::Graph::Node& tuttle::host::createNode, tuttle::host::ImageEffectNode);

%include <tuttle/host/Node.hpp>

%pythoncode
{
	def createNode(pluginName, *orderedParams, **namedParams):
		node = private_createNode(pluginName)
		node.setParamValues(*orderedParams, **namedParams)
		return node
	
	class NodeInit(PrivateNodeInit):
		def __init__(self, pluginName, *orderedParams, **namedParams):
			super(self.__class__, self).__init__(pluginName)
			self.get().setParamValues(*orderedParams, **namedParams)
}
