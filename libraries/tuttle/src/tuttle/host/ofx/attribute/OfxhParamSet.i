%include <tuttle/host/global.i>
%include <tuttle/host/ofx/attribute/OfxhParam.i>

%include <std_vector.i>
%include <std_map.i>
%include <std_string.i>

%{
#include <tuttle/host/ofx/attribute/OfxhParamSet.hpp>
%}

%include <tuttle/host/ofx/attribute/OfxhParamSet.hpp>

%extend tuttle::host::ofx::attribute::OfxhParamSet
{
	%pythoncode
	{
		def getParams(self):
			nb = self.getNbParams()
			return [self.getParam(i) for i in range(nb)]
		def getChildParams(self):
			nb = self.getNbChildParams()
			return [self.getChildParam(i) for i in range(nb)]
	}
}

