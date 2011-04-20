#ifndef _TUTTLE_PLUGIN_SAMPLER_HPP_
#define _TUTTLE_PLUGIN_SAMPLER_HPP_


#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/math/constants/constants.hpp>


#include <boost/math/constants/constants.hpp>

#include <cmath>

namespace boost {
namespace gil {


/// \brief A sampler that sets the destination pixel without interpolation from the closest pixels from the source.
// http://www.paulinternet.nl/?page=bicubic
struct ttl_nearest_neighbor_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_nearest_neighbor_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	point2<std::ptrdiff_t> center( iround( p ) );

	if (center.x < -0.5 || center.y < -0.5 || center.x>=src.width() || center.y>=src.height()) return false;

	if( center.x < 0 )
		center.x = 0;
	if( center.y < 0 )
		center.y = 0;
	if( center.x > src.width()-1 )
		center.x = src.width()-1;
	if( center.y > src.height()-1 )
		center.y = src.height()-1;

	result = src( center.x, center.y );
	return true;
}

namespace ttl_detail {

template <typename Weight>
struct add_dst_mul_src_channel
{
	const Weight _w;

	add_dst_mul_src_channel( const Weight w ) : _w( w ) { }

	template <typename SrcChannel, typename DstChannel>
	void operator( )( const SrcChannel& src, DstChannel & dst ) const
	{
		dst += DstChannel( src * _w );
	}
};

template <typename SrcP, typename Weight, typename DstP>
struct add_dst_mul_src
{
	void operator( )( const SrcP& src, const Weight weight, DstP & dst ) const
	{
		static_for_each( src, dst, add_dst_mul_src_channel<Weight>( weight ) );
	}
};

}

/// \brief A sampler that sets the destination pixel as the bilinear interpolation of the four closest pixels from the source.
/// If outside the bounds, it doesn't change the destination
/// \ingroup ImageAlgorithms

struct ttl_bilinear_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_bilinear_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;

	// Interpolation of the four closest pixels
	// _________________________________
	// |               |               |
	// |               |               |
	// |      TL...............TR      |
	// |      :        |       :       |
	// |______:________|_______:_______|
	// |      :    x   |       :       | // x: floating point position of p
	// |      :        |       :       |
	// |      BL...............BR      |
	// |               |               |
	// |_______________|_______________|
	point2<std::ptrdiff_t> pTL( ifloor( p ) ); // the closest integer coordinate top left from p

	point2<F> frac( p.x - pTL.x, p.y - pTL.y );

	if( pTL.x == -1 )
	{
		pTL.x = 0;
		frac.x = 0;
	}
	if( pTL.y == -1 )
	{
		pTL.y = 0;
		frac.y = 0;
	}

	// if we are outside the image
	if( pTL.x < 0 ||
	    pTL.y < 0 ||
	    pTL.x > src.width() - 2 ||
	    pTL.y > src.height() - 2 )
	{
		// to keep the border pixel
		// if we are inside [-0.5,0] or [(width-1), (width-1)+0.5]
		return sample( ttl_nearest_neighbor_sampler(), src, p, result );
	}

	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > PixelW;
	PixelW mp( 0 ); // suboptimal

	typename SrcView::xy_locator loc = src.xy_at( pTL.x, pTL.y );

	if( pTL.x + 1 < src.width( ) )
	{
		if( pTL.y + 1 < src.height( ) )
		{
			// most common case - inside the image, not on the last row or column
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( *loc, ( 1.0 - frac.x )*( 1.0 - frac.y ), mp );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( loc.x()[1], frac.x * ( 1.0 - frac.y ), mp );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( loc.x()[1],  frac.x *(1.0-frac.y),		mp );
			++loc.y( );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( *loc, ( 1.0 - frac.x ) * frac.y, mp );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( loc.x()[1], frac.x * frac.y, mp );
		}
		else
		{
			// on the last row, but not the bottom-right corner pixel
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( *loc, ( 1.0 - frac.x ), mp );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( loc.x()[1], frac.x, mp );
		}
	}
	else
	{
		if( pTL.y + 1 < src.height( ) )
		{
			// on the last column, but not the bottom-right corner pixel
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( *loc, ( 1.0 - frac.y ), mp );
			++loc.y( );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >()( *loc, frac.y, mp );
		}
		else
		{
			// the bottom-right corner pixel
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW >( )( *loc, 1, mp );
		}
	}


	/*
if ( p0.x > 0 )
{
	if ( p0.x+1<src.width() )
	{
		if ( p0.y>0 )
		{
			if (p0.y+1<src.height())
			{
				// most common case - inside the image, not on the last row or column
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	(1.0-frac.x)*(1.0-frac.y)	,mp);
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],	frac.x *(1.0-frac.y)		,mp);
				++loc.y();
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	(1.0-frac.x)*   frac.y		,mp);
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],	frac.x *   frac.y		,mp);
			}
			else
			{
				// on the last row, but not the bottom-right corner pixel
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	(1.0-frac.x)			,mp);
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],	frac.x				,mp);
			}
		}
		else
		{
			if (p0.y+1<src.height())
			{
				// most common case - inside the image, not on the last row or column
				//add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,      (1.0-frac.x)*(1.0-frac.y),mp);
				//add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],   frac.x *(1.0-frac.y),mp);
				++loc.y();
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	(1.0-frac.x)			,mp);
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],	frac.x				,mp);
			}
			else
			{
				++loc.y();
				mp = *loc;
			//add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	1				,mp);
			}
		}

	}
	else
	{
		if ( p0.y>0 )
		{
			if (p0.y+1<src.height())
			{
				// on the last column, but not the bottom-right corner pixel
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	(1.0-frac.y)	,mp);
				++loc.y();
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	frac.y		,mp);
			}
			else
			{
				// the bottom-right corner pixel
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	1		,mp);
			}
		}
		else
		{
			if (p0.y+1<src.height())
			{
				mp = loc.y()[1];
			}
		}
	}
}
else
{
	if (p0.x+1<src.width())
	{
		if ( p0.y>0 )
		{
			if (p0.y+1<src.height())
			{
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],	(1.0-frac.y)	,mp);
				++loc.y();
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],	frac.y		,mp);
			}
			else
			{
				++loc.y();
				mp = loc.x()[1];
			}
		}
		else
		{
			if (p0.y+1<src.height())
			{
				++loc.y();
				mp = loc.x()[1];
			}
		}
	}
	else
	{
		if ( p0.y>0 )
		{
			if (p0.y+1<src.height())
			{
				// on the last column, but not the bottom-right corner pixel
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	(1.0-frac.y)	,mp);
				++loc.y();
				add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,	frac.y		,mp);
			}
			else
			{
									mp = loc.x()[1];
			}
		}
		else
		{
			if (p0.y+1<src.height())
			{
				mp = loc.y()[1];
			}
		}
	}
	}*/

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}



template <typename SrcP, typename F, typename DstP>
struct bicubic1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, F weight, DstP & dst ) const
	{
		DstP mp( 0 ), mp3( 0 ), mp2( 0 ), mp1( 0 );

		// second methos to minimize image merory access
//		const float valueA = weight * ( weight - 1 ); // x (x-1)
//		const float valueB = weight * ( weight - 2 ); // x (x-2)
		const float valueXX	= weight * weight;	// x^2
		const float valueXXX = weight * valueXX;	// x^3

		detail::add_dst_mul_src<SrcP, float, DstP >()(srcA, -valueXXX+2*valueXX-weight,		mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcB, valueXX*(weight-2)+1,	mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcC, -valueXXX+valueXX+weight,	mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcD, valueXX*(weight-1),		mp );

		dst = mp;
	}
};

template <typename SrcP, typename F, typename DstP>
struct keys1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, F weight, DstP & dst ) const
	{
		DstP mp( 0 ), mp3( 0 ), mp2( 0 ), mp1( 0 );

		// fisrt method, but with a lot of access to image memory
		/*
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	,-1, mp3 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, 3, mp3 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	,-3, mp3 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcD	, 1, mp3 );

		ttl_detail::add_dst_mul_src<DstP, float, DstP >()(mp3	, weight*0.5,	mp2 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	, 1,		mp2 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, -2.5,		mp2 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	, 2,		mp2 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcD	, -0.5,		mp2 );

		ttl_detail::add_dst_mul_src<DstP, float, DstP >()(mp2	, weight,	mp1 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	, -0.5,		mp1 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	, 0.5,		mp1 );

		ttl_detail::add_dst_mul_src<DstP, float, DstP >()(mp1	, weight,	mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, 1,		mp );
		*/

		// second methos to minimize image merory access
		const float valueX = weight * 0.5; // x/2
		const float valueXX = weight * valueX; // (x^2)/2
		const float valueXXX = weight - 1; // x-1

		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, -valueX * valueXXX*valueXXX, mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, 1 + valueXX * ( 3 * weight - 5 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, valueX * ( ( -3 * weight + 4 ) * weight + 1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, valueXX*valueXXX, mp );

		dst = mp;
	}
};

template <typename SrcP, typename F, typename DstP>
struct simon1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, F weight, DstP & dst ) const
	{
		DstP mp( 0 ), mp3( 0 ), mp2( 0 ), mp1( 0 );

		// second methos to minimize image merory access
		const float valueX = weight * 0.25; // x/4
		const float valueXX = weight * valueX; // (x^2)/4
		const float valueXXX = weight - 1; // x-1

		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, -3 * valueX * valueXXX*valueXXX, mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, 1 + valueXX * ( 5 * weight - 9 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, valueX * ( ( -5 * weight + 6 ) * weight + 3 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, 3 * valueXX*valueXXX, mp );

		dst = mp;
	}
};

template <typename SrcP, typename F, typename DstP>
struct rifman1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, F weight, DstP & dst ) const
	{
		DstP mp( 0 ), mp3( 0 ), mp2( 0 ), mp1( 0 );

		// second method to minimize image memory access
		const float valueXX = weight * weight; // (x^2)
		const float valueXXX = weight - 1; // x-1

		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, -weight * valueXXX*valueXXX, mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, 1 + valueXX * ( weight - 2 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, weight * ( -valueXXX * weight + 1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, valueXX*valueXXX, mp );

		dst = mp;
	}
};

template < typename xy_locator, typename SrcP, typename diff >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const diff p, int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD )
{
	static const SrcP nullPt( 0 );

	if(p0.x < 0 )
	{
		ptA = loc.x()[1];
		ptB = loc.x()[1];
		ptC = loc.x()[1];
		p0.x+2	< windowWidth ?		ptD = loc.x()[2]	: ptD = ptC;
		return;
	}

	ptB = *loc;
	p0.x	> 0 && p0.x-1	< windowWidth ?	ptA = loc.x()[-1]	: ptA = ptB;
	p0.x+1	< windowWidth ?			ptC = loc.x()[1]	: ptC = *loc;
	p0.x+2	< windowWidth ?			ptD = loc.x()[2]	: ptD = ptC;

}

template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD, SrcP& ptE )
{
	static const SrcP nullPt( 0 );
	ptA = p0.x + 2 > 0 && p0.x - 2 < windowWidth ? loc.x( )[-2] : *loc;
	ptB = p0.x + 1 > 0 && p0.x - 1 < windowWidth ? loc.x( )[-1] : *loc;
	ptC = p0.x < windowWidth ? *loc : *loc;
	ptD = p0.x + 1 < windowWidth ? loc.x( )[1] : *loc;
	ptE = p0.x + 2 < windowWidth ? loc.x( )[2] : *loc;
}

template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD, SrcP& ptE, SrcP& ptF )
{
	static const SrcP nullPt( 0 );
	ptA = p0.x + 2 > 0 && p0.x - 2 < windowWidth ? loc.x( )[-2] : nullPt;
	ptB = p0.x + 1 > 0 && p0.x - 1 < windowWidth ? loc.x( )[-1] : nullPt;
	ptC = p0.x < windowWidth ? *loc : nullPt;
	ptD = p0.x + 1 < windowWidth ? loc.x( )[ 1] : nullPt;
	ptE = p0.x + 2 < windowWidth ? loc.x( )[ 2] : nullPt;
	ptF = p0.x + 3 < windowWidth ? loc.x( )[ 3] : nullPt;
}

template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD, SrcP& ptE, SrcP& ptF, SrcP& ptG )
{
	static const SrcP nullPt( 0 );
	ptA = p0.x + 3 > 0 && p0.x - 3 < windowWidth ? loc.x( )[-3] : nullPt;
	ptB = p0.x + 2 > 0 && p0.x - 2 < windowWidth ? loc.x( )[-2] : nullPt;
	ptC = p0.x + 1 > 0 && p0.x - 1 < windowWidth ? loc.x( )[-1] : nullPt;
	ptD = p0.x < windowWidth ? *loc : nullPt;
	ptE = p0.x + 1 < windowWidth ? loc.x( )[ 1] : nullPt;
	ptF = p0.x + 2 < windowWidth ? loc.x( )[ 2] : nullPt;
	ptG = p0.x + 3 < windowWidth ? loc.x( )[ 3] : nullPt;
}

template< typename F >
F getSincWeight( const F weight )
{
	static const F filterSize = 2.0;
	const F pix = weight * boost::math::constants::pi<F>();
	
	if( weight == 0 )
		return 1;

	return filterSize / ( pix * pix ) * sin( pix ) * sin( pix / filterSize );
}

template <typename SrcP, typename F, typename DstP>
struct lanczos1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, const SrcP& srcE, const SrcP& srcF, const SrcP& srcG, F weight, DstP & dst ) const
	{
		DstP mp( 0 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, getSincWeight( weight + 3 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, getSincWeight( weight + 2 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, getSincWeight( weight + 1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, getSincWeight( weight     ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcE, getSincWeight( weight - 1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcF, getSincWeight( weight - 2 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcG, getSincWeight( weight - 3 ), mp );
		dst = mp;
	}
};


struct ttl_bicubic_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_bicubic_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

	point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 );

	if( p0.y < 0 )
	{
		--loc.y( );
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );

		a1 = a0;
		a2 = a0;

		++loc.y( );
		if ( p0.y+2 < src.height() )
		{
			setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width(), ptA, ptB, ptC, ptD );
			bicubic1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}
	else
	{
	if( p0.y < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a1 );
		}
		if( p0.y !=0 && p0.y-1 < src.height() )
		{
			--loc.y();
			setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width(), ptA, ptB, ptC, ptD );
			bicubic1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a0 );
			++loc.y();
		}
		else
		{
			a0 = a1;
		}

	++loc.y( );
	if( p0.y + 1 < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a2 );
		}
		else
		{
			a2 = a1;
		}
	++loc.y( );
	if( p0.y + 2 < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}

	// vertical process
	bicubic1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, frac.y, mp );

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

struct ttl_keys_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_keys_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

	point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 );

	if( p0.y < 0 )
	{
		++loc.y();
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width(), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a0 );

		a1 = a0;
		a2 = a0;

		++loc.y();
		if ( p0.y+2 < src.height() )
		{
			setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width(), ptA, ptB, ptC, ptD );
			keys1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}
	else
	{
	if( p0.y < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a1 );
		}
	if( p0.y != 0 && p0.y - 1 < src.height( ) )
		{
		--loc.y( );
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );
		++loc.y( );
		}
	else
	{
		a0 = a1;
	}

	++loc.y( );
	if( p0.y + 1 < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a2 );
		}
		else
		{
			a2 = a1;
		}
	++loc.y( );
	if( p0.y + 2 < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}

	// vertical process
	keys1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, frac.y, mp );

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

struct ttl_simon_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_simon_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;
	point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

	//if (p0.x < 0 || p0.y < 0 || p0.x>=src.width() || p0.y>=src.height()) return false;

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 );

	if( p0.y < 0 )
	{
		++loc.y();
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );

		a1 = a0;
		a2 = a0;

		++loc.y( );
		if ( p0.y+2 < src.height() )
		{
			setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width(), ptA, ptB, ptC, ptD );
			simon1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}
	else
	{
	if( p0.y < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a1 );
		}
		if( p0.y !=0 && p0.y-1 < src.height() )
		{
			--loc.y();
			setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width(), ptA, ptB, ptC, ptD );
			simon1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a0 );
			++loc.y();
		}
		else
		{
			a0 = a1;
		}

	++loc.y( );
	if( p0.y + 1 < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a2 );
		}
		else
		{
			a2 = a1;
		}
	++loc.y( );
	if( p0.y + 2 < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}

	// vertical process
	simon1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, frac.y, mp );


	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

struct ttl_rifman_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_rifman_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

	point2<std::ptrdiff_t> p0(ifloor(p)); // the closest integer coordinate top left from p

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 );

	if( p0.y < 0 )
	{
		++loc.y();
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );

		a1 = a0;
		a2 = a0;

		++loc.y( );
		if ( p0.y+2 < src.height() )
		{
			setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width(), ptA, ptB, ptC, ptD );
			rifman1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}
	else
	{
	if( p0.y < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a1 );
		}
		if( p0.y !=0 && p0.y-1 < src.height() )
		{
			--loc.y();
			setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width(), ptA, ptB, ptC, ptD );
			rifman1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a0 );
			++loc.y();
		}
		else
		{
			a0 = a1;
		}

	++loc.y( );
	if( p0.y + 1 < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a2 );
		}
		else
		{
			a2 = a1;
		}
	++loc.y( );
	if( p0.y + 2 < src.height( ) )
		{
		setXPixels<xy_locator, SrcP, point2<F> >( loc, p0, frac, src.width( ), ptA, ptB, ptC, ptD );
		rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}

	// vertical process
	rifman1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, frac.y, mp );


	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

struct ttl_lanczos_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_lanczos_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;
	point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

	//if (p0.x < 0 || p0.y < 0 || p0.x>=src.width() || p0.y>=src.height()) return false;

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	if( p0.x == -1 )
	{
		p0.x = 0;
		frac.x = 0;
	}
	if( p0.y == -1 )
	{
		p0.y = 0;
		frac.y = 0;
	}

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 ), a4( 0 ), a5( 0 ), a6( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 ), ptE( 0 ), ptF( 0 ), ptG( 0 );


	loc.y( ) -= 3;
	if( p0.y + 3 > 0 && p0.y - 3 < src.height( ) )
	{
		setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a0 );
	}
	++loc.y( );
	if( p0.y + 2 > 0 && p0.y - 2 < src.height( ) )
	{
		setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a1 );
	}
	++loc.y( );
	if( p0.y + 1 > 0 && p0.y - 1 < src.height( ) )
	{
		setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a2 );
	}
	++loc.y( );
	if( p0.y < src.height( ) )
	{
		setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a3 );
	}
	++loc.y( );
	if( p0.y + 1 < src.height( ) )
	{
		setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a4 );
	}
	++loc.y( );
	if( p0.y + 2 < src.height( ) )
	{
		setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a5 );
	}
	++loc.y( );
	if( p0.y + 3 < src.height( ) )
	{
		setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a6 );
	}

	// vertical process
	lanczos1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, a4, a5, a6, frac.y, mp );

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( a0, src_result );

	color_convert( src_result, result );
	return true;
}


}
}

#endif
