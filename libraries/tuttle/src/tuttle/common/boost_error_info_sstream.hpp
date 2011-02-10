#ifndef _BOOST_ERROR_INFO_SSTREAM_HPP
#define	_BOOST_ERROR_INFO_SSTREAM_HPP

#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/version.hpp>

#include <sstream>
#include <string>

#ifndef SWIG
namespace boost {

struct error_info_sstream
{
	typedef std::ostringstream value_type;
	value_type _v;
};

inline std::string to_string( const error_info_sstream& x )
{
	return x._v.str();
}

template<class Tag>
class error_info<Tag, error_info_sstream>: public exception_detail::error_info_base
{
public:
	typedef error_info_sstream T;
	typedef error_info<Tag, T> This;
	typedef T value_type;

	error_info() {}
	error_info( const This& v )
	{
		_value._v << v._value._v.str();
	}

	template<typename V>
	error_info( const V& value )
	{
		_value._v << value;
	}

	~error_info() throw( ) {}

	template<typename V>
	This& operator+( const V& v )
	{
		_value._v << v;
		return *this;
	}

	const value_type& value() const { return _value; }
	value_type&       value()       { return _value; }

private:
	#if( BOOST_VERSION >= 104300 )
	std::string tag_typeid_name() const { return tag_type_name<Tag>(); }
	#else
	char const* tag_typeid_name() const { return tag_type_name<Tag>(); }
	#endif
	std::string value_as_string() const { return _value._v.str(); }

	value_type _value;
};
}
#endif


#endif

