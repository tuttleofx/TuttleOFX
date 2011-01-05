#ifndef _TUTTLE_PLUGIN_OFXALLOCATOR_HPP_
#define _TUTTLE_PLUGIN_OFXALLOCATOR_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxsMemory.h>

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <sstream>

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
	pointer allocate( const size_type n, const void* = 0 )
	{
		++size_all;
		TUTTLE_TCOUT( "Use OfxAllocator to allocate" );
		T* t = static_cast<T*>( OFX::memory::allocate( n * sizeof( T ) /*, ImageEffect* handle = 0*/ ) );
		//		T* t = (T*) malloc( n * sizeof(T) );
		TUTTLE_TCOUT( "allocate done (address:" << t << ") (+" << n << ") " << size_all );
		return t;
	}

	void deallocate( void* ptr, size_type )
	{
		--size_all;
		TUTTLE_TCOUT( "Use OfxAllocator to deallocate (address:" << ptr << ") (-)" << size_all );
		OFX::memory::free( ptr );
		//free( ptr );
		TUTTLE_TCOUT( "deallocate done." );
	}

	void construct( pointer p, const T& val ) { new ( (T*) p )T( val ); }

	void destroy( pointer p ) { p->~T(); }

	template <class U>
	struct rebind
	{
		typedef OfxAllocator<U> other;
	};

	OfxAllocator<T>& operator=( const OfxAllocator& ) { return *this; }

	template <class U>
	OfxAllocator& operator=( const OfxAllocator<U>& ) { return *this; }

	pointer       address( reference x ) const       { return &x; }
	const_pointer address( const_reference x ) const { return &x; }

	size_type max_size() const { return size_t( -1 ); }
};

template <typename T>
int OfxAllocator<T>::size_all = 0;

}
}

#endif

