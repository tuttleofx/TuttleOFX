#include "macos.hpp"

#ifdef __MACOS__

#include <string>


namespace tuttle {
namespace common {

std::string CFStringContainer::str( CFStringRef cfString )
{
	if( !cfString )
		return std::string();

	CFIndex length = CFStringGetLength( cfString );
	if( length == 0 )
		return std::string();

	std::string s( length );
	CFStringGetCharacters( cfString, CFRangeMake( 0, length ), reinterpret_cast<UniChar *> ( const_cast<QChar *> ( s.unicode() ) ) );

	return string;
}

CFStringContainer::operator std::string() const
{
	if( string.isEmpty() && type )
		const_cast<CFStringContainer*>(this)->string = toQString( type );
	return string;
}

CFStringRef CFStringContainer::toCFStringRef( const std::string& s )
{
	return CFStringCreateWithCharacters( 0, reinterpret_cast<const UniChar *> ( s.unicode() ),
										 s.size() );
}

CFStringContainer::operator CFStringRef() const
{
	if( !_type )
	{
		const_cast<CFStringContainer*> ( this )->_type =
			CFStringCreateWithCharactersNoCopy( 0,
												reinterpret_cast<const UniChar *> ( _string.unicode() ),
												_string.size(),
												kCFAllocatorNull );
	}
	return type;
}


}
}

#endif
