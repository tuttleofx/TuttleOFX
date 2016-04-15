%include <tuttle/host/global.i>
%include <tuttle/host/ofx/attribute/OfxhAttribute.i>
%include <tuttle/host/ofx/attribute/OfxhParamAccessor.i>

%include <ofxCore.i>

%include <std_string.i>
%include <std_vector.i>

namespace std {
%template(BoolVector) vector<bool>;
%template(IntVector) vector<int>;
%template(DoubleVector) vector<double>;
%template(StringVector) vector<string>;
}

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
	ofx::property::OfxhProperty& __getitem__(const std::string& name)
	{
		return self->getEditableProperties().fetchLocalProperty(name);
	}

	std::string __str__() const
	{
		std::stringstream s;

		s << *self;
		return s.str();
	}

	%pythoncode
	{
		def setValueAtTime(self, time, value, change = eChangeUserEdited):
			if isinstance(value, list) or isinstance(value, tuple):
				if len(value) != self.getSize():
					raise ValueError( ("The number of values (%d) doesn't match with the parameter size (%d).") % (len(value), self.getSize()) )
				for i, v in enumerate(value):
					self.setValueAtTimeAndIndex(time, i, v, eChangeNone)
				if change != eChangeNone:
					self.paramChanged(change)
			else:
				self.private_setValueAtTime(time, value, change)
		
		def setValueAtIndex(self, index, value, change = eChangeUserEdited):
			if isinstance(value, dict):
				for time, v in value.items():
					self.setValueAtTimeAndIndex(time, index, v, eChangeNone)
				if change != eChangeNone:
					self.paramChanged(change)
			else:
				self.private_setValueAtIndex(index, value, change)
		
		def setValue(self, value, change = eChangeUserEdited):
			if isinstance(value, list) or isinstance(value, tuple):
				if len(value) != self.getSize():
					raise ValueError( ("The number of values (%d) doesn't match with the parameter size (%d).") % (len(value), self.getSize()) )
				for i, v in enumerate(value):
					self.setValueAtIndex(i, v, eChangeNone)
				if change != eChangeNone:
					self.paramChanged(change)
			elif isinstance(value, dict):
				for time, v in value.items():
					self.setValueAtTime(time, v, eChangeNone)
				if change != eChangeNone:
					self.paramChanged(change)
			else:
				self.private_setValue(value, change)
	}
}
