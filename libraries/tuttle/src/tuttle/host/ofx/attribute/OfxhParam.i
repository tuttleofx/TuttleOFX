%include <tuttle/host/global.i>
%include <tuttle/host/ofx/attribute/OfxhAttribute.i>

typedef double OfxTime;
%{
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>
%}

// rename the original function to reimplement it in python
%rename(private_setValue) setValue;
%rename(private_setValueAtTime) setValueAtTime;

%include <tuttle/host/ofx/attribute/OfxhParam.hpp>

%extend tuttle::host::ofx::attribute::OfxhParam
{
	%pythoncode
	{
		def setValueAtTime(self, time, value, change = eChangeUserEdited):
			if isinstance(value, list) or isinstance(value, tuple):
				args = value[:]
				args.append(change)
				self.private_setValueAtTime(time, *args)
			else:
				self.private_setValueAtTime(time, value, change)
			
		def setValue(self, value, change = eChangeUserEdited):
			if isinstance(value, list) or isinstance(value, tuple):
				args = value[:]
				args.append(change)
				self.private_setValue(*args)
			elif isinstance(value, dict):
				for time, v in value.iteritems():
					self.setValueAtTime(time, v, change)
			else:
				self.private_setValue(value, change)
	}
}
