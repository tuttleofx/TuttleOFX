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
	const OfxhProperty& __getitem__( const std::string& name ) const
	{
		return self->fetchProperty( name );
	}
	const OfxhProperty& __getitem__( int index ) const
	{
		return self->at( index );
	}

	std::string __str__() const
	{
		std::stringstream s;

		s << *self;
		return s.str();
	}
	
	%pythoncode
	{
		def getProperties(self):
			nb = self.getSize()
			return [self.at(i) for i in range(nb)]
	}
}

}
}
}
}
