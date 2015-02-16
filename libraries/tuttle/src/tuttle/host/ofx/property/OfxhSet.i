%include <tuttle/host/global.i>
%include <tuttle/host/ofx/property/OfxhProperty.i>
%include <tuttle/host/ofx/property/OfxhPropertyTemplate.i>

%{
#include <tuttle/host/ofx/property/OfxhSet.hpp>
%}

%include <tuttle/host/ofx/property/OfxhSet.hpp>


namespace tuttle {
namespace host {
namespace ofx {
namespace property {


%extend OfxhSet
{
	std::string __str__() const
	{
		std::stringstream s;

		s << *self;
		return s.str();
	}
	
	%pythoncode
	{
		def __len__(self):
			return self.getSize()

		def __getitem__(self, index_or_name):
			if isinstance(index_or_name, str):
				return self.fetchProperty(index_or_name)

			if index_or_name >= self.getSize():
				raise IndexError("list index out of range")
			return self.at(index_or_name)
	}
}

}
}
}
}
