#include "Bezier/bezier.hpp"

#include <boost/math/special_functions/pow.hpp>
#include <boost/math/special_functions/math_fwd.hpp>

namespace tuttle {
namespace plugin {
namespace warp {
namespace bezier {

double barycentre( const double a, const double b, const double c, const double d, const double t )
{
	using boost::math::pow;
	//B(t) = P0(1-t)^3 + 3 P1 t (1-t)^2 + 3 P2 t^2 (1-t) + P3 t^3;
	return ( a*pow<3>( 1 - t ) )+( 3 * b * t * pow<2>( 1 - t ) )+( 3 * c * pow<2>(t) * ( 1 - t ) )+( d * pow<3>(t) );
}

point2<double> barycentre( const std::vector< point2<double> >& pts, const double t )
{
	point2<double> p;
	p.x = barycentre( pts[0].x, pts[1].x, pts[2].x, pts[3].x, t );
	p.y = barycentre( pts[0].y, pts[1].y, pts[2].y, pts[3].y, t );
	return p;
}

void tracerPoint( const point2<double>& p )
{
	//std::cout<<"--------------TracePoints--------------"<<std::endl;
	glVertex2f( p.x, p.y );
}

void drawBezier( const std::vector< point2<double> >& tabPts, const std::size_t nbBezier, const double r, const double v, const double b )
{
	//std::cout<<"--------------DessineRecur--------------"<<std::endl;
	for( std::size_t i = 0; i < nbBezier; ++i )
	{
		double t = (double(nbBezier ) - i ) / double(nbBezier );

		point2<double> tab = barycentre( tabPts, t );

		//_tgtPointsBezier.push_back( tab );

		glPointSize( pointWidth );
		glColor3f( r, v, b );
		glBegin( GL_POINTS );
		tracerPoint( tab );
		glEnd( );
	}
}

void bezierSubdivide( const std::vector< point2<double> >& pts, const std::size_t subdivs, std::vector< point2<double> >& outputPts )
{
	const std::size_t fullSubdivs = subdivs + 1;
	for( std::size_t i = 1; i < fullSubdivs; ++i )
	{
		const double t = i / double( fullSubdivs );
		outputPts.push_back( barycentre( pts, t ) );
	}
}


}
}
}
}


