%include <tuttle/host/global.i>

%{
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/INode.hpp>
%}

%include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {

%extend INode
{
	%pythoncode
	{
		def setParamValues(self, *orderedParams, **namedParams):
			for index, paramValue in enumerate(orderedParams):
				self.getParam(index).setValue(paramValue)
			for paramName, paramValue in namedParams.items():
				self.getParam(paramName).setValue(paramValue)
			return self
	}
}

}
}
