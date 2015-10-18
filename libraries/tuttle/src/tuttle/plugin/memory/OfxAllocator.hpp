#ifndef _TUTTLE_PLUGIN_OFXALLOCATOR_HPP_
#define _TUTTLE_PLUGIN_OFXALLOCATOR_HPP_

#include <tuttle/plugin/global.hpp>

#include <ofxsMemory.h>

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <limits>
namespace tuttle {
namespace plugin {

template <typename T>
class OfxAllocator
{
static int size_all;

public:
	typedef T Type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

public:
	OfxAllocator() {}
	OfxAllocator( const OfxAllocator& ) {}
	template <class U>
	OfxAllocator( const OfxAllocator<U>& ) {}

public:
	inline pointer allocate( const size_type n, const void* = 0 )
	{
		++size_all;
		//TUTTLE_LOG_TRACE( "Use OfxAllocator to allocate" );
		T* t = static_cast<T*>( OFX::memory::allocate( n * sizeof( T ) /*, ImageEffect* handle = 0*/ ) );
		//		T* t = (T*) malloc( n * sizeof(T) );
		//TUTTLE_LOG_TRACE( "allocate done (address:" << t << ") (+" << n << ") " << size_all );
		return t;
	}

	inline void deallocate( void* ptr, size_type )
	{
		--size_all;
		//TUTTLE_LOG_TRACE( "Use OfxAllocator to deallocate (address:" << ptr << ") (-)" << size_all );
		OFX::memory::free( ptr );
		//free( ptr );
		//TUTTLE_LOG_TRACE( "deallocate done." );
	}

	inline void construct( pointer p, const T& val ) { new ( (T*) p )T( val ); }

	inline void destroy( pointer p ) { p->~T(); }

	template <class U>
	struct rebind
	{
		typedef OfxAllocator<U> other;
	};

	inline OfxAllocator<T>& operator=( const OfxAllocator<T>& ) { return *this; }

	template <class U>
	inline OfxAllocator<T>& operator=( const OfxAllocator<U>& ) { return *this; }

	inline pointer       address( reference x ) const       { return &x; }
	inline const_pointer address( const_reference x ) const { return &x; }

	inline size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }

	inline bool operator==( OfxAllocator<T> const& ) { return true; }
	inline bool operator!=( OfxAllocator<T> const& a ) { return !operator==(a); }

};

template <typename T>
int OfxAllocator<T>::size_all = 0;

}
}

#endif

