#include "CTLAlgorithm.hpp"
#include "CTLProcess.hpp"

#include <half.h>
#include <Iex.h>
#include <IlmCtl/CtlMessage.h>
#include <IlmCtlSimd/CtlSimdInterpreter.h>


namespace tuttle {
namespace plugin {
namespace ctl {

namespace {
CTLPlugin* ctlPlugin;

void ctlMessageOutput( const std::string& message )
{
	if( ctlPlugin )
	{
		ctlPlugin->sendMessage( OFX::Message::eMessageMessage, "CTL message", message );
	}
}

void fillInputArg( Ctl::FunctionArgPtr& arg, const std::string& argStr, const half& v, const std::size_t n )
{
	if( !arg ||
//		!arg->type().cast<half>() ||
		!arg->isVarying( ) )
	{
		// The CTL function has no argument argStr, the argument
		// is not of type half, or the argument is not varying
		BOOST_THROW_EXCEPTION( Iex::ArgExc( std::string("Cannot set value of argument ")+argStr ) );
	}

	///@todo copy buffer !
	//memcpy( arg->data(), r, n * sizeof(half) );
}

void retrieveOutputArg( const Ctl::FunctionArgPtr& arg, const std::string& argStr, half& v, const std::size_t n )
{
	if( !arg ||
//		!arg->type( ).cast<half>() ||
		!arg->isVarying( ) )
	{
		// The CTL function has no argument argStr, the argument
		// is not of type half, or the argument is not varying
		BOOST_THROW_EXCEPTION( Iex::ArgExc( std::string("Cannot set value of argument ")+argStr ) );
	}

	///@todo copy buffer !
//	memcpy( v, arg->data(), n*sizeof(half) );
}

void callCtlChunk(
	Ctl::FunctionCallPtr call,
	const std::size_t n,
	half& rOut,
	half& gOut,
	half& bOut,
	half& aOut,
	const half& r,
	const half& g,
	const half& b,
	const half& a )
{
	// First set the input arguments for the function call:
	Ctl::FunctionArgPtr rArg = call->findInputArg( "r" );
	fillInputArg( rArg, "r", r, n );
	Ctl::FunctionArgPtr gArg = call->findInputArg( "g" );
	fillInputArg( gArg, "g", g, n );
	Ctl::FunctionArgPtr bArg = call->findInputArg( "b" );
	fillInputArg( bArg, "b", b, n );
	Ctl::FunctionArgPtr aArg = call->findInputArg( "a" );
	fillInputArg( aArg, "a", a, n );

	// Now we can call the CTL function for
	// pixels 0, through n-1
	call->callFunction( n );

	// Retrieve the results
	Ctl::FunctionArgPtr rOutArg = call->findOutputArg( "rOut" );
	retrieveOutputArg( rOutArg, "rOut", rOut, n );
	Ctl::FunctionArgPtr gOutArg = call->findOutputArg( "gOut" );
	retrieveOutputArg( gOutArg, "gOut", gOut, n );
	Ctl::FunctionArgPtr bOutArg = call->findOutputArg( "bOut" );
	retrieveOutputArg( bOutArg, "bOut", bOut, n );
	Ctl::FunctionArgPtr aOutArg = call->findOutputArg( "aOut" );
	retrieveOutputArg( aOutArg, "aOut", aOut, n );
}

void callCtl(
	Ctl::Interpreter &interp,
	Ctl::FunctionCallPtr call,
	const std::size_t size,
	half* rOut,
	half* gOut,
	half* bOut,
	half* aOut,
	const half* r,
	const half* g,
	const half* b,
	const half* a )
{
	std::size_t n = size;
	while( n > 0 )
	{
		const std::size_t m = std::min( n, interp.maxSamples() );
		callCtlChunk( call, m, *rOut, *gOut, *bOut, *aOut, *r, *g, *b, *a );

		n    -= m;
		rOut += m;
		gOut += m;
		bOut += m;
		aOut += m;
		r    += m;
		g    += m;
		b    += m;
		a    += m;
	}
}


}

template<class View>
CTLProcess<View>::CTLProcess( CTLPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class View>
void CTLProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

	ctlPlugin = &_plugin;
	Ctl::setMessageOutputFunction( ctlMessageOutput );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void CTLProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	Ctl::SimdInterpreter interpreter;
	Ctl::FunctionCallPtr func = interpreter.newFunctionCall( "process" );

	for( int y = procWindowOutput.y1;
			 y < procWindowOutput.y2;
			 ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
			 x < procWindowOutput.x2;
			 ++x, ++src_it, ++dst_it )
		{
			(*dst_it) = (*src_it);
		}
		if( this->progressForward() )
			return;
	}
	/*
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
							                  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							                  procWindowSize.x, procWindowSize.y );
	copy_pixels( src, dst );
	*/

}

}
}
}
