#include "hd3d_abstract_lut.h"
#include "hd3d_interpolator.h"
#include "hd3d_trilin_interpolator.h"
#include "hd3d_color.h"
#include "lut_reader.h"

namespace tuttle
{

AbstractLut::AbstractLut()
	: _dimSize( 0 ),
	_data( NULL ),
	_shared( false ),
	_interpolator( new TrilinInterpolator() )
{}

AbstractLut::AbstractLut( Interpolator* interpolator, size_t dimSize, double* data /* = NULL */ )
	: _dimSize( dimSize ),
	_data( data ),
	_shared( false )
{}

AbstractLut::AbstractLut( Interpolator* interpolator, LutReader& reader )
	: _dimSize( reader.steps().size() ),
	_data( &reader.data()[0] ),
	_shared( true )
{}

AbstractLut::~AbstractLut()
{
	if( _data && !_shared )
	{
		delete [] _data;
	}
	if( _interpolator )
		delete _interpolator;
}

void AbstractLut::reset( Interpolator* interpolator, LutReader& reader )
{
	if( !_shared && _data )
	{
		delete [] _data;
	}
	_data    = &reader.data()[0];
	_dimSize =  reader.steps().size();
	_shared  =  true;
	if( _interpolator )
		delete _interpolator;
	_interpolator = interpolator;
}

void AbstractLut::reset( LutReader& reader )
{
	if( !_shared && _data )
	{
		delete [] _data;
	}
	_data    = &reader.data()[0];
	_dimSize =  reader.steps().size();
	_shared  =  true;
	if( _interpolator )
		delete _interpolator;
	_interpolator = new TrilinInterpolator();
}

void AbstractLut::reset( size_t dimSize, double* data /* = NULL */ )
{
	if( _data && !_shared )
		delete [] _data;
	_dimSize = _dimSize;
	if( _data == NULL )
	{
		_data = new double[ totalSize() * 3 ];
		memset( _data, 0, sizeof( double ) * totalSize() * 3 );
	}
	_data   = data;
	_shared = false;
	if( _interpolator )
		delete _interpolator;
	_interpolator = new TrilinInterpolator();
}

};
