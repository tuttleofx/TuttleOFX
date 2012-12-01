%include <tuttle/host/global.i>
%include <tuttle/host/ofx/attribute/OfxhAttribute.i>

%include <ofxCore.i>

%include <std_vector.i>

%{
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>
%}

// rename the original function to reimplement it in python
%rename(private_setValue) setValue;
%rename(private_setValueAtTime) setValueAtTime;
%rename(private_setValueAtIndex) setValueAtIndex;

%include <tuttle/host/ofx/attribute/OfxhParam.hpp>

%extend tuttle::host::ofx::attribute::OfxhParam
{
	%pythoncode
	{
		def setValueAtTime(self, time, value):
			if isinstance(value, list) or isinstance(value, tuple):
				self.private_setValueAtTime(time, *value)
			else:
				self.private_setValueAtTime(time, value)
		
		def setValueAtIndex(self, index, value):
			if isinstance(value, list) or isinstance(value, tuple):
				self.private_setValueAtIndex(index, *value)
			else:
				self.private_setValueAtIndex(index, value)
		
		def setValue(self, value):
			if isinstance(value, list) or isinstance(value, tuple):
				self.private_setValue(*value)
			elif isinstance(value, dict):
				for time, v in value.iteritems():
					self.setValueAtTime(time, v)
			else:
				self.private_setValue(value)
	}
}
