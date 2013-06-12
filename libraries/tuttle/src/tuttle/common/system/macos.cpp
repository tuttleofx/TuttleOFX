#include "macos.hpp"

#ifdef __MACOS__

#include <string>


namespace tuttle {
namespace common {

std::string CFStringContainer::toString( CFStringRef cfString )
{
	if( !cfString )
		return std::string();

	CFIndex length = CFStringGetLength( cfString );
	if( length == 0 )
		return std::string();

	std::string s;
	s.resize( length );
	// std::wstring s( length );
	CFStringGetCharacters( cfString, CFRangeMake( 0, length ), reinterpret_cast<UniChar *>( const_cast<char*>( s.c_str() ) ) );

	return s;
}

CFStringContainer::operator std::string() const
{
	return str();
}

const std::string& CFStringContainer::str() const
{
	if( _string.empty() && _type )
		const_cast<CFStringContainer*>(this)->_string = toString(_type);
	return _string;
}

CFStringRef CFStringContainer::toCFStringRef( const std::string& s )
{
	return CFStringCreateWithCharacters( 0,
		reinterpret_cast<const UniChar *>( s.c_str() ), s.size() );
}

CFStringContainer::operator CFStringRef() const
{
	if( !_type )
	{
		const_cast<CFStringContainer*> ( this )->_type =
			CFStringCreateWithCharactersNoCopy( 0,
				reinterpret_cast<const UniChar *> ( _string.c_str() ),
				_string.size(),
				kCFAllocatorNull );
	}
	return _type;
}


}
}

#endif
