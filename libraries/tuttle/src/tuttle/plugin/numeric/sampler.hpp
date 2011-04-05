#ifndef TUTTLE_SAMPLER_HPP
#define TUTTLE_SAMPLER_HPP

#define PI 3.14159265
#include <cmath>

#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

/// \brief A sampler that sets the destination pixel as the bicubic interpolation of the four closest pixels from the source.


// http://www.paulinternet.nl/?page=bicubic
using namespace boost::gil;


template <typename SrcP, typename F, typename DstP>
struct bicubic1D {

	void operator()(const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD , F weight, DstP& dst) const {
		DstP mp(0), mp3(0), mp2(0), mp1(0);

		// second methos to minimize image merory access
		float valueA	= weight * (weight-1);		// x (x-1)
		float valueB	= weight * (weight-2);		// x (x-2)

		detail::add_dst_mul_src<SrcP, float, DstP >()(srcA, -weight*(1+valueB),		mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcB, 1+(weight*valueB),		mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcC, weight*(1-valueA),		mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcD, weight*valueA,		mp );

		dst = mp;
	}
};


template <typename SrcP, typename F, typename DstP>
struct keys1D {

	void operator()(const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD , F weight, DstP& dst) const {
		DstP mp(0), mp3(0), mp2(0), mp1(0);

		// fisrt method, but with a lot of access to image memory
		/*detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	,-1, mp3 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, 3, mp3 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	,-3, mp3 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcD	, 1, mp3 );

		detail::add_dst_mul_src<DstP, float, DstP >()(mp3	, weight*0.5,	mp2 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	, 1,		mp2 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, -2.5,		mp2 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	, 2,		mp2 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcD	, -0.5,		mp2 );

		detail::add_dst_mul_src<DstP, float, DstP >()(mp2	, weight,	mp1 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	, -0.5,		mp1 );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	, 0.5,		mp1 );

		detail::add_dst_mul_src<DstP, float, DstP >()(mp1	, weight,	mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, 1,		mp );*/

		// second methos to minimize image merory access
		float valueX	= weight * 0.5;		// x/2
		float valueXX	= weight * valueX;	// (x^2)/2
		float valueXXX	= weight -1;		// x-1

		detail::add_dst_mul_src<SrcP, float, DstP >()(srcA, -valueX*valueXXX*valueXXX,		mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcB, 1+valueXX*(3*weight-5),		mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcC, valueX*( (-3*weight+4)*weight+1),	mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcD, valueXX*valueXXX,			mp );

		dst = mp;
	}
};


template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD )
{
	SrcP nullPt(0);
	/*if(p0.x<0)
	{
		p0.x > 1.5 && p0.x-1 < windowWidth ?	ptA = loc.x()[-2]	: ptA = nullPt;
		p0.x > 0.5 && p0.x   < windowWidth ?	ptB = loc.x()[-1]	: ptB = nullPt;
		p0.x+1 < windowWidth ?			ptC = *loc		: ptC = nullPt;
		p0.x+2 < windowWidth ?			ptD = loc.x()[1]	: ptD = nullPt;
	}*/
	if((p0.x -(int)p0.x) < 0.5)
	{
		p0.x > 1.5 && p0.x-1 < windowWidth ?	ptA = loc.x()[-2]	: ptA = nullPt;
		p0.x > 0.5 && p0.x   < windowWidth ?	ptB = loc.x()[-1]	: ptB = nullPt;
		p0.x+1 < windowWidth ?			ptC = *loc		: ptC = nullPt;
		p0.x+2 < windowWidth ?			ptD = loc.x()[1]	: ptD = nullPt;
	}
	else
	{
		p0.x > 1.5 && p0.x-1 < windowWidth ?	ptA = loc.x()[-1]	: ptA = nullPt;
		p0.x > 0.5 && p0.x   < windowWidth ?	ptB = *loc		: ptB = nullPt;
		p0.x+1 < windowWidth ?			ptC = loc.x()[1]	: ptC = nullPt;
		p0.x+2 < windowWidth ?			ptD = loc.x()[2]	: ptD = nullPt;
	}

}

template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD, SrcP& ptE )
{
	SrcP nullPt(0);
	p0.x+2 > 0 && p0.x-2 < windowWidth ?	ptA = loc.x()[-2]	: ptA = nullPt;
	p0.x+1 > 0 && p0.x-1 < windowWidth ?	ptB = loc.x()[-1]	: ptB = nullPt;
	p0.x   < windowWidth ?			ptC = *loc		: ptC = nullPt;
	p0.x+1 < windowWidth ?			ptD = loc.x()[1]	: ptD = nullPt;
	p0.x+2 < windowWidth ?			ptE = loc.x()[2]	: ptE = nullPt;
}

template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD, SrcP& ptE, SrcP& ptF )
{
	SrcP nullPt(0);
	p0.x+2 > 0 && p0.x-2 < windowWidth ?	ptA = loc.x()[-2]	: ptA = nullPt;
	p0.x+1 > 0 && p0.x-1 < windowWidth ?	ptB = loc.x()[-1]	: ptB = nullPt;
	p0.x   < windowWidth ?			ptC = *loc		: ptC = nullPt;
	p0.x+1 < windowWidth ?			ptD = loc.x()[ 1]	: ptD = nullPt;
	p0.x+2 < windowWidth ?			ptE = loc.x()[ 2]	: ptE = nullPt;
	p0.x+3 < windowWidth ?			ptF = loc.x()[ 3]	: ptF = nullPt;
}

template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD, SrcP& ptE, SrcP& ptF, SrcP& ptG )
{
	SrcP nullPt(0);
	p0.x+3 > 0 && p0.x-3 < windowWidth ?	ptA = loc.x()[-3]	: ptA = nullPt;
	p0.x+2 > 0 && p0.x-2 < windowWidth ?	ptB = loc.x()[-2]	: ptB = nullPt;
	p0.x+1 > 0 && p0.x-1 < windowWidth ?	ptC = loc.x()[-1]	: ptC = nullPt;
	p0.x   < windowWidth ?			ptD = *loc		: ptD = nullPt;
	p0.x+1 < windowWidth ?			ptE = loc.x()[ 1]	: ptE = nullPt;
	p0.x+2 < windowWidth ?			ptF = loc.x()[ 2]	: ptF = nullPt;
	p0.x+3 < windowWidth ?			ptG = loc.x()[ 3]	: ptG = nullPt;
}

template< typename F >
F getSincWeight(F weight)
{
	F filterSize = 2.0;
	F pix = weight*PI;
	if(weight==0)
		return 1;

	return filterSize/(pix*pix)*sin(pix)*sin(pix/filterSize);
}

template <typename SrcP, typename F, typename DstP>
struct lanczos1D {

	void operator()(const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, const SrcP& srcE, const SrcP& srcF, const SrcP& srcG , F weight, DstP& dst) const
	{
		DstP mp(0);
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcA, getSincWeight(weight+3) , mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcB, getSincWeight(weight+2) , mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcC, getSincWeight(weight+1) , mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcD, getSincWeight(weight  ) , mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcE, getSincWeight(weight-1) , mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcF, getSincWeight(weight-2) , mp );
		detail::add_dst_mul_src<SrcP, float, DstP >()(srcG, getSincWeight(weight-3) , mp );
		dst = mp;
	}
};

struct bicubic_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( bicubic_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{

	typedef typename SrcView::value_type SrcP;
	typedef pixel<F,devicen_layout_t<num_channels<SrcView>::value> > SrcC;
	point2<std::ptrdiff_t> p0(ifloor(p)); // the closest integer coordinate top left from p

	if (p0.x < 0 || p0.y < 0 || p0.x>=src.width() || p0.y>=src.height()) return false;

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp(0);
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac(p.x-p0.x, p.y-p0.y);

	SrcC  a0(0), a1(0), a2(0), a3(0);

	SrcP ptA(0), ptB(0), ptC(0), ptD(0);



	if( p0.y != 0 && p0.y-1 < src.height() )
	{
		--loc.y();
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD );
		bicubic1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a0 );
		++loc.y();
	}
	if ( p0.y   < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD );
		bicubic1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a1 );
	}
	++loc.y();
	if ( p0.y+1 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD );
		bicubic1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a2 );
	}
	++loc.y();
	if ( p0.y+2 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD );
		bicubic1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a3 );
	}

	// vertical process
	bicubic1D< SrcC, F, SrcC >()( a0, a1, a2, a3, frac.y, mp );


	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

struct keys_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( keys_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{

	typedef typename SrcView::value_type SrcP;
	typedef pixel<F,devicen_layout_t<num_channels<SrcView>::value> > SrcC;
	point2<std::ptrdiff_t> p0(ifloor(p)); // the closest integer coordinate top left from p

	if (p0.x < 0 || p0.y < 0 || p0.x>=src.width() || p0.y>=src.height()) return false;

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp(0);
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac(p.x-p0.x, p.y-p0.y);

	SrcC  a0(0), a1(0), a2(0), a3(0);

	SrcP ptA(0), ptB(0), ptC(0), ptD(0);

	if(frac.x < 0.5)
		frac.x +=0.5;
	if(frac.x > 0.5)
		frac.x += 0;

	if( p0.y != 0 && p0.y-1 < src.height() )
	{
		--loc.y();
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a0 );
		++loc.y();
	}
	if ( p0.y   < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a1 );
	}
	++loc.y();
	if ( p0.y+1 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a2 );
	}
	++loc.y();
	if ( p0.y+2 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, frac.x, a3 );
	}

	// vertical process
	keys1D< SrcC, F, SrcC >()( a0, a1, a2, a3, frac.y, mp );


	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( a1, src_result );

	color_convert( src_result, result );
	return true;
}


struct lanczos_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( lanczos_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{

	typedef typename SrcView::value_type SrcP;
	typedef pixel<F,devicen_layout_t<num_channels<SrcView>::value> > SrcC;
	point2<std::ptrdiff_t> p0(ifloor(p)); // the closest integer coordinate top left from p

	if (p0.x < 0 || p0.y < 0 || p0.x>=src.width() || p0.y>=src.height()) return false;

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp(0);
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac(p.x-p0.x, p.y-p0.y);

	SrcC  a0(0), a1(0), a2(0), a3(0), a4(0), a5(0), a6(0);

	SrcP ptA(0), ptB(0), ptC(0), ptD(0), ptE(0), ptF(0), ptG(0);


	loc.y() -= 3;
	if( p0.y+3 > 0 && p0.y-3 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a0 );
	}
	++loc.y();
	if( p0.y+2 > 0 && p0.y-2 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a1 );
	}
	++loc.y();
	if( p0.y+1 > 0 && p0.y-1 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a2 );
	}
	++loc.y();
	if ( p0.y   < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a3 );
	}
	++loc.y();
	if ( p0.y+1 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a4 );
	}
	++loc.y();
	if ( p0.y+2 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a5 );
	}
	++loc.y();
	if ( p0.y+3 < src.height() )
	{
		setXPixels<xy_locator, SrcP>( loc, p0, src.width(), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		lanczos1D< SrcP, F, SrcC >()( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a6 );
	}

	// vertical process
	lanczos1D< SrcC, F, SrcC >()( a0, a1, a2, a3, a4, a5, a6, frac.y, mp );


	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

#endif
