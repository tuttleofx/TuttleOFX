/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "OfxhBinary.hpp"
#include "tuttle/host/exceptionsHost.hpp"

namespace tuttle {
namespace host {
namespace ofx {

OfxhBinary::OfxhBinary()
	: _binaryPath()
	, _invalid( false )
	, _dlHandle( NULL )
	, _exists( false )
	, _time( 0 )
	, _size( 0 )
	, _users( 0 )
{}

OfxhBinary::OfxhBinary( const std::string& binaryPath )
	: _binaryPath( binaryPath )
	, _invalid( false )
	, _dlHandle( NULL )
	, _exists( false )
	, _time( 0 )
	, _size( 0 )
	, _users( 0 )
{
	init( binaryPath );
}

void OfxhBinary::init( const std::string& binaryPath )
{
	struct stat sb;

	if( stat( binaryPath.c_str(), &sb ) != 0 )
	{
		_invalid = true;
	}
	else
	{
		_time = sb.st_mtime;
		_size = sb.st_size;
	}
}

// actually open the binary.
void OfxhBinary::load()
{
	if( _invalid )
		return;

	#if defined ( UNIX )
	_dlHandle = dlopen( _binaryPath.c_str(), RTLD_LAZY );
	#else
	//std::cout << "LoadLibrary" << _binaryPath << std::endl;
	_dlHandle = LoadLibrary( _binaryPath.c_str() );
	//std::cout << "_dlHandle: " << _dlHandle << std::endl;
	#endif
	if( _dlHandle == 0 )
	{
		_invalid = true;
		#if defined ( UNIX )
			BOOST_THROW_EXCEPTION( exception::File()
				<< exception::user() + "Couldn't open library because " + quotes(dlerror())
				<< exception::filename( _binaryPath ) );
		#else
			LPVOID lpMsgBuf = NULL;
			DWORD err       = GetLastError();

			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
						   FORMAT_MESSAGE_FROM_SYSTEM |
						   FORMAT_MESSAGE_IGNORE_INSERTS,
						   NULL,
						   err,
						   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
						   (LPTSTR) &lpMsgBuf,
						   0, NULL );
			exception::user userMsg;
			userMsg + "Couldn't open library because " + quotes((char*)lpMsgBuf) + " was returned";
			if( lpMsgBuf != NULL )
			{
				LocalFree( lpMsgBuf );
			}
			BOOST_THROW_EXCEPTION( exception::File()
				<< userMsg
				<< exception::filename( _binaryPath ) );
		#endif
	}
}

/// close the binary
void OfxhBinary::unload()
{
	if( _dlHandle != 0 )
	{
		#if defined ( UNIX )
		dlclose( _dlHandle );
		#elif defined ( WINDOWS )
		FreeLibrary( _dlHandle );
		#endif
		_dlHandle = 0;
	}
}

/// look up a symbol in the binary file and return it as a pointer.
/// returns null pointer if not found.
void* OfxhBinary::findSymbol( const std::string& symbol )
{
	if( _invalid || _dlHandle == 0 )
	{
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user() + "Error while loading plugin."
			<< exception::filename( _binaryPath )
			<< exception::dev() + "Can't search symbol " + quotes( symbol ) + " (invalid:" + _invalid + ", dlHandle:" + _dlHandle + ")." );
	}
	#if defined( UNIX )
		return dlsym( _dlHandle, symbol.c_str() );
	#elif defined ( WINDOWS )
		return (void*)GetProcAddress( _dlHandle, symbol.c_str() );
	#endif
}

void OfxhBinary::ref()
{
	if( _users == 0 )
	{
		load();
	}
	++_users;
}

void OfxhBinary::unref()
{
	--_users;
	if( _users == 0 )
	{
		unload();
	}
	if( _users < 0 )
	{
		_users = 0;
	}
}

}
}
}
